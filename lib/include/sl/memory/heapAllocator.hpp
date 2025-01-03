#pragma once

#include <cstdint>
#include <optional>
#include <vector>

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

			HeapAllocator(const HeapAllocator &allocator) noexcept = delete;
			auto operator=(const HeapAllocator &allocator) noexcept -> HeapAllocator& = delete;
			HeapAllocator(HeapAllocator &&allocator) noexcept;
			auto operator=(HeapAllocator &&allocator) noexcept -> HeapAllocator&;

			[[nodiscard]]
			auto allocate(size_type size, size_type alignment = 1) noexcept -> pointer;
			auto deallocate(const pointer &ptr) noexcept -> void;


		private:
			struct Allocation {
				value_type **start;
				size_type size;
			};

			static auto s_alignToAlignment(value_type *ptr, size_type alignment) noexcept -> value_type*;

			static auto s_findEmptySpot(
				value_type *page,
				size_type pageSize,
				std::vector<Allocation> &allocations,
				size_type size,
				size_type alignment
			) noexcept -> std::optional<std::pair<decltype(allocations.begin()), value_type*>>;

			sl::utils::Bytes m_pageSize;
			size_type m_pageCount;
			size_type m_maxPageCount;
			sl::utils::Bytes m_maxAllocationPageSize;
			sl::memory::PoolAllocator<value_type*> m_pages;
			sl::memory::PoolAllocator<value_type*> m_pTable;
			sl::memory::PoolAllocator<std::vector<Allocation>> m_allocationPages;
	};


	template <typename T>
	class HeapAllocatorView final {
		public:
			using value_type = T;
			using pointer = T*;
			using const_pointer = const T*;
			using reference = T&;
			using const_reference = const T&;
			using size_type = HeapAllocator::size_type;
			using difference_type = HeapAllocator::difference_type;
			using is_always_equal = std::false_type;

			inline HeapAllocatorView(HeapAllocator &heapAllocator) noexcept : m_heapAllocator {&heapAllocator} {}
			inline HeapAllocatorView(const HeapAllocatorView<T> &) noexcept = default;
			inline auto operator=(const HeapAllocatorView<T> &) noexcept -> HeapAllocatorView<T>& = default;
			inline HeapAllocatorView(HeapAllocatorView<T> &&) noexcept = default;
			inline auto operator=(HeapAllocatorView<T> &&) noexcept -> HeapAllocatorView<T>& = default;

			[[nodiscard]]
			inline auto allocate(size_type n) noexcept -> pointer {return reinterpret_cast<pointer> (m_heapAllocator->allocate(sizeof(T) * n, alignof(T)));}
			inline auto deallocate(const pointer &ptr, size_type) noexcept -> void {m_heapAllocator->deallocate(reinterpret_cast<HeapAllocator::pointer> (ptr));}

			inline auto operator==(const HeapAllocatorView<T> &view) const noexcept -> bool {return m_heapAllocator == view.m_heapAllocator;}

		private:
			HeapAllocator *m_heapAllocator;
	};

	static_assert(sl::memory::IsAllocator<HeapAllocatorView<char>>);

} // namespace sl::memory
