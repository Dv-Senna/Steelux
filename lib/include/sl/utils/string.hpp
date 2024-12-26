#pragma once

#include <cstddef>
#include <iterator>
#include <ranges>


namespace sl::utils {
	template <typename T>
	class StringIterator final {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			inline StringIterator(pointer ptr = nullptr) noexcept : m_ptr {ptr} {}
			inline ~StringIterator() = default;

			inline StringIterator(const StringIterator<T> &iterator) noexcept : m_ptr {iterator.m_ptr} {}
			inline StringIterator<T> &operator=(const StringIterator<T> &iterator) noexcept {iterator.m_ptr = m_ptr; return *this;}

			inline StringIterator<T> &operator=(pointer ptr) noexcept {m_ptr = ptr; return *this;}

			inline operator bool() const noexcept {return m_ptr;}

			inline bool operator==(const StringIterator<T> &iterator) const noexcept {return m_ptr == iterator.m_ptr;}
			inline auto operator<=>(const StringIterator<T> &iterator) const noexcept {return m_ptr <=> iterator.m_ptr;}

			inline StringIterator<T> &operator+=(difference_type diff) noexcept {m_ptr += diff; return *this;}
			inline StringIterator<T> &operator-=(difference_type diff) noexcept {m_ptr -= diff; return *this;}

			inline StringIterator<T> &operator++() noexcept {++m_ptr; return *this;}
			inline StringIterator<T> operator++(int) noexcept {auto tmp {*this}; ++m_ptr; return tmp;}
			inline StringIterator<T> &operator--() noexcept {--m_ptr; return *this;}
			inline StringIterator<T> operator--(int) noexcept {auto tmp {*this}; --m_ptr; return tmp;}

			inline StringIterator<T> operator+(difference_type diff) const noexcept {auto tmp {*this}; return tmp += diff;}
			inline StringIterator<T> operator-(difference_type diff) const noexcept {auto tmp {*this}; return tmp -= diff;}

			inline difference_type operator-(const StringIterator<T> &iterator) const noexcept {return m_ptr - iterator.m_ptr;}

			reference operator*() const noexcept {return *m_ptr;}
			pointer operator->() const noexcept {return m_ptr;}

			reference operator[](difference_type diff) const noexcept {return *(m_ptr + diff);}

			inline pointer getPtr() const noexcept {return m_ptr;}


		protected:
			pointer m_ptr;
	};


	template <typename T>
	class ReverseStringIterator final {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			inline ReverseStringIterator(pointer ptr = nullptr) noexcept : m_ptr {ptr} {}
			inline ~ReverseStringIterator() = default;

			inline ReverseStringIterator(const ReverseStringIterator<T> &iterator) noexcept : m_ptr {iterator.m_ptr} {}
			inline ReverseStringIterator<T> &operator=(const ReverseStringIterator<T> &iterator) noexcept {iterator.m_ptr = m_ptr; return *this;}

			inline ReverseStringIterator<T> &operator=(pointer ptr) noexcept {m_ptr = ptr; return *this;}

			inline operator bool() const noexcept {return m_ptr;}

			inline bool operator==(const ReverseStringIterator<T> &iterator) const noexcept {return m_ptr == iterator.m_ptr;}
			inline auto operator<=>(const ReverseStringIterator<T> &iterator) const noexcept {return iterator.m_ptr <=> m_ptr;}

			inline ReverseStringIterator<T> &operator+=(difference_type diff) noexcept {m_ptr -= diff; return *this;}
			inline ReverseStringIterator<T> &operator-=(difference_type diff) noexcept {m_ptr += diff; return *this;}

			inline ReverseStringIterator<T> &operator++() noexcept {--m_ptr; return *this;}
			inline ReverseStringIterator<T> operator++(int) noexcept {auto tmp {*this}; --m_ptr; return tmp;}
			inline ReverseStringIterator<T> &operator--() noexcept {++m_ptr; return *this;}
			inline ReverseStringIterator<T> operator--(int) noexcept {auto tmp {*this}; ++m_ptr; return tmp;}

			inline ReverseStringIterator<T> operator+(difference_type diff) const noexcept {auto tmp {*this}; return tmp -= diff;}
			inline ReverseStringIterator<T> operator-(difference_type diff) const noexcept {auto tmp {*this}; return tmp += diff;}

			inline difference_type operator-(const ReverseStringIterator<T> &iterator) const noexcept {return iterator.m_ptr - m_ptr;}

			reference operator*() const noexcept {return *m_ptr;}
			pointer operator->() const noexcept {return m_ptr;}

			reference operator[](difference_type diff) const noexcept {return *(m_ptr - diff);}

			inline pointer getPtr() const noexcept {return m_ptr;}


		protected:
			pointer m_ptr;
	};


	template <typename T>
	StringIterator<T> operator+(typename StringIterator<T>::difference_type diff, const StringIterator<T> &iterator) {
		return iterator + diff;
	}

	template <typename T>
	ReverseStringIterator<T> operator+(typename ReverseStringIterator<T>::difference_type diff, const ReverseStringIterator<T> &iterator) {
		return iterator + diff;
	}


	class String final {
		public:
			using iterator = StringIterator<char>;
			using const_iterator = StringIterator<const char>;
			using reverse_iterator = ReverseStringIterator<char>;
			using const_reverse_iterator = ReverseStringIterator<const char>;

			String() noexcept;
			String(const char *str) noexcept;
			String(const char *str, std::size_t length) noexcept;

			~String();

			String(const String &str) noexcept;
			const String &operator=(const String &str) noexcept;

			String(String &&str) noexcept;
			const String &operator=(String &&str) noexcept;

			inline char *getData() noexcept;
			inline const char *getData() const noexcept;
			inline std::size_t getSize() const noexcept {return m_size;}
			inline std::size_t getCapacity() const noexcept;

			inline bool isEmpty() const noexcept {return m_size == 0;}

			inline char &operator[](std::size_t index) noexcept;
			inline const char &operator[](std::size_t index) const noexcept;

			inline iterator begin() noexcept;
			inline iterator end() noexcept;
			inline const_iterator cbegin() const noexcept;
			inline const_iterator cend() const noexcept;
			inline const_iterator begin() const noexcept {return this->cbegin();}
			inline const_iterator end() const noexcept {return this->cend();}
			inline reverse_iterator rbegin() noexcept;
			inline reverse_iterator rend() noexcept;
			inline const_reverse_iterator crbegin() const noexcept;
			inline const_reverse_iterator crend() const noexcept;
			inline const_reverse_iterator rbegin() const noexcept {return this->crbegin();}
			inline const_reverse_iterator rend() const noexcept {return this->crend();}


		private:
			inline bool m_isSSO() const noexcept;

			// not counting null-terminating character
			std::size_t m_size;

			union {
				struct {
					char *start;
					std::size_t capacity;
				} m_heap;

				struct {
					char buffer[sizeof(m_heap) / sizeof(char)];
				} m_sso;
			};

			static constexpr std::size_t MAX_SSO_SIZE {sizeof(m_heap) / sizeof(char) - 1};
	};


	static_assert(std::ranges::random_access_range<String>, "String type must fullfill std::ranges::random_access_range concept");


	namespace literals {
		constexpr sl::utils::String operator ""_s(const char *str, std::size_t length);
	} // namespace literals

} // namespace sl::utils

#include "sl/utils/string.inl"
