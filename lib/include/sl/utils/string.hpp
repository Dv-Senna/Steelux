#pragma once

#include <cstddef>


namespace sl::utils {
	class String final {
		public:
			using Type = char;

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

			inline char &operator[](std::size_t index) noexcept;
			inline const char &operator[](std::size_t index) const noexcept;


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

	namespace literals {
		constexpr sl::utils::String operator ""_s(const char *str, std::size_t length);
	} // namespace literals

} // namespace sl::utils

#include "sl/utils/string.inl"
