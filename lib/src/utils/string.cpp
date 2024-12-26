#include "sl/utils/string.hpp"


namespace sl::utils {
	String::String() noexcept :
		m_size {0}
	{
		
	}


	String::String(const char *str) noexcept :
		m_size {0}
	{
		for (const char *current {str}; *current != (char)'\0'; ++current) ++m_size;
		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str, m_size + 1);
			return;
		}

		m_heap.capacity = m_size + 1;
		m_heap.start = new char[m_heap.capacity];
		std::memcpy(m_heap.start, str, m_heap.capacity);
	}


	String::String(const char *str, std::size_t length) noexcept :
		m_size {length}
	{
		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str, length);
			m_sso.buffer[length] = (char)'\0';
			return;
		}

		m_heap.capacity = length + 1;
		m_heap.start = new char[m_heap.capacity];
		std::memcpy(m_heap.start, str, length);
		m_heap.start[length] = (char)'\0';
	}


	String::~String() {
		if (!this->m_isSSO())
			delete m_heap.start;
		m_size = 0;
	}


	String::String(const String &str) noexcept :
		m_size {str.m_size}
	{
		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str.m_sso.buffer, str.m_size + 1);
			return;
		}

		m_heap.capacity = str.m_size + 1;
		m_heap.start = new char[m_heap.capacity];
	}


	const String &String::operator=(const String &str) noexcept {
		this->~String();

		if (this->m_isSSO()) {
			std::memcpy(m_sso.buffer, str.m_sso.buffer, str.m_size + 1);
			return *this;
		}

		m_heap.capacity = str.m_size + 1;
		m_heap.start = new char[m_heap.capacity];
		return *this;
	}


	String::String(String &&str) noexcept :
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


	const String &String::operator=(String &&str) noexcept {
		this->~String();

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
} // namespace sl::utils
