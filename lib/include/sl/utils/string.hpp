#pragma once

#include "sl/utils/enums.hpp"


namespace sl::utils {
	enum class StringEncoding : sl::utils::PackedEnumString {
		eUTF8 = "UTF8"_pes,
		eUTF16 = "UTF16"_pes
	};

	template <StringEncoding ENCODING> struct Char final {};
	template <> struct Char<StringEncoding::eUTF8> final {using Type = char;};
	template <> struct Char<StringEncoding::eUTF16> final {using Type = char16_t;};

	template <StringEncoding ENCODING> using Char_t = typename Char<ENCODING>::Type;

#ifndef SL_USE_UTF16_STRING
	constexpr StringEncoding DEFAULT_STRING_ENCODING {StringEncoding::eUTF8};
#else
	constexpr StringEncoding DEFAULT_STRING_ENCODING {sl::utils::StringEncoding::eeUTF16};
#endif

	template <StringEncoding ENCODING>
	class StringIterator final {
		public:
			using Char = sl::utils::Char_t<ENCODING>;

			StringIterator();
			~StringIterator();
	};

	template <StringEncoding E>
	class _String final {
		public:
			static constexpr StringEncoding ENCODING = E;
			using Char = sl::utils::Char_t<E>;

			inline _String() noexcept;
			inline _String(const Char *str) noexcept;
			inline _String(const Char *str, std::size_t length) noexcept;

			inline ~_String();

			template <StringEncoding E2>
			_String(const _String<E2> &str) noexcept;
			template <StringEncoding E2>
			const _String<E> &operator=(const _String<E2> &str) noexcept;

			inline _String(_String<E> &&str) noexcept;
			inline const _String<E> &operator=(_String<E> &&str) noexcept;

			inline Char *getData() noexcept;
			inline const Char *getData() const noexcept;
			inline std::size_t getSize() const noexcept {return m_size;}
			inline std::size_t getCapacity() const noexcept;


		private:
			inline bool m_isSSO() const noexcept;

			// not counting null-terminating character
			std::size_t m_size;

			union {
				struct {
					Char *start;
					std::size_t capacity;
				} m_heap;

				struct {
					Char buffer[sizeof(m_heap) / sizeof(Char)];
				} m_sso;
			};

			static constexpr std::size_t MAX_SSO_SIZE {sizeof(m_heap) / sizeof(Char) - 1};
	};

	using UTF8String  = _String<StringEncoding::eUTF8>;
	using UTF16String = _String<StringEncoding::eUTF16>;
	using String = _String<DEFAULT_STRING_ENCODING>;

	namespace literals {
		constexpr sl::utils::String operator ""_s(const char *str, std::size_t length);
		constexpr sl::utils::UTF8String operator ""_s8(const char *str, std::size_t length);
		constexpr sl::utils::UTF16String operator ""_s16(const char *str, std::size_t length);
	} // namespace literals

} // namespace sl::utils

#include "sl/utils/string.inl"
