#include "sl/utils/enums.hpp"


namespace sl::utils {
	sl::String toString(sl::utils::PackedEnumString value) noexcept {
		constexpr std::size_t MAX_STRING_SIZE {10};
		constexpr std::size_t CHARACTER_SIZE {6};
		constexpr std::size_t CHARACTER_FILTER {0b00111111};
		sl::String result {};
		result.reserve(MAX_STRING_SIZE);

		for (std::size_t i {0}; i < MAX_STRING_SIZE; ++i) {
			sl::utils::PackedEnumString current {value};
			current >>= i * CHARACTER_SIZE;
			current &= CHARACTER_FILTER;
			if (current == 0)
				continue;
			--current;

			char character {};
			if (current < 26)
				character = 'A' + current;
			else if (current < 52)
				character = 'a' + current - 26;
			else if (current < 62)
				character = '0' + current - 52;
			else if (current == 62)
				character = '_';
			result.pushBack(character);
		}

		return result;
	}

} // namespace sl::utils
