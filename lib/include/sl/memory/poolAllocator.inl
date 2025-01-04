#pragma once

#include "sl/memory/poolAllocator.hpp"
#include "sl/utils/assert.hpp"


namespace sl::memory {
	template <typename T>
	auto PoolAllocatorIterator<T>::operator==(const PoolAllocatorIterator<T> &it) const noexcept -> bool {
		return m_allocator == it.m_allocator && m_position == it.m_position;
	}


	template <typename T>
	auto PoolAllocatorIterator<T>::operator++() noexcept -> PoolAllocatorIterator<T>& {
		while (
			static_cast<PoolAllocator<T>::size_type> (m_position) < m_allocator->m_poolSize &&
			!m_allocator->m_poolState[++m_position]
		);
		return *this;
	}


	template <typename T>
	auto PoolAllocatorIterator<T>::operator++(int) noexcept -> PoolAllocatorIterator<T> {
		auto tmp {*this};
		++(*this);
		return tmp;
	}


	template <typename T>
	auto PoolAllocatorIterator<T>::operator--() noexcept -> PoolAllocatorIterator<T>& {
		while (m_position > 0 && !m_allocator->m_poolState[--m_position]);
		return *this;
	}


	template <typename T>
	auto PoolAllocatorIterator<T>::operator--(int) noexcept -> PoolAllocatorIterator<T> {
		auto tmp {*this};
		--(*this);
		return tmp;
	}


	template <typename T>
	auto PoolAllocatorIterator<T>::operator*() const noexcept -> reference {
		return m_allocator->m_pool[m_position];
	}


	template <typename T>
	auto PoolAllocatorIterator<T>::operator->() const noexcept -> pointer {
		return m_allocator->m_pool + m_position;
	}


	template <typename T>
	PoolAllocatorIterator<T>::PoolAllocatorIterator(PoolAllocator<T> &allocator, difference_type position) noexcept :
		m_allocator {&allocator},
		m_position {position}
	{

	}




	template <typename T>
	PoolAllocator<T>::PoolAllocator(size_type size) noexcept :
		m_poolSize {size},
		m_pool {reinterpret_cast<T*> (std::malloc(sizeof(T) * m_poolSize))},
		m_poolState {reinterpret_cast<bool*> (std::malloc(sizeof(bool) * m_poolSize))},
		m_instanceID {++s_lastInstanceID}
	{
		s_instanceCounts[m_instanceID] = 1;
		for (size_type i {0}; i < m_poolSize; ++i)
			m_poolState[i] = false;
	}


	template <typename T>
	PoolAllocator<T>::~PoolAllocator() {
		if (m_instanceID == 0)
			return;
		if (--s_instanceCounts[m_instanceID] != 0)
			return;

		m_instanceID = 0;
		std::free(m_poolState);
		std::free(m_pool);
		m_poolSize = 0;
	}


	template <typename T>
	PoolAllocator<T>::PoolAllocator(const PoolAllocator<T> &allocator) noexcept :
		m_poolSize {allocator.m_poolSize},
		m_pool {allocator.m_pool},
		m_poolState {allocator.m_poolState},
		m_instanceID {allocator.m_instanceID}
	{
		++s_instanceCounts[m_instanceID];
	}


	template <typename T>
	PoolAllocator<T> &PoolAllocator<T>::operator=(const PoolAllocator<T> &allocator) noexcept {
		this->~PoolAllocator<T>();

		m_poolSize = allocator.m_poolSize;
		m_pool = allocator.m_pool;
		m_poolState = allocator.m_poolState;
		m_instanceID = allocator.m_instanceID;
		++s_instanceCounts[m_instanceID];

		return *this;
	}


	template <typename T>
	PoolAllocator<T>::PoolAllocator(PoolAllocator<T> &&allocator) noexcept :
		m_poolSize {allocator.m_poolSize},
		m_pool {allocator.m_pool},
		m_poolState {allocator.m_poolState},
		m_instanceID {allocator.m_instanceID}
	{
		allocator.m_poolSize = 0;
		allocator.m_pool = nullptr;
		allocator.m_poolState = nullptr;
		allocator.m_instanceID = 0;
	}


	template <typename T>
	PoolAllocator<T> &PoolAllocator<T>::operator=(PoolAllocator<T> &&allocator) noexcept {
		this->~PoolAllocator<T>();

		m_poolSize = allocator.m_poolSize;
		m_pool = allocator.m_pool;
		m_poolState = allocator.m_poolState;
		m_instanceID = allocator.m_instanceID;

		allocator.m_poolSize = 0;
		allocator.m_pool = nullptr;
		allocator.m_poolState = nullptr;
		allocator.m_instanceID = 0;

		return *this;
	}


	template <typename T>
	[[nodiscard]]
	auto PoolAllocator<T>::allocate(size_type n) noexcept -> pointer {
		SL_TEXT_ASSERT(n == 1, "n of PoolAllocator->allocate must be 1");

		for (size_type i {0}; i < m_poolSize; ++i) {
			if (m_poolState[i])
				continue;

			m_poolState[i] = true;
			return m_pool + i;
		}

		return nullptr;
	}


	template <typename T>
	auto PoolAllocator<T>::deallocate(pointer ptr, size_type n) noexcept -> void {
		SL_TEXT_ASSERT(n == 1, "n of PoolAllocator->deallocate must be 1");

		difference_type diff {ptr - m_pool};
		if (diff < 0)
			return;
		size_type index {static_cast<size_type> (diff)};
		if (index >= m_poolSize)
			return;

		m_poolState[index] = false;
	}


	template <typename T>
	typename PoolAllocator<T>::size_type PoolAllocator<T>::s_lastInstanceID {0};
	template <typename T>
	std::map<typename PoolAllocator<T>::size_type, typename PoolAllocator<T>::size_type> PoolAllocator<T>::s_instanceCounts {};

} // namespace sl::memory
