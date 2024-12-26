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


	namespace literals {
		constexpr sl::utils::String operator ""_s(const char *str, std::size_t length) {
			return sl::utils::String(str, length);
		}

	} // namespace literals

} // namespace sl::utils
