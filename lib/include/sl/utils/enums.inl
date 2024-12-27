#pragma once

#include <algorithm>
#include <print>
#include <utility>

#include "sl/utils/enums.hpp"


namespace sl::utils {
	template <sl::utils::StringEnum Enum>
	sl::utils::String toString(Enum value) {
		return toString(static_cast<sl::utils::PackedEnumString> (value));
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
	} // namespace literals

} // namespace sl::utils
