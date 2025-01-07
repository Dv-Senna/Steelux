#pragma once

#include "sl/utils/hash.hpp"


namespace sl::utils {
	template <std::unsigned_integral T, typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto hash(const sl::utils::BasicString<CharT, Alloc> &string) noexcept -> Hash<T> {
		T hash {};
		T word {};
		std::size_t wordPosition {};
		constexpr std::size_t CHAR_PER_WORD {sizeof(T) / sizeof(CharT)};

		for (auto it {string.begin()}; it != string.end(); ++it) {
			if (wordPosition < CHAR_PER_WORD - 1) {
				word |= static_cast<T> (*it) << (sizeof(CharT) * wordPosition);
				++wordPosition;
				continue;
			}

			word |= static_cast<T> (*it) << (sizeof(CharT) * (CHAR_PER_WORD - 1));
			hash ^= word;

			word = 0;
			wordPosition = 0;
		}

		if (wordPosition != 0)
			hash ^= word;

		return Hash<T> (hash);
	}


	template <std::unsigned_integral T>
	constexpr auto hash(const char *str, std::size_t N) noexcept -> Hash<T> {
		T hash {};
		T word {};
		std::size_t wordPosition {};
		constexpr std::size_t CHAR_PER_WORD {sizeof(T) / sizeof(char)};

		for (std::size_t i {0}; i != N; ++i) {
			if (wordPosition < CHAR_PER_WORD - 1) {
				word |= static_cast<T> (str[i]) << (sizeof(char) * wordPosition);
				++wordPosition;
				continue;
			}

			word |= static_cast<T> (str[i]) << (sizeof(char) * (CHAR_PER_WORD - 1));
			hash ^= word;

			word = 0;
			wordPosition = 0;
		}

		if (wordPosition != 0)
			hash ^= word;

		return Hash<T> (hash);
	}

} // namespace sl::utils
