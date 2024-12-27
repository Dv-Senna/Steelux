#pragma once

#include <cstddef>
#include <ranges>

#include "sl/utils/iterator.hpp"


namespace sl::utils {
	class String final {
		public:
			using iterator = sl::utils::ContinousIterator<String, char>;
			using const_iterator = sl::utils::ContinousIterator<const String, const char>;
			using reverse_iterator = sl::utils::ReverseContinousIterator<String, char>;
			using const_reverse_iterator = sl::utils::ReverseContinousIterator<const String, const char>;

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
