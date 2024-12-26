#pragma once

#include "sl/utils/string.hpp"

#include <cstring>


namespace sl::utils {
	template <StringEncoding E>
	_String<E>::_String() noexcept :
		m_size {0}
	{
		
	}


	template <StringEncoding E>
	_String<E>::_String(const Char *str) noexcept :
		m_size {0}
	{
		for (const Char *current {str}; *current != (Char)'\0'; ++current) ++m_size;
		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str, m_size + 1);
			return;
		}

		m_heap.capacity = m_size + 1;
		m_heap.start = new Char[m_heap.capacity];
		std::memcpy(m_heap.start, str, m_heap.capacity);
	}


	template <StringEncoding E>
	_String<E>::_String(const Char *str, std::size_t length) noexcept :
		m_size {length}
	{
		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str, length);
			m_sso.buffer[length] = (Char)'\0';
			return;
		}

		m_heap.capacity = length + 1;
		m_heap.start = new Char[m_heap.capacity];
		std::memcpy(m_heap.start, str, length);
		m_heap.start[length] = (Char)'\0';
	}


	template <StringEncoding E>
	_String<E>::~_String() {
		std::println("SSO {}", this->m_isSSO());

		if (!this->m_isSSO())
			delete m_heap.start;
		m_size = 0;
	}


	template <StringEncoding E>
	_String<E>::_String(_String<E> &&str) noexcept :
		m_size {str.m_size}
	{
		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str.m_sso.buffer, str.m_size + 1);
			std::memset(str.m_sso.buffer, 0, sizeof(m_heap));
			str.m_size = 0;
			return;
		}

		m_heap.capacity = str.m_heap.capacity;
		m_heap.start = str.m_heap.start;
		str.m_heap.capacity = 0;
		str.m_heap.start = nullptr;
		str.m_size = 0;
	}


	template <StringEncoding E>
	const _String<E> &_String<E>::operator=(_String<E> &&str) noexcept {
		this->~_String();

		m_size = str.m_size;
		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str.m_sso.buffer, str.m_size + 1);
			std::memset(str.m_sso.buffer, 0, sizeof(m_heap));
			str.m_size = 0;
			return *this;
		}

		m_heap.capacity = str.m_heap.capacity;
		m_heap.start = str.m_heap.start;
		str.m_heap.capacity = 0;
		str.m_heap.start = nullptr;
		str.m_size = 0;
		return *this;
	}


	template <StringEncoding E>
	_String<E>::Char *_String<E>::getData() noexcept {
		if (m_size == 0)
			return nullptr;
		if (this->m_isSSO())
			return m_sso.buffer;
		return m_heap.start;
	}


	template <StringEncoding E>
	const _String<E>::Char *_String<E>::getData() const noexcept {
		if (m_size == 0)
			return nullptr;
		if (this->m_isSSO())
			return m_sso.buffer;
		return m_heap.start;
	}


	template <StringEncoding E>
	std::size_t _String<E>::getCapacity() const noexcept {
		if (this->m_isSSO())
			return MAX_SSO_SIZE + 1;
		return m_heap.capacity;
	}


	template <StringEncoding E>
	bool _String<E>::m_isSSO() const noexcept {
		return m_size <= MAX_SSO_SIZE;
	}

} // namespace sl::utils
