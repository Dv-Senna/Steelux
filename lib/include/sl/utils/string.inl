#pragma once

#include "sl/utils/string.hpp"

#include <cstring>


namespace sl::utils {
	char *String::getData() noexcept {
		if (m_size == 0)
			return nullptr;
		if (this->m_isSSO())
			return m_sso.buffer;
		return m_heap.start;
	}


	const char *String::getData() const noexcept {
		if (m_size == 0)
			return nullptr;
		if (this->m_isSSO())
			return m_sso.buffer;
		return m_heap.start;
	}


	std::size_t String::getCapacity() const noexcept {
		if (this->m_isSSO())
			return MAX_SSO_SIZE + 1;
		return m_heap.capacity;
	}


	bool String::m_isSSO() const noexcept {
		return m_size <= MAX_SSO_SIZE;
	}


	char &String::operator[](std::size_t index) noexcept {
		if (this->m_isSSO())
			return m_sso.buffer[index];
		return m_sso.buffer[index];
	}


	const char &String::operator[](std::size_t index) const noexcept {
		if (this->m_isSSO())
			return m_sso.buffer[index];
		return m_sso.buffer[index];
	}


	String::iterator String::begin() noexcept {
		if (this->m_isSSO())
			return iterator(m_sso.buffer);
		return iterator(m_heap.start);
	}


	String::iterator String::end() noexcept {
		if (this->m_isSSO())
			return iterator(m_sso.buffer + m_size);
		return iterator(m_heap.start + m_size);
	}


	String::const_iterator String::cbegin() const noexcept {
		if (this->m_isSSO())
			return const_iterator(m_sso.buffer);
		return const_iterator(m_heap.start);
	}


	String::const_iterator String::cend() const noexcept {
		if (this->m_isSSO())
			return const_iterator(m_sso.buffer + m_size);
		return const_iterator(m_heap.start + m_size);
	}


	String::reverse_iterator String::rbegin() noexcept {
		if (this->m_isSSO())
			return reverse_iterator(m_sso.buffer + m_size - 1);
		return reverse_iterator(m_heap.start + m_size - 1);
	}


	String::reverse_iterator String::rend() noexcept {
		if (this->m_isSSO())
			return reverse_iterator(m_sso.buffer - 1);
		return reverse_iterator(m_heap.start - 1);
	}


	String::const_reverse_iterator String::crbegin() const noexcept {
		if (this->m_isSSO())
			return const_reverse_iterator(m_sso.buffer + m_size - 1);
		return const_reverse_iterator(m_heap.start + m_size - 1);
	}


	String::const_reverse_iterator String::crend() const noexcept {
		if (this->m_isSSO())
			return const_reverse_iterator(m_sso.buffer - 1);
		return const_reverse_iterator(m_heap.start - 1);
	}


	namespace literals {
		constexpr sl::utils::String operator ""_s(const char *str, std::size_t length) {
			return sl::utils::String(str, length);
		}

	} // namespace literals

} // namespace sl::utils
