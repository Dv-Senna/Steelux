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
			constexpr FlagField() noexcept : m_value {0} {}
			constexpr FlagField(Enum flag) noexcept : m_value {static_cast<sl::utils::FlagEnumBit> (flag)} {}
			constexpr ~FlagField() = default;

			constexpr FlagField(const FlagField<Enum> &field) noexcept : m_value {field.m_value} {}
			constexpr auto operator=(const FlagField<Enum> &field) noexcept -> FlagField& {m_value = field.m_value; return *this;}

			constexpr FlagField(FlagField<Enum> &&field) noexcept : m_value {field.m_value} {field.m_value = 0;}
			constexpr auto operator=(FlagField<Enum> &&field) noexcept -> FlagField& {m_value = field.m_value; field.m_value = 0; return *this;}

			constexpr auto operator&=(const FlagField<Enum> &field) noexcept -> FlagField& {m_value &= field.m_value; return *this;}
			constexpr auto operator&=(Enum flag) noexcept -> FlagField& {m_value &= static_cast<sl::utils::FlagEnumBit> (flag); return *this;}
			constexpr auto operator|=(const FlagField<Enum> &field) noexcept -> FlagField& {m_value |= field.m_value; return *this;}
			constexpr auto operator|=(Enum flag) noexcept -> FlagField& {m_value |= static_cast<sl::utils::FlagEnumBit> (flag); return *this;}
			constexpr auto operator^=(const FlagField<Enum> &field) noexcept -> FlagField& {m_value ^= field.m_value; return *this;}
			constexpr auto operator^=(Enum flag) noexcept -> FlagField& {m_value ^= static_cast<sl::utils::FlagEnumBit> (flag); return *this;}

			constexpr auto operator~() const noexcept -> FlagField {return FlagField<Enum> (~m_value);}
			constexpr auto operator&(FlagField<Enum> field) const noexcept -> FlagField {return field &= *this;}
			constexpr auto operator&(Enum flag) const noexcept -> FlagField {auto tmp {*this}; return tmp &= flag;}
			constexpr auto operator|(FlagField<Enum> field) const noexcept -> FlagField {return field |= *this;}
			constexpr auto operator|(Enum flag) const noexcept -> FlagField {auto tmp {*this}; return tmp |= flag;}
			constexpr auto operator^(FlagField<Enum> field) const noexcept -> FlagField {return field ^= *this;}
			constexpr auto operator^(Enum flag) const noexcept -> FlagField {auto tmp {*this}; return tmp ^= flag;}

			constexpr auto operator==(const FlagField<Enum> &field) const noexcept -> bool {return m_value == field.m_value;}
			constexpr auto operator==(Enum flag) const noexcept -> bool {return m_value == static_cast<sl::utils::FlagEnumBit> (flag);}

			constexpr operator bool() const noexcept {return !!m_value;}
			constexpr explicit operator sl::utils::FlagEnumBit() const noexcept {return m_value;}

		private:
			constexpr FlagField(sl::utils::FlagEnumBit value) : m_value {value} {}

			sl::utils::FlagEnumBit m_value;
	};

	template <sl::utils::FlagEnum Enum>
	constexpr auto operator&(Enum lhs, Enum rhs) noexcept -> FlagField<Enum> {return FlagField<Enum> (lhs) & rhs;}
	template <sl::utils::FlagEnum Enum>
	constexpr auto operator&(Enum lhs, FlagField<Enum> rhs) noexcept -> FlagField<Enum> {return rhs &= lhs;}
	template <sl::utils::FlagEnum Enum>
	constexpr auto operator|(Enum lhs, Enum rhs) noexcept -> FlagField<Enum> {return FlagField<Enum> (lhs) | rhs;}
	template <sl::utils::FlagEnum Enum>
	constexpr auto operator|(Enum lhs, FlagField<Enum> rhs) noexcept -> FlagField<Enum> {return rhs |= lhs;}
	template <sl::utils::FlagEnum Enum>
	constexpr auto operator^(Enum lhs, Enum rhs) noexcept -> FlagField<Enum> {return FlagField<Enum> (lhs) ^ rhs;}
	template <sl::utils::FlagEnum Enum>
	constexpr auto operator^(Enum lhs, FlagField<Enum> rhs) noexcept -> FlagField<Enum> {return rhs ^= lhs;}
	template <sl::utils::FlagEnum Enum>
	constexpr auto operator~(Enum flag) noexcept -> FlagField<Enum> {return ~FlagField<Enum> (flag);}


	SL_CORE auto toString(PackedEnumString value) noexcept -> sl::String;
	template <sl::utils::StringEnum Enum>
	inline auto toString(Enum value) noexcept -> sl::String;

	template <sl::utils::FlagEnum Enum>
	auto toString(const FlagField<Enum> &field) noexcept -> sl::String;
	template <sl::utils::FlagEnum Enum>
	inline auto toString(Enum flag) noexcept -> sl::String;


	namespace literals {
		constexpr auto operator ""_pes(const char *str, std::size_t length) noexcept -> sl::utils::PackedEnumString;
		constexpr auto operator ""_feb(unsigned long long position) noexcept -> sl::utils::FlagEnumBit;
	} // namespace literals


	constexpr auto operator<<(std::ostream &stream, PackedEnumString value) noexcept -> std::ostream& {return stream << toString(value);}
	template <sl::utils::StringEnum Enum>
	inline auto operator<<(std::ostream &stream, Enum value) noexcept -> std::ostream& {return stream << toString(value);}

	template <sl::utils::FlagEnum Enum>
	constexpr auto operator<<(std::ostream &stream, const FlagField<Enum> &field) noexcept -> std::ostream& {return stream << toString(field);}
	template <sl::utils::FlagEnum Enum>
	inline auto operator<<(std::ostream &stream, Enum flag) noexcept -> std::ostream& {return stream << toString(flag);}

	using namespace sl::utils::literals;
} // namespace sl::utils

#include "sl/utils/enums.inl"


template <>
struct std::formatter<sl::utils::PackedEnumString> : std::formatter<sl::String> {
	constexpr auto format(sl::utils::PackedEnumString value, std::format_context &ctx) const noexcept {
		return std::formatter<sl::String>::format(sl::utils::toString(value), ctx);
	}
};

template <sl::utils::StringEnum Enum>
struct std::formatter<Enum> : std::formatter<sl::String> {
	constexpr auto format(Enum value, std::format_context &ctx) const noexcept {
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
		text.reserve(ctx.end() - ctx.begin());
		text.pushBack(it, ctx.end() - 1);
		it = ctx.end() - 1;
		if (text.isEmpty())
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

		if (text.getSize() > static_cast<std::size_t> (m_bitShown)) {
			while (text.getSize() != static_cast<std::size_t> (m_bitShown))
				text.erase(0, 1);
		}

		else if (text.getSize() < static_cast<std::size_t> (m_bitShown)) {
			sl::String missing {};
			missing.reserve(m_bitShown - text.getSize());
			for (auto i {text.getSize()}; i != static_cast<std::size_t> (m_bitShown); ++i)
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

