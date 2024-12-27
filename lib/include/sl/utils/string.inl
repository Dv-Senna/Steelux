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


	std::ptrdiff_t String::getSize() const noexcept {
		if (this->m_isSSO())
			return -m_size;
		return m_size;
	}


	std::ptrdiff_t String::getCapacity() const noexcept {
		if (this->m_isSSO())
			return MAX_SSO_SIZE + 1;
		return m_heap.capacity;
	}


	bool String::m_isSSO() const noexcept {
		return m_size <= 0;
	}


	char &String::operator[](std::ptrdiff_t index) noexcept {
		char *buffer {m_heap.start};
		if (this->m_isSSO())
			buffer = m_sso.buffer;
		if (index >= 0)
			return buffer[index];
		return buffer[this->getSize() + index];
	}


	const char &String::operator[](std::ptrdiff_t index) const noexcept {
		const char *buffer {m_heap.start};
		if (this->m_isSSO())
			buffer = m_sso.buffer;
		if (index >= 0)
			return buffer[index];
		return buffer[this->getSize() + index];
	}


	String::iterator String::begin() noexcept {
		if (this->m_isSSO())
			return iterator(this, m_sso.buffer);
		return iterator(this, m_heap.start);
	}


	String::iterator String::end() noexcept {
		if (this->m_isSSO())
			return iterator(this, m_sso.buffer + this->getSize());
		return iterator(this, m_heap.start + this->getSize());
	}


	String::const_iterator String::cbegin() const noexcept {
		if (this->m_isSSO())
			return const_iterator(this, m_sso.buffer);
		return const_iterator(this, m_heap.start);
	}


	String::const_iterator String::cend() const noexcept {
		if (this->m_isSSO())
			return const_iterator(this, m_sso.buffer + this->getSize());
		return const_iterator(this, m_heap.start + this->getSize());
	}


	String::reverse_iterator String::rbegin() noexcept {
		if (this->m_isSSO())
			return reverse_iterator(this, m_sso.buffer + this->getSize() - 1);
		return reverse_iterator(this, m_heap.start + this->getSize() - 1);
	}


	String::reverse_iterator String::rend() noexcept {
		if (this->m_isSSO())
			return reverse_iterator(this, m_sso.buffer - 1);
		return reverse_iterator(this, m_heap.start - 1);
	}


	String::const_reverse_iterator String::crbegin() const noexcept {
		if (this->m_isSSO())
			return const_reverse_iterator(this, m_sso.buffer + this->getSize() - 1);
		return const_reverse_iterator(this, m_heap.start + this->getSize() - 1);
	}


	String::const_reverse_iterator String::crend() const noexcept {
		if (this->m_isSSO())
			return const_reverse_iterator(this, m_sso.buffer - 1);
		return const_reverse_iterator(this, m_heap.start - 1);
	}


	namespace literals {
		constexpr sl::utils::String operator ""_s(const char *str, std::size_t length) {
			return sl::utils::String(str, length);
		}

	} // namespace literals

} // namespace sl::utils
