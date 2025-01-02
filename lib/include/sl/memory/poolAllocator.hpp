#pragma once

#include <cstddef>
#include <map>

#include "sl/memory/allocatorTraits.hpp"


namespace sl::memory {
	template <typename T>
	class PoolAllocator {
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

			[[nodiscard]] pointer allocate() noexcept;
			void deallocate(pointer ptr) noexcept;


		private:
			size_type m_poolSize;
			value_type *m_pool;
			bool *m_poolState;
			size_type m_instanceID;

			static size_type s_lastInstanceID;
			static std::map<size_type, size_type> s_instanceCounts;
	};

} // namespace sl::memory

#include "sl/memory/poolAllocator.inl"
