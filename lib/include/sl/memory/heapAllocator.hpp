#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include "sl/core.hpp"
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

			HeapAllocator(sl::utils::Bytes pageSize = 32_MiB, sl::utils::Bytes averageAllocationSize = 2 * sizeof(std::uintmax_t)) noexcept;
			~HeapAllocator();

			HeapAllocator(const HeapAllocator &allocator) noexcept;
			HeapAllocator &operator=(const HeapAllocator &allocator) noexcept;
			HeapAllocator(HeapAllocator &&allocator) noexcept;
			HeapAllocator &operator=(HeapAllocator &&allocator) noexcept;

			pointer allocate(size_type size, size_type alignement = 1) noexcept;
			void deallocate(const pointer &ptr) noexcept;


		private:
			struct Allocation {
				value_type *start;
				value_type *end;
			};

			size_type m_pageSize;
			size_type m_maxAllocationPageSize;
			std::vector<value_type*> m_pages;
			std::vector<Allocation*> m_allocationPages;
			size_type m_instanceID;

			static size_type s_lastInstanceID;
			static std::map<size_type, size_type> s_instanceCounts;
	};

} // namespace sl::memory
