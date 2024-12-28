#pragma once

#include "sl/utils/string.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>


namespace sl::utils {
	template <typename Alloc>
	String::String(Alloc *allocator) noexcept :
		m_allocator {allocator == nullptr ? std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> ()
			: std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> (*allocator)},
		m_size {0}
	{
		std::memset(m_sso.buffer, 0, MAX_SSO_SIZE);
	}


	template <typename Alloc>
	String::String(const char *str, Alloc *allocator) noexcept :
		m_allocator {allocator == nullptr ? std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> ()
			: std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> (*allocator)},
		m_size {0}
	{
		for (const char *current {str}; *current != (char)'\0'; ++current) ++m_size;
		if (m_size <= MAX_SSO_SIZE) {
			std::memcpy(m_sso.buffer, str, m_size + 1);
			m_size *= -1;
			return;
		}

		m_heap.capacity = m_size + 1;
		m_heap.start = reinterpret_cast<char*> (m_allocator->allocate(m_heap.capacity));
		std::memcpy(m_heap.start, str, m_heap.capacity);
	}


	template <typename Alloc>
	String::String(const char *str, std::ptrdiff_t length, Alloc *allocator) noexcept :
		m_allocator {allocator == nullptr ? std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> ()
			: std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> (*allocator)},
		m_size {length}
	{
		if (m_size <= MAX_SSO_SIZE) {
			m_size *= -1;
			std::memcpy(m_sso.buffer, str, length);
			m_sso.buffer[length] = (char)'\0';
			return;
		}

		m_heap.capacity = length + 1;
		m_heap.start = reinterpret_cast<char*> (m_allocator->allocate(m_heap.capacity));
		std::memcpy(m_heap.start, str, length);
		m_heap.start[length] = (char)'\0';
	}


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


	template <std::integral T>
	std::optional<T> stringToNumber(const sl::utils::String &string) {
		static const std::map<T, sl::utils::String> allowedDigits {
			{2, "01"},
			{8, "01234567"},
			{10, "0123456789"},
			{16, "0123456789abcdef"}
		};

		T result {0};
		if (string.isEmpty())
			return std::nullopt;
		bool isPositive {true};

		auto it {string.begin()};
		if (*it == '-') {
			if constexpr (std::unsigned_integral<T>)
				return std::nullopt;
			else {
				if (string.getSize() == 1)
					return std::nullopt;
				isPositive = false;
				++it;
			}
		}

		else if (*it == '+')
			++it;

		T base {10};
		if (*it == '0') {
			++it;
			if (*it == 'b') {
				base = 2;
				++it;
			}
			else if (*it == 'x') {
				base = 16;
				++it;
			}
			else if (*it == 'o') {
				base = 8;
				++it;
			}
		}

		const sl::utils::String &baseDigits {allowedDigits.find(base)->second};

		for (; it != string.end(); ++it) {
			auto digit {std::ranges::find(baseDigits, *it)};
			if (digit == baseDigits.end())
				return std::nullopt;
			T value {static_cast<T> (digit - baseDigits.begin())};
			const T position {static_cast<T> (string.end() - it - 1)};
			value *= std::pow(base, position);
			result += value;
		}

		return isPositive ? result : -result;
	}


	template <std::floating_point T>
	std::optional<T> stringToNumber(const sl::utils::String &string) {
		static const sl::utils::String digits {"0123456789"};

		T result {0};
		T exponant {0};
		if (string.isEmpty())
			return std::nullopt;
		bool isPositive {true};
		auto it {string.begin()};
		if (*it == '-') {
			if (string.getSize() == 1)
				return std::nullopt;
			isPositive = false;
			++it;
		}

		if (std::ranges::count(string, '.') > 1)
			return std::nullopt;
		if (std::ranges::count(string, 'e') > 1)
			return std::nullopt;
		auto dotIt {std::ranges::find(string, '.')};
		auto sciIt {std::ranges::find(string, 'e')};
		if (dotIt != string.end() && dotIt > sciIt)
			return std::nullopt;

		bool exponentIsPositive {true};
		for (; it != string.end(); ++it) {
			if (it == dotIt || it == sciIt)
				continue;

			if (it == sciIt + 1) {
				if (*it == '-') {
					exponentIsPositive = false;
					continue;
				}
				if (*it == '+')
					continue;
			}

			auto digit {std::ranges::find(digits, *it)};
			if (digit == digits.end())
				return std::nullopt;
			T value {static_cast<T> (digit - digits.begin())};

			if (it > sciIt) {
				const T position {static_cast<T> (string.end() - it - 1)};
				value *= std::pow(static_cast<T> (10), position);
				exponant += value;
				continue;
			}

			T position {};
			if (it > dotIt)
				position = static_cast<T> (dotIt - it);
			else
				position = static_cast<T> (std::min(dotIt, sciIt) - it - 1);
			value *= std::pow(static_cast<T> (10), position);
			result += value;
		}

		result *= std::pow(static_cast<T> (10), exponentIsPositive ? exponant : -exponant);
		return isPositive ? result : -result;
	}


	namespace literals {
		constexpr sl::utils::String operator ""_s(const char *str, std::size_t length) {
			return sl::utils::String(str, length);
		}

	} // namespace literals

} // namespace sl::utils
