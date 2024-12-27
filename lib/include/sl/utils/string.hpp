#pragma once

#include <cstddef>
#include <ranges>
#include <vector>

#include "sl/utils/iterator.hpp"


namespace sl::utils {
	/**
	 * @brief A class that handles strings in Steelux
	 */
	class String final {
		public:
			using iterator = sl::utils::ContinousIterator<String, char>;
			using const_iterator = sl::utils::ContinousIterator<const String, const char>;
			using reverse_iterator = sl::utils::ReverseContinousIterator<String, char>;
			using const_reverse_iterator = sl::utils::ReverseContinousIterator<const String, const char>;

			String() noexcept;
			String(const char *str) noexcept;
			String(const char *str, std::ptrdiff_t length) noexcept;

			~String();

			String(const String &str) noexcept;
			const String &operator=(const String &str) noexcept;

			String(String &&str) noexcept;
			const String &operator=(String &&str) noexcept;

			/**
			 * @brief Reserve a certain amount of space for the string
			 * @param newSize The amount of space to reserve, not including the null-terminating character
			 * @return The amount of space the string has available. May be bigger than `newSize`. Not including the
			 *         null-terminating character
			 */
			std::ptrdiff_t reserve(std::ptrdiff_t newSize) noexcept;

			inline char *getData() noexcept;
			inline const char *getData() const noexcept;
			inline std::ptrdiff_t getSize() const noexcept;
			inline std::ptrdiff_t getCapacity() const noexcept;

			inline bool isEmpty() const noexcept {return m_size == 0;}

			inline char &operator[](std::ptrdiff_t index) noexcept;
			inline const char &operator[](std::ptrdiff_t index) const noexcept;

			iterator at(std::ptrdiff_t index) noexcept;
			const_iterator at(std::ptrdiff_t index) const noexcept;

			iterator insert(std::ptrdiff_t index, char value) noexcept;
			inline iterator insert(iterator position, char value) noexcept {return this->insert(position - this->begin(), value);}
			inline iterator insert(reverse_iterator position, char value) noexcept {return this->insert(position - this->rbegin(), value);}
			inline iterator pushFront(char value) noexcept {return this->insert(0, value);}
			inline iterator pushBack(char value) noexcept {return this->insert(this->getSize(), value);}

			void erase(std::ptrdiff_t index) noexcept;
			inline void erase(iterator position) noexcept {this->erase(position - this->begin());}
			inline void erase(reverse_iterator position) noexcept {this->erase(position - this->rbegin());}
			void erase(std::ptrdiff_t start, std::ptrdiff_t end) noexcept;
			inline void erase(iterator start, iterator end) noexcept {this->erase(start - this->begin(), end - this->begin());}
			inline void erase(reverse_iterator start, reverse_iterator end) noexcept {this->erase(start - this->rbegin(), end - this->rbegin());}
			inline void popFront() noexcept {this->erase(0);}
			inline void popBack() noexcept {this->erase(m_size - 1);}

			String &operator+=(const String &string) noexcept;

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
			std::ptrdiff_t m_size;

			union {
				struct {
					char *start;
					std::ptrdiff_t capacity;
				} m_heap;

				struct {
					char buffer[sizeof(m_heap) / sizeof(char)];
				} m_sso;
			};

			static constexpr std::ptrdiff_t MAX_SSO_SIZE {sizeof(m_heap) / sizeof(char) - 1};
	};


	static_assert(std::ranges::random_access_range<String>, "String type must fullfill std::ranges::random_access_range concept");


	namespace literals {
		constexpr sl::utils::String operator ""_s(const char *str, std::size_t length);
	} // namespace literals

} // namespace sl::utils

#include "sl/utils/string.inl"
