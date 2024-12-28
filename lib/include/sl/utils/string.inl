#pragma once

#include "sl/utils/string.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "sl/utils/memory.hpp"


namespace sl::utils {
	template <typename Alloc>
	constexpr String::String(Alloc *allocator) noexcept :
		m_allocator {},
		m_size {0},
		m_sso {}
	{
		if (!std::is_constant_evaluated())
			m_allocator = allocator == nullptr ? std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> ()
				: std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> (*allocator);
		sl::utils::memset<char> (m_sso.buffer, '\0', MAX_SSO_SIZE);
	}


	template <typename Alloc>
	constexpr String::String(const char *str, Alloc *allocator) noexcept :
		m_allocator {},
		m_size {0},
		m_sso {}
	{
		if (!std::is_constant_evaluated())
			m_allocator = allocator == nullptr ? std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> ()
				: std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> (*allocator);
		for (const char *current {str}; *current != (char)'\0'; ++current) ++m_size;
		if (m_size <= MAX_SSO_SIZE) {
			sl::utils::memcpy<char> (m_sso.buffer, str, m_size + 1);
			m_size *= -1;
			return;
		}

		m_heap.capacity = m_size + 1;
		if (!std::is_constant_evaluated())
			m_heap.start = reinterpret_cast<char*> (m_allocator->allocate(m_heap.capacity));
		else
			m_heap.start = new char[m_heap.capacity];
		sl::utils::memcpy<char> (m_heap.start, str, m_heap.capacity);
	}


	template <typename Alloc>
	constexpr String::String(const char *str, std::ptrdiff_t length, Alloc *allocator) noexcept :
		m_allocator {},
		m_size {length},
		m_sso {}
	{
		if (!std::is_constant_evaluated())
			m_allocator = allocator == nullptr ? std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> ()
				: std::make_unique<sl::memory::AllocatorViewFactory<Alloc>> (*allocator);
		if (m_size <= MAX_SSO_SIZE) {
			m_size *= -1;
			sl::utils::memcpy<char> (m_sso.buffer, str, length);
			m_sso.buffer[length] = (char)'\0';
			return;
		}

		m_heap.capacity = length + 1;
		if (!std::is_constant_evaluated())
			m_heap.start = reinterpret_cast<char*> (m_allocator->allocate(m_heap.capacity));
		else
			m_heap.start = new char[m_heap.capacity];
		sl::utils::memcpy<char> (m_heap.start, str, length);
		m_heap.start[length] = (char)'\0';
	}


	constexpr String::~String() {
		if (!this->m_isSSO()) {
			if (!std::is_constant_evaluated())
				m_allocator->deallocate(reinterpret_cast<std::byte*> (m_heap.start), m_heap.capacity);
			else
				delete[] m_heap.start;
		}
		m_size = 0;
	}


	constexpr std::ptrdiff_t String::reserve(std::ptrdiff_t newSize) noexcept {
		const std::ptrdiff_t capacity {this->getCapacity()};
		++newSize;
		if (newSize <= capacity)
			return capacity - 1;

		if (this->m_isSSO()) {
			char *buffer {nullptr};
			if (!std::is_constant_evaluated())
				buffer = reinterpret_cast<char*> (m_allocator->allocate(newSize));
			else
				buffer = new char[newSize];
			sl::utils::memcpy<char> (buffer, m_sso.buffer, newSize);
			m_heap.capacity = newSize;
			m_heap.start = buffer;
			m_size *= -1;
			return newSize - 1;
		}

		char *tmp {nullptr};
		if (!std::is_constant_evaluated())
			tmp = reinterpret_cast<char*> (m_allocator->allocate(newSize));
		else
			tmp = new char[newSize];
		sl::utils::memcpy(tmp, m_heap.start, m_heap.capacity);
		if (!std::is_constant_evaluated())
			m_allocator->deallocate(reinterpret_cast<std::byte*> (m_heap.start), m_heap.capacity);
		else
			delete[] m_heap.start;
		m_heap.capacity = newSize;
		m_heap.start = tmp;
		return newSize - 1;
	}


	constexpr char *String::getData() noexcept {
		if (m_size == 0)
			return nullptr;
		if (this->m_isSSO())
			return m_sso.buffer;
		return m_heap.start;
	}


	constexpr const char *String::getData() const noexcept {
		if (m_size == 0)
			return nullptr;
		if (this->m_isSSO())
			return m_sso.buffer;
		return m_heap.start;
	}


	constexpr std::ptrdiff_t String::getSize() const noexcept {
		if (this->m_isSSO())
			return -m_size;
		return m_size;
	}


	constexpr std::ptrdiff_t String::getCapacity() const noexcept {
		if (this->m_isSSO())
			return MAX_SSO_SIZE + 1;
		return m_heap.capacity;
	}


	constexpr bool String::m_isSSO() const noexcept {
		return m_size <= 0;
	}


	constexpr char &String::operator[](std::ptrdiff_t index) noexcept {
		char *buffer {m_heap.start};
		if (this->m_isSSO())
			buffer = m_sso.buffer;
		if (index >= 0)
			return buffer[index];
		return buffer[this->getSize() + index];
	}


	constexpr const char &String::operator[](std::ptrdiff_t index) const noexcept {
		const char *buffer {m_heap.start};
		if (this->m_isSSO())
			buffer = m_sso.buffer;
		if (index >= 0)
			return buffer[index];
		return buffer[this->getSize() + index];
	}


	constexpr String::iterator String::at(std::ptrdiff_t index) noexcept {
		if (index >= this->getSize())
			return this->end();
		if (index < -this->getSize())
			return this->end();
		if (index >= 0)
			return this->begin() + index;
		return this->end() + index;
	}


	constexpr String::const_iterator String::at(std::ptrdiff_t index) const noexcept {
		if (index >= this->getSize())
			return this->cend();
		if (index < -this->getSize())
			return this->cend();
		if (index >= 0)
			return this->cbegin() + index;
		return this->cend() + index;
	}


	constexpr String::iterator String::insert(std::ptrdiff_t index, char value) noexcept {
		constexpr float growFactor {1.5f};

		const std::ptrdiff_t capacity {this->getCapacity()};
		if (this->getSize() + 2 > capacity) {
			const std::ptrdiff_t newCapacity {static_cast<std::ptrdiff_t> (capacity * growFactor)};
			this->reserve(newCapacity);
		}

		auto it {this->begin()};
		it += index;
		sl::utils::memmove<char> (it.getPtr() + 1, it.getPtr(), this->getSize() - index + 1);
		*it = value;
		if (this->m_isSSO())
			--m_size;
		else
			++m_size;
		*this->end().getPtr() = '\0';
		return it;
	}


	constexpr void String::erase(std::ptrdiff_t index) noexcept {
		auto it {this->begin()};
		it += index;
		sl::utils::memmove(it.getPtr(), it.getPtr() + 1, this->getSize() - index);
		if (this->m_isSSO())
			++m_size;
		else
			--m_size;
	}


	constexpr void String::erase(std::ptrdiff_t start, std::ptrdiff_t end) noexcept {
		if (end <= start)
			return;
		auto it {this->begin()};
		it += start;
		sl::utils::memmove(it.getPtr(), it.getPtr() + end - 1, this->getSize() - end + 1);
		if (this->m_isSSO())
			m_size += end - start;
		else
			m_size -= end - start;
	}


	constexpr String::iterator String::begin() noexcept {
		if (this->m_isSSO())
			return iterator(this, m_sso.buffer);
		return iterator(this, m_heap.start);
	}


	constexpr String::iterator String::end() noexcept {
		if (this->m_isSSO())
			return iterator(this, m_sso.buffer + this->getSize());
		return iterator(this, m_heap.start + this->getSize());
	}


	constexpr String::const_iterator String::cbegin() const noexcept {
		if (this->m_isSSO())
			return const_iterator(this, m_sso.buffer);
		return const_iterator(this, m_heap.start);
	}


	constexpr String::const_iterator String::cend() const noexcept {
		if (this->m_isSSO())
			return const_iterator(this, m_sso.buffer + this->getSize());
		return const_iterator(this, m_heap.start + this->getSize());
	}


	constexpr String::reverse_iterator String::rbegin() noexcept {
		if (this->m_isSSO())
			return reverse_iterator(this, m_sso.buffer + this->getSize() - 1);
		return reverse_iterator(this, m_heap.start + this->getSize() - 1);
	}


	constexpr String::reverse_iterator String::rend() noexcept {
		if (this->m_isSSO())
			return reverse_iterator(this, m_sso.buffer - 1);
		return reverse_iterator(this, m_heap.start - 1);
	}


	constexpr String::const_reverse_iterator String::crbegin() const noexcept {
		if (this->m_isSSO())
			return const_reverse_iterator(this, m_sso.buffer + this->getSize() - 1);
		return const_reverse_iterator(this, m_heap.start + this->getSize() - 1);
	}


	constexpr String::const_reverse_iterator String::crend() const noexcept {
		if (this->m_isSSO())
			return const_reverse_iterator(this, m_sso.buffer - 1);
		return const_reverse_iterator(this, m_heap.start - 1);
	}


	template <std::integral T>
	constexpr std::optional<T> stringToNumber(const sl::utils::String &string) {
		constexpr const char allowedDigits[17][17] {"", "",
			"01", "", "", "", "", "",
			"01234567", "",
			"0123456789", "", "", "", "", "",
			"0123456789abcdef"
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

		const char *baseDigits {allowedDigits[static_cast<std::size_t> (base)]};

		for (; it != string.end(); ++it) {
			T value {std::numeric_limits<T>::max()};
			for (auto digit {baseDigits}; *digit != '\0'; ++digit) {
				if (*it != *digit)
					continue;
				value = static_cast<T> (digit - baseDigits);
			}
			if (value == std::numeric_limits<T>::max())
				return std::nullopt;
			const T position {static_cast<T> (string.end() - it - 1)};
			value *= std::pow(base, position);
			result += value;
		}

		return isPositive ? result : -result;
	}


	template <std::floating_point T>
	constexpr std::optional<T> stringToNumber(const sl::utils::String &string) {
		constexpr const char *digits {"0123456789"};

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

			T value {std::numeric_limits<T>::max()};
			for (auto digit {digits}; *digit != '\0'; ++digit) {
				if (*it != *digit)
					continue;
				value = static_cast<T> (digit - digits);
			}
			if (value == std::numeric_limits<T>::max())
				return std::nullopt;

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
