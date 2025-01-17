#include "sl/memory/heapAllocator.hpp"

#include "sl/utils/logger.hpp"


namespace sl::memory {
	HeapAllocator::HeapAllocator(sl::utils::Bytes pageSize, size_type maxPageCount, sl::utils::Bytes averageAllocationSize) noexcept :
		m_pageSize {pageSize},
		m_pageCount {1},
		m_maxPageCount {maxPageCount},
		m_maxAllocationPageSize {m_pageSize / averageAllocationSize},
		m_pages {m_maxPageCount},
		m_pTable {m_maxPageCount * m_maxAllocationPageSize},
		m_allocationPages {m_maxPageCount}
	{
		*m_pages.allocate() = new value_type[m_pageSize];
		std::vector<Allocation> *allocationPage {m_allocationPages.allocate()};
		new (allocationPage) std::vector<Allocation> ();
		allocationPage->reserve(m_maxAllocationPageSize);
	}


	HeapAllocator::~HeapAllocator() {
		for (auto &allocationPage : m_allocationPages)
			allocationPage.~vector<Allocation> ();

		for (const auto &page : m_pages)
			delete[] page;
	}


	HeapAllocator::HeapAllocator(HeapAllocator &&allocator) noexcept :
		m_pageSize {allocator.m_pageSize},
		m_pageCount {allocator.m_pageCount},
		m_maxPageCount {allocator.m_maxPageCount},
		m_maxAllocationPageSize {allocator.m_maxAllocationPageSize},
		m_pages {std::move(allocator.m_pages)},
		m_pTable {std::move(allocator.m_pTable)},
		m_allocationPages {std::move(allocator.m_allocationPages)}
	{
		allocator.m_pageSize = 0_B;
		allocator.m_pageCount = 0;
		allocator.m_maxPageCount = 0;
		allocator.m_maxAllocationPageSize = 0_B;
	}


	auto HeapAllocator::operator=(HeapAllocator &&allocator) noexcept -> HeapAllocator& {
		this->~HeapAllocator();

		m_pageSize = allocator.m_pageSize;
		m_pageCount = allocator.m_pageCount;
		m_maxPageCount = allocator.m_maxPageCount;
		m_maxAllocationPageSize = allocator.m_maxAllocationPageSize;
		m_pages = std::move(allocator.m_pages);
		m_pTable = std::move(allocator.m_pTable);
		m_allocationPages = std::move(allocator.m_allocationPages);

		allocator.m_pageSize = 0_B;
		allocator.m_pageCount = 0;
		allocator.m_maxPageCount = 0;
		allocator.m_maxAllocationPageSize = 0_B;

		return *this;
	}


	auto HeapAllocator::allocate(size_type size, size_type alignment) noexcept -> HeapAllocator::pointer {
		SL_TEXT_ASSERT(size <= m_pageSize, "Can't allocate more memory at once that what a page of HeapAllocator can contain");

		auto page {m_pages.begin()};
		decltype(m_allocationPages.begin()->begin()) allocationPosition {};
		auto allocationPage {m_allocationPages.begin()};
		value_type *address {nullptr};

		for (; page != m_pages.end(); ++page, ++allocationPage) {
			auto emptySpot {s_findEmptySpot(*page, m_pageSize, *allocationPage, size, alignment)};
			if (!emptySpot)
				continue;

			allocationPosition = emptySpot.value().first;
			address = emptySpot.value().second;
			break;
		}

		if (page == m_pages.end()) {
			if (m_pageCount >= m_maxPageCount)
				return nullptr;

			page = m_pages.allocateIt();
			*page = new value_type[m_pageSize];
			allocationPage = m_allocationPages.allocateIt();
			allocationPage->reserve(m_maxAllocationPageSize);
			allocationPosition = allocationPage->end();
			address = s_alignToAlignment(*page, alignment);
		}

		value_type **pTableEntry {m_pTable.allocate()};
		*pTableEntry = address;
		allocationPage->insert(allocationPosition, Allocation{pTableEntry, size, alignment});

		return pointer(pTableEntry);
	}


	auto HeapAllocator::deallocate(const pointer &ptr) noexcept -> void {
		for (auto it {m_pTable.begin()}; it != m_pTable.end(); ++it) {
			if (&*it != ptr.m_pTableEntry)
				continue;
			m_pTable.deallocate(&*it);
			break;
		}

		for (auto &allocationPage : m_allocationPages) {
			for (auto it {allocationPage.begin()}; it != allocationPage.end(); ++it) {
				if (it->start != ptr.m_pTableEntry)
					continue;
				allocationPage.erase(it);
				return;
			}
		}
	}


	auto HeapAllocator::defragment(size_type maxRelocationCount) noexcept -> void {
		size_type relocationCount {0};

		auto page {m_pages.begin()};
		auto allocationPage {m_allocationPages.begin()};

		for (; page != m_pages.end(); ++page, ++allocationPage) {
			value_type *lastAllocationEnd {*page};

			for (auto &allocation : *allocationPage) {
				if (relocationCount >= maxRelocationCount)
					return;

				size_type offset {static_cast<size_type> (*allocation.start - lastAllocationEnd)};
				if (offset < allocation.alignment) {
					lastAllocationEnd = *allocation.start + allocation.size;
					continue;
				}

				value_type *newAddress {s_alignToAlignment(lastAllocationEnd, allocation.alignment)};
				(void)std::memmove(newAddress, *allocation.start, allocation.size);
				*allocation.start = newAddress;
				lastAllocationEnd = *allocation.start + allocation.size;
				++relocationCount;
			}
		}
	}


	auto HeapAllocator::findInPTable(std::byte *ptr) noexcept -> pointer {
		for (auto &pTableEntry : m_pTable) {
			if (pTableEntry != ptr)
				continue;
			return pointer(&pTableEntry);
		}

		return nullptr;
	}


	auto HeapAllocator::s_alignToAlignment(value_type *ptr, size_type alignment) noexcept -> value_type* {
		if (reinterpret_cast<size_type> (ptr) % alignment == 0)
			return ptr;
		return ptr + alignment - (reinterpret_cast<size_type> (ptr) % alignment);
	}


	auto HeapAllocator::s_findEmptySpot(
		value_type *page,
		size_type pageSize,
		std::vector<Allocation> &allocations,
		size_type size,
		size_type alignment
	) noexcept -> std::optional<std::pair<decltype(allocations.begin()), value_type*>> {
		const auto isSpotValid = [alignment, size](value_type *lastEnd, value_type *start) -> bool {
			lastEnd = s_alignToAlignment(lastEnd, alignment);
			return start - lastEnd >= static_cast<std::ptrdiff_t> (size);
		};

		value_type *lastEnd {page};

		for (auto it {allocations.begin()}; it != allocations.end(); ++it) {
			value_type *start {*it->start};
			value_type *end {start + it->size};

			if (isSpotValid(lastEnd, start))
				return std::make_optional(std::make_pair(it, s_alignToAlignment(lastEnd, alignment)));
			lastEnd = end;
		}

		if (isSpotValid(lastEnd, page + pageSize))
			return std::make_optional(std::make_pair(allocations.end(), s_alignToAlignment(lastEnd, alignment)));
		return std::nullopt;
	}

} // namespace sl::memory
