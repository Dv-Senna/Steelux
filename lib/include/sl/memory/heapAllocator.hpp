#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "sl/core.hpp"
#include "sl/memory/poolAllocator.hpp"
#include "sl/utils/units.hpp"


namespace sl::memory {
	using namespace sl::utils::literals;

	class HeapAllocator;

	template <typename T>
	class HeapAllocatorPointer final {
		friend class HeapAllocator;

		public:
			using iterator_category = std::contiguous_iterator_tag;
			using element_type = T;
			using difference_type = std::ptrdiff_t;
			template <typename U>
			using rebind = HeapAllocatorPointer<U>;

			constexpr HeapAllocatorPointer() noexcept = default;
			constexpr HeapAllocatorPointer(std::byte **pTableEntry, ptrdiff_t offset = 0) noexcept : m_pTableEntry {pTableEntry}, m_offset {offset} {}

			constexpr HeapAllocatorPointer(const HeapAllocatorPointer<T> &) noexcept = default;
			constexpr auto operator=(const HeapAllocatorPointer<T> &) noexcept -> HeapAllocatorPointer<T>& = default;
			constexpr HeapAllocatorPointer(HeapAllocatorPointer<T> &&) noexcept = default;
			constexpr auto operator=(HeapAllocatorPointer<T> &&) noexcept -> HeapAllocatorPointer<T>& = default;

			constexpr auto operator==(const HeapAllocatorPointer<T> &pointer) const noexcept -> bool {
				return m_pTableEntry == pointer.m_pTableEntry && m_offset == pointer.m_offset;
			}
			constexpr auto operator<=>(const HeapAllocatorPointer<T> &pointer) const noexcept {
				return *m_pTableEntry + m_offset <=> *pointer.m_pTableEntry + pointer.m_offset;
			}

			constexpr auto operator++() noexcept -> HeapAllocatorPointer<T>& {m_offset += sizeof(T); return *this;}
			constexpr auto operator++(int) noexcept -> HeapAllocatorPointer<T> {auto tmp {*this}; ++(*this); return tmp;}
			constexpr auto operator--() noexcept -> HeapAllocatorPointer<T>& {m_offset -= sizeof(T); return *this;}
			constexpr auto operator--(int) noexcept -> HeapAllocatorPointer<T> {auto tmp {*this}; --(*this); return tmp;}

			constexpr auto operator+=(difference_type offset) noexcept -> HeapAllocatorPointer<T>& {m_offset += sizeof(T) * offset; return *this;}
			constexpr auto operator-=(difference_type offset) noexcept -> HeapAllocatorPointer<T>& {m_offset -= sizeof(T) * offset; return *this;}

			constexpr auto operator+(difference_type offset) const noexcept -> HeapAllocatorPointer<T> {auto tmp {*this}; return tmp += offset;}
			constexpr auto operator-(difference_type offset) const noexcept -> HeapAllocatorPointer<T> {auto tmp {*this}; return tmp -= offset;}

			constexpr auto operator-(const HeapAllocatorPointer<T> &pointer) const noexcept -> difference_type {
				return ((*m_pTableEntry + m_offset) - (*pointer.m_pTableEntry + pointer.m_offset)) / sizeof(T);
			}

			inline auto operator*() const noexcept -> element_type& {return *reinterpret_cast<element_type*> (*m_pTableEntry + m_offset);}
			inline auto operator->() const noexcept -> element_type* {return reinterpret_cast<element_type*> (*m_pTableEntry + m_offset);}

			inline auto operator[](difference_type offset) const noexcept -> element_type& {return *(reinterpret_cast<element_type*> (*m_pTableEntry + m_offset) + offset);}

			constexpr operator rebind<const T> () const noexcept {return rebind<const T> (m_pTableEntry, m_offset);}
			template <typename U>
			requires (!std::same_as<U, T> && !std::same_as<U, const T>)
			explicit constexpr operator rebind<U> () const noexcept {return rebind<U> (m_pTableEntry, m_offset);}


		private:
			std::byte **m_pTableEntry;
			ptrdiff_t m_offset;
	};

	template <typename T>
	constexpr auto operator+(typename HeapAllocatorPointer<T>::difference_type offset, const HeapAllocatorPointer<T> &pointer) noexcept {
		return pointer + offset;
	}

	static_assert(std::contiguous_iterator<HeapAllocatorPointer<char>>);


	class SL_CORE HeapAllocator final {
		public:
			using value_type = std::byte;
			using pointer = sl::memory::HeapAllocatorPointer<value_type>;
			using const_pointer = sl::memory::HeapAllocatorPointer<const value_type>;
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
			using pointer = sl::memory::HeapAllocatorPointer<value_type>;
			using const_pointer = sl::memory::HeapAllocatorPointer<const value_type>;
			using reference = value_type&;
			using const_reference = const value_type&;
			using size_type = HeapAllocator::size_type;
			using difference_type = HeapAllocator::difference_type;
			using is_always_equal = std::false_type;

			inline HeapAllocatorView(HeapAllocator &heapAllocator) noexcept : m_heapAllocator {&heapAllocator} {}
			inline HeapAllocatorView(const HeapAllocatorView<T> &) noexcept = default;
			inline auto operator=(const HeapAllocatorView<T> &) noexcept -> HeapAllocatorView<T>& = default;
			inline HeapAllocatorView(HeapAllocatorView<T> &&) noexcept = default;
			inline auto operator=(HeapAllocatorView<T> &&) noexcept -> HeapAllocatorView<T>& = default;

			[[nodiscard]]
			inline auto allocate(size_type n) const noexcept -> pointer {return static_cast<pointer> (m_heapAllocator->allocate(sizeof(T) * n, alignof(T)));}
			inline auto deallocate(const pointer &ptr, size_type) const noexcept -> void {m_heapAllocator->deallocate(static_cast<HeapAllocator::pointer> (ptr));}

			inline auto operator==(const HeapAllocatorView<T> &view) const noexcept -> bool {return m_heapAllocator == view.m_heapAllocator;}

		private:
			HeapAllocator *m_heapAllocator;
	};

	static_assert(sl::memory::IsAllocator<HeapAllocatorView<char>>);

} // namespace sl::memory
