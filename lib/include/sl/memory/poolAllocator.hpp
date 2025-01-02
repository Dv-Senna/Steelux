#pragma once

#include <cstddef>
#include <map>

#include "sl/memory/allocatorTraits.hpp"


namespace sl::memory {
	template <typename T>
	class PoolAllocator;

	template <typename T>
	class PoolAllocatorIterator {
		friend class PoolAllocator<T>;

		public:
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;
			using iterator_category = std::bidirectional_iterator_tag;

			inline PoolAllocatorIterator() noexcept = default;
			inline ~PoolAllocatorIterator() = default;
			
			inline PoolAllocatorIterator(const PoolAllocatorIterator<T> &it) noexcept = default;
			inline auto operator=(const PoolAllocatorIterator<T> &it) noexcept -> PoolAllocatorIterator<T>& = default;
			inline PoolAllocatorIterator(PoolAllocatorIterator<T> &&it) noexcept = default;
			inline auto operator=(PoolAllocatorIterator<T> &&it) noexcept -> PoolAllocatorIterator<T>& = default;

			inline auto operator==(const PoolAllocatorIterator<T> &it) const noexcept -> bool;

			inline auto operator++() noexcept -> PoolAllocatorIterator<T>&;
			inline auto operator++(int) noexcept -> PoolAllocatorIterator<T>;
			inline auto operator--() noexcept -> PoolAllocatorIterator<T>&;
			inline auto operator--(int) noexcept -> PoolAllocatorIterator<T>;

			inline auto operator*() const noexcept -> reference;
			inline auto operator->() const noexcept -> pointer;


		protected:
			inline PoolAllocatorIterator(PoolAllocator<T> &allocator, difference_type position) noexcept;

		private:
			PoolAllocator<T> *m_allocator;
			difference_type m_position;
	};


	static_assert(std::bidirectional_iterator<PoolAllocatorIterator<char>>);


	template <typename T>
	class PoolAllocator {
		friend class PoolAllocatorIterator<T>;

		public:
			using value_type = T;
			using pointer = T*;
			using const_pointer = const T*;
			using reference = T&;
			using const_reference = const T&;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using is_always_equal = std::false_type;

			PoolAllocator(size_type size) noexcept;
			~PoolAllocator();

			PoolAllocator(const PoolAllocator<T> &allocator) noexcept;
			PoolAllocator<T> &operator=(const PoolAllocator<T> &allocator) noexcept;
			PoolAllocator(PoolAllocator<T> &&allocator) noexcept;
			PoolAllocator<T> &operator=(PoolAllocator<T> &&allocator) noexcept;

			[[nodiscard]]
			auto allocate(size_type n = 1) noexcept -> pointer;
			auto deallocate(pointer ptr, size_type n = 1) noexcept -> void;

			inline auto operator==(const PoolAllocator<T> &allocator) const noexcept {return m_instanceID == allocator.m_instanceID;}

			inline auto begin() noexcept {return PoolAllocatorIterator<T> (*this, 0);}
			inline auto end() noexcept {return PoolAllocatorIterator<T> (*this, static_cast<difference_type> (m_poolSize));}


		protected:
			size_type m_poolSize;
			value_type *m_pool;
			bool *m_poolState;
			size_type m_instanceID;

			static size_type s_lastInstanceID;
			static std::map<size_type, size_type> s_instanceCounts;
	};

	static_assert(sl::memory::IsAllocator<PoolAllocator<char>>);

} // namespace sl::memory

#include "sl/memory/poolAllocator.inl"
