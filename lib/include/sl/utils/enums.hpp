#pragma once

#include <cstdint>
#include <type_traits>

#include "sl/utils/string.hpp"


namespace sl::utils {
	// Pack a string composed only of characters a-z, A-Z, 0-9 and _ into
	// a 64-bits integer. Used to provide string value to enums. Can contain
	// up to 10 characters
	using PackedEnumString = std::uint64_t;

	template <typename T>
	concept StringEnum = requires {
		std::is_enum_v<T>;
		std::is_same_v<std::underlying_type_t<T>, sl::utils::PackedEnumString>;
	};

	sl::utils::String toString(PackedEnumString value);
	template <sl::utils::StringEnum Enum>
	inline sl::utils::String toString(Enum value);


	namespace literals {
		constexpr sl::utils::PackedEnumString operator ""_pes(const char *str, std::size_t length) noexcept;
	} // namespace literals

	using namespace sl::utils::literals;
} // namespace sl::utils

#include "sl/utils/enums.inl"
