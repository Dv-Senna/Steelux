#pragma once

#include <algorithm>
#include <print>
#include <utility>

#include "sl/utils/enums.hpp"


namespace sl::utils {
	template <sl::utils::StringEnum Enum>
	sl::String toString(Enum value) noexcept {
		return toString(static_cast<sl::utils::PackedEnumString> (value));
	}


	template <sl::utils::FlagEnum Enum>
	sl::String toString(const FlagField<Enum> &field) noexcept {
		std::size_t size {0};
		for (auto value {static_cast<sl::utils::FlagEnumBit> (field)}; value != 0; value >>= 1) ++size;
		if (size == 0)
			size = 1;
		sl::String result {};
		result.reserve(size);

		for (auto mask {static_cast<sl::utils::FlagEnumBit> (1ULL << (size - 1))}; mask != 0; mask >>= 1)
			result.pushBack(field & static_cast<Enum> (mask) ? '1' : '0');
		return result;
	}

	template <sl::utils::FlagEnum Enum>
	sl::String toString(Enum flag) noexcept {
		return toString(FlagField<Enum> (flag));
	}


	namespace literals {
		constexpr sl::utils::PackedEnumString operator ""_pes(const char *str, std::size_t length) noexcept {
			#define __FAILURE(...) std::unreachable()

			constexpr std::size_t MAX_STRING_SIZE {10};
			constexpr std::size_t CHARACTER_SIZE {6};

			sl::utils::PackedEnumString result {0};

			for (std::size_t i {0}; i < std::min(length, MAX_STRING_SIZE); ++i) {
				sl::utils::PackedEnumString current {};
				if (str[i] >= 'A' && str[i] <= 'Z')
					current = str[i] - 'A';
				else if (str[i] >= 'a' && str[i] <= 'z')
					current = str[i] - 'a' + 26;
				else if (str[i] >= '0' && str[i] <= '9')
					current = str[i] - '0' + 52;
				else if (str[i] == '_')
					current = 62;
				else
					__FAILURE("This only support the following characters : A-Z, a-z, 0-9 and _");

				result |= (current + 1) << (CHARACTER_SIZE * i);
			}

			return result;

			#undef __FAILURE
		}


		constexpr sl::utils::FlagEnumBit operator ""_feb(unsigned long long position) noexcept {
			return static_cast<FlagEnumBit> (1 << position);
		}

	} // namespace literals

} // namespace sl::utils
