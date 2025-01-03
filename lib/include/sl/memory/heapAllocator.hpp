#pragma once

#include <cstdint>
#include <map>

#include "sl/core.hpp"
#include "sl/memory/poolAllocator.hpp"
#include "sl/utils/units.hpp"


namespace sl::memory {
	using namespace sl::utils::literals;

	class SL_CORE HeapAllocator final {
		public:
			using value_type = std::byte;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			HeapAllocator(
				sl::utils::Bytes pageSize = 32_MiB,
				size_type maxPageCount = 16,
				sl::utils::Bytes averageAllocationSize = 4 * sizeof(std::uintmax_t)
			) noexcept;
			~HeapAllocator();

			HeapAllocator(const HeapAllocator &allocator) noexcept;
			auto operator=(const HeapAllocator &allocator) noexcept -> HeapAllocator&;
			HeapAllocator(HeapAllocator &&allocator) noexcept;
			auto operator=(HeapAllocator &&allocator) noexcept -> HeapAllocator&;

			[[nodiscard]]
			auto allocate(size_type size, size_type alignement = 1) noexcept -> pointer;
			auto deallocate(const pointer &ptr) noexcept -> void;


		private:
			struct Allocation {
				value_type *start;
				value_type *end;
			};

			sl::utils::Bytes m_pageSize;
			size_type m_pageCount;
			size_type m_maxPageCount;
			sl::utils::Bytes m_maxAllocationPageSize;
			sl::memory::PoolAllocator<value_type*> m_pages;
			sl::memory::PoolAllocator<sl::memory::PoolAllocator<Allocation>> m_allocationPages;
			size_type m_instanceID;

			static size_type s_lastInstanceID;
			static std::map<size_type, size_type> s_instanceCounts;
	};

} // namespace sl::memory
