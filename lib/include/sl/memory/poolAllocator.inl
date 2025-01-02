#pragma once

#include "sl/memory/poolAllocator.hpp"


namespace sl::memory {
	template <typename T>
	PoolAllocator<T>::PoolAllocator(size_type size) noexcept :
		m_poolSize {size},
		m_pool {new T[m_poolSize]},
		m_poolState {new bool[m_poolSize]},
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
		delete[] m_poolState;
		delete[] m_pool;
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
	auto PoolAllocator<T>::allocate() noexcept -> pointer {
		for (size_type i {0}; i < m_poolSize; ++i) {
			if (m_poolState[i])
				continue;

			m_poolState[i] = true;
			new(m_pool + i) T();
			return m_pool + i;
		}

		return nullptr;
	}


	template <typename T>
	auto PoolAllocator<T>::deallocate(pointer ptr) noexcept -> void {
		difference_type diff {ptr - m_pool};
		if (diff < 0)
			return;
		size_type index {static_cast<size_type> (diff)};
		if (index >= m_poolSize)
			return;

		m_poolState[index] = false;
		m_pool[index].~T();
	}


	template <typename T>
	typename PoolAllocator<T>::size_type PoolAllocator<T>::s_lastInstanceID {0};
	template <typename T>
	std::map<typename PoolAllocator<T>::size_type, typename PoolAllocator<T>::size_type> PoolAllocator<T>::s_instanceCounts {};

} // namespace sl::memory
