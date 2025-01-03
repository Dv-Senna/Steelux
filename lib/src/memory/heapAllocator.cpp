#include "sl/memory/heapAllocator.hpp"


namespace sl::memory {
	HeapAllocator::HeapAllocator(sl::utils::Bytes pageSize, size_type maxPageCount, sl::utils::Bytes averageAllocationSize) noexcept :
		m_pageSize {pageSize},
		m_pageCount {1},
		m_maxPageCount {maxPageCount},
		m_maxAllocationPageSize {m_pageSize / averageAllocationSize},
		m_pages {m_maxPageCount},
		m_allocationPages {m_maxPageCount},
		m_instanceID {++s_lastInstanceID}
	{
		*m_pages.allocate() = new value_type[m_pageSize];
		(void)m_allocationPages.allocate(1, m_maxAllocationPageSize);
		s_instanceCounts[m_instanceID] = 1;
	}


	HeapAllocator::~HeapAllocator() {
		if (m_instanceID == 0)
			return;
		--s_instanceCounts[m_instanceID];
		if (s_instanceCounts[m_instanceID] != 0)
			return;

		for (const auto &page : m_pages)
			delete[] page;
	}


	HeapAllocator::HeapAllocator(const HeapAllocator &allocator) noexcept :
		m_pageSize {allocator.m_pageSize},
		m_pageCount {allocator.m_pageCount},
		m_maxPageCount {allocator.m_maxPageCount},
		m_maxAllocationPageSize {allocator.m_maxAllocationPageSize},
		m_pages {allocator.m_pages},
		m_allocationPages {allocator.m_allocationPages},
		m_instanceID {allocator.m_instanceID}
	{
		++s_instanceCounts[m_instanceID];
	}


	auto HeapAllocator::operator=(const HeapAllocator &allocator) noexcept -> HeapAllocator& {
		this->~HeapAllocator();

		m_pageSize = allocator.m_pageSize;
		m_pageCount = allocator.m_pageCount;
		m_maxPageCount = allocator.m_maxPageCount;
		m_maxAllocationPageSize = allocator.m_maxAllocationPageSize;
		m_pages = allocator.m_pages;
		m_allocationPages = allocator.m_allocationPages;
		m_instanceID = ++s_lastInstanceID;

		++s_instanceCounts[m_instanceID];

		return *this;
	}


	HeapAllocator::HeapAllocator(HeapAllocator &&allocator) noexcept :
		m_pageSize {allocator.m_pageSize},
		m_pageCount {allocator.m_pageCount},
		m_maxPageCount {allocator.m_maxPageCount},
		m_maxAllocationPageSize {allocator.m_maxAllocationPageSize},
		m_pages {std::move(allocator.m_pages)},
		m_allocationPages {std::move(allocator.m_allocationPages)},
		m_instanceID {allocator.m_instanceID}
	{
		allocator.m_pageSize = 0_B;
		allocator.m_pageCount = 0;
		allocator.m_maxPageCount = 0;
		allocator.m_maxAllocationPageSize = 0_B;
		allocator.m_instanceID = 0;
	}


	auto HeapAllocator::operator=(HeapAllocator &&allocator) noexcept -> HeapAllocator& {
		this->~HeapAllocator();

		m_pageSize = allocator.m_pageSize;
		m_pageCount = allocator.m_pageCount;
		m_maxPageCount = allocator.m_maxPageCount;
		m_maxAllocationPageSize = allocator.m_maxAllocationPageSize;
		m_pages = std::move(allocator.m_pages);
		m_allocationPages = std::move(allocator.m_allocationPages);
		m_instanceID = allocator.m_instanceID;

		allocator.m_pageSize = 0_B;
		allocator.m_pageCount = 0;
		allocator.m_maxPageCount = 0;
		allocator.m_maxAllocationPageSize = 0_B;
		allocator.m_instanceID = 0;

		return *this;
	}


	auto HeapAllocator::allocate(size_type size, size_type alignement) noexcept -> HeapAllocator::pointer {
		return nullptr;
	}


	auto HeapAllocator::deallocate(const pointer &ptr) noexcept -> void {

	}


	HeapAllocator::size_type HeapAllocator::s_lastInstanceID {0};
	std::map<HeapAllocator::size_type, HeapAllocator::size_type> HeapAllocator::s_instanceCounts {};
} // namespace sl::memory
