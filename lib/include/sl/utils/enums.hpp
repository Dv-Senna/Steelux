#pragma once

#include <cstdint>
#include <format>
#include <ostream>
#include <type_traits>

#include "sl/core.hpp"
#include "sl/utils/string.hpp"


namespace sl::utils {
	// Pack a string composed only of characters a-z, A-Z, 0-9 and _ into
	// a 64-bits integer. Used to provide string value to enums. Can contain
	// up to 10 characters
	using PackedEnumString = std::int64_t;
	// Enum that can be represented as multi-values using a bitfield
	using FlagEnumBit = std::uint64_t;

	template <typename T>
	concept StringEnum = std::is_enum_v<T>
		&& std::is_same_v<std::underlying_type_t<T>, sl::utils::PackedEnumString>;

	template <typename T>
	concept FlagEnum = std::is_enum_v<T>
		&& std::is_same_v<std::underlying_type_t<T>, sl::utils::FlagEnumBit>;


	template <sl::utils::FlagEnum Enum>
	class FlagField {
		public:
			inline FlagField() noexcept : m_value {0} {}
			inline FlagField(Enum flag) noexcept : m_value {static_cast<sl::utils::FlagEnumBit> (flag)} {}
			inline ~FlagField() = default;

			inline FlagField(const FlagField<Enum> &field) noexcept : m_value {field.m_value} {}
			inline const FlagField &operator=(const FlagField<Enum> &field) noexcept {m_value = field.m_value; return *this;}

			inline FlagField(FlagField<Enum> &&field) noexcept : m_value {field.m_value} {field.m_value = 0;}
			inline const FlagField &operator=(FlagField<Enum> &&field) noexcept {m_value = field.m_value; field.m_value = 0; return *this;}

			inline const FlagField &operator&=(const FlagField<Enum> &field) noexcept {m_value &= field.m_value; return *this;}
			inline const FlagField &operator&=(Enum flag) noexcept {m_value &= static_cast<sl::utils::FlagEnumBit> (flag); return *this;}
			inline const FlagField &operator|=(const FlagField<Enum> &field) noexcept {m_value |= field.m_value; return *this;}
			inline const FlagField &operator|=(Enum flag) noexcept {m_value |= static_cast<sl::utils::FlagEnumBit> (flag); return *this;}
			inline const FlagField &operator^=(const FlagField<Enum> &field) noexcept {m_value ^= field.m_value; return *this;}
			inline const FlagField &operator^=(Enum flag) noexcept {m_value ^= static_cast<sl::utils::FlagEnumBit> (flag); return *this;}

			inline FlagField operator~() const noexcept {return FlagField<Enum> (~m_value);}
			inline FlagField operator&(FlagField<Enum> field) const noexcept {return field &= *this;}
			inline FlagField operator&(Enum flag) const noexcept {auto tmp {*this}; return tmp &= flag;}
			inline FlagField operator|(FlagField<Enum> field) const noexcept {return field |= *this;}
			inline FlagField operator|(Enum flag) const noexcept {auto tmp {*this}; return tmp |= flag;}
			inline FlagField operator^(FlagField<Enum> field) const noexcept {return field ^= *this;}
			inline FlagField operator^(Enum flag) const noexcept {auto tmp {*this}; return tmp ^= flag;}

			inline bool operator==(const FlagField<Enum> &field) const noexcept {return m_value == field.m_value;}
			inline bool operator==(Enum flag) const noexcept {return m_value == static_cast<sl::utils::FlagEnumBit> (flag);}

			inline operator bool() const noexcept {return !!m_value;}
			inline explicit operator sl::utils::FlagEnumBit() const noexcept {return m_value;}

		private:
			inline FlagField(sl::utils::FlagEnumBit value) : m_value {value} {}

			sl::utils::FlagEnumBit m_value;
	};

	template <sl::utils::FlagEnum Enum>
	inline FlagField<Enum> operator&(Enum lhs, Enum rhs) noexcept {return FlagField<Enum> (lhs) & rhs;}
	template <sl::utils::FlagEnum Enum>
	inline FlagField<Enum> operator&(Enum lhs, FlagField<Enum> rhs) noexcept {return rhs &= lhs;}
	template <sl::utils::FlagEnum Enum>
	inline FlagField<Enum> operator|(Enum lhs, Enum rhs) noexcept {return FlagField<Enum> (lhs) | rhs;}
	template <sl::utils::FlagEnum Enum>
	inline FlagField<Enum> operator|(Enum lhs, FlagField<Enum> rhs) noexcept {return rhs |= lhs;}
	template <sl::utils::FlagEnum Enum>
	inline FlagField<Enum> operator^(Enum lhs, Enum rhs) noexcept {return FlagField<Enum> (lhs) ^ rhs;}
	template <sl::utils::FlagEnum Enum>
	inline FlagField<Enum> operator^(Enum lhs, FlagField<Enum> rhs) noexcept {return rhs ^= lhs;}
	template <sl::utils::FlagEnum Enum>
	inline FlagField<Enum> operator~(Enum flag) noexcept {return ~FlagField<Enum> (flag);}


	SL_CORE sl::String toString(PackedEnumString value) noexcept;
	template <sl::utils::StringEnum Enum>
	inline sl::String toString(Enum value) noexcept;

	template <sl::utils::FlagEnum Enum>
	sl::String toString(const FlagField<Enum> &field) noexcept;
	template <sl::utils::FlagEnum Enum>
	inline sl::String toString(Enum flag) noexcept;


	namespace literals {
		constexpr sl::utils::PackedEnumString operator ""_pes(const char *str, std::size_t length) noexcept;
		constexpr sl::utils::FlagEnumBit operator ""_feb(unsigned long long position) noexcept;
	} // namespace literals


	inline std::ostream &operator<<(std::ostream &stream, PackedEnumString value) noexcept {stream << toString(value); return stream;}
	template <sl::utils::StringEnum Enum>
	inline std::ostream &operator<<(std::ostream &stream, Enum value) noexcept {stream << toString(value); return stream;}

	template <sl::utils::FlagEnum Enum>
	inline std::ostream &operator<<(std::ostream &stream, const FlagField<Enum> &field) noexcept {stream << toString(field); return stream;}
	template <sl::utils::FlagEnum Enum>
	inline std::ostream &operator<<(std::ostream &stream, Enum flag) noexcept {stream << toString(flag); return stream;}

	using namespace sl::utils::literals;
} // namespace sl::utils

#include "sl/utils/enums.inl"


template <>
struct std::formatter<sl::utils::PackedEnumString> : std::formatter<sl::String> {
	inline auto format(sl::utils::PackedEnumString value, std::format_context &ctx) const noexcept {
		return std::formatter<sl::String>::format(sl::utils::toString(value), ctx);
	}
};

template <sl::utils::StringEnum Enum>
struct std::formatter<Enum> : std::formatter<sl::String> {
	inline auto format(Enum value, std::format_context &ctx) const noexcept {
		return std::formatter<sl::String>::format(sl::utils::toString(value), ctx);
	}
};



template <sl::utils::FlagEnum Enum>
struct std::formatter<sl::utils::FlagField<Enum>> {
	std::int32_t m_bitShown {-1};

	constexpr auto parse(std::format_parse_context &ctx) {
		auto it {ctx.begin()};
		if (it == ctx.end())
			return it;
		sl::String text {};
		text.reserve(ctx.begin() - ctx.end());
		for (; it != ctx.end() - 1; ++it) text.pushBack(*it);
		if (text.getSize() == 0)
			return it;

		auto bitShown {sl::utils::stringToNumber<std::int32_t> (text)};
		if (!bitShown)
			throw std::format_error("Invalid format args for sl::utils::FlagField<Enum>. The argument must be an integer");
		m_bitShown = *bitShown;
		return it;
	}

	inline auto format(const sl::utils::FlagField<Enum> &field, std::format_context &ctx) const noexcept {
		sl::String text {sl::utils::toString(field)};
		if (m_bitShown < 0)
			return std::ranges::copy(text, ctx.out()).out;

		if (text.getSize() > m_bitShown) {
			while (text.getSize() != m_bitShown)
				text.popFront();
		}

		else if (text.getSize() < m_bitShown) {
			sl::String missing {};
			missing.reserve(m_bitShown - text.getSize());
			for (auto i {text.getSize()}; i != m_bitShown; ++i)
				missing.pushBack('0');
			text = missing + text;
		}
		return std::ranges::copy(text, ctx.out()).out;
	}
};

template <sl::utils::FlagEnum Enum>
struct std::formatter<Enum> : std::formatter<sl::utils::FlagField<Enum>> {
	inline auto format(Enum flag, std::format_context &ctx) const noexcept {
		return std::formatter<sl::utils::FlagField<Enum>>::format(sl::utils::FlagField<Enum> (flag), ctx);
	}
};

