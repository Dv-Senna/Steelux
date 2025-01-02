#include "sl/memory/heapAllocator.hpp"


namespace sl::memory {
	HeapAllocator::HeapAllocator(sl::utils::Bytes pageSize, sl::utils::Bytes averageAllocationSize) noexcept :
		m_pageSize {static_cast<std::size_t> (pageSize)},
		m_maxAllocationPageSize {m_pageSize / averageAllocationSize},
		m_pages {new value_type[m_pageSize]},
		m_allocationPages {new Allocation[m_maxAllocationPageSize]},
		m_instanceID {++s_lastInstanceID}
	{
		s_instanceCounts[m_instanceID] = 1;
	}


	HeapAllocator::~HeapAllocator() {
		if (m_instanceID == 0)
			return;
		--s_instanceCounts[m_instanceID];
		if (s_instanceCounts[m_instanceID] != 0)
			return;

		for (auto &page : m_pages)
			delete[] page;
		for (auto &allocationPage : m_allocationPages)
			delete[] allocationPage;
	}


	HeapAllocator::HeapAllocator(const HeapAllocator &allocator) noexcept :
		m_pageSize {allocator.m_pageSize},
		m_maxAllocationPageSize {allocator.m_maxAllocationPageSize},
		m_pages {allocator.m_pages},
		m_allocationPages {allocator.m_allocationPages},
		m_instanceID {++s_lastInstanceID}
	{

	}


/*	HeapAllocator &HeapAllocator::operator=(const HeapAllocator &allocator) noexcept {
		return *this;
	}


	HeapAllocator::HeapAllocator(HeapAllocator &&allocator) noexcept {

	}


	HeapAllocator &HeapAllocator::operator=(HeapAllocator &&allocator) noexcept {

	}


	HeapAllocator::pointer HeapAllocator::allocate(size_type size, size_type alignement) noexcept {

	}


	void HeapAllocator::deallocate(const pointer &ptr) noexcept {

	}
*/

	HeapAllocator::size_type HeapAllocator::s_lastInstanceID {0};
	std::map<HeapAllocator::size_type, HeapAllocator::size_type> HeapAllocator::s_instanceCounts {};
} // namespace sl::memory
