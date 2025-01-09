#pragma once

#include <algorithm>
#include <cstdint>
#include <tuple>

#include "sl/utils/string.hpp"


namespace sl::utils {
	template <std::size_t N, typename T, std::same_as<T> ...Args>
	struct TupleFiller;

	template <bool V, std::size_t N, typename T, std::same_as<T> ...Args>
	struct __TupleFillerInternal {
		using Type = typename TupleFiller<N, T, Args..., T>::Type;
	};

	template <std::size_t N, typename T, std::same_as<T> ...Args>
	struct __TupleFillerInternal<true, N, T, Args...> {
		using Type = std::tuple<T, Args...>;
	};

	template <std::size_t N, typename T, std::same_as<T> ...Args>
	struct TupleFiller {
		using PreviousType = typename __TupleFillerInternal<sizeof...(Args) == N - 2, N - 1, T, Args..., T>::Type;
		using Type = typename __TupleFillerInternal<sizeof...(Args) == N - 1, N, T, Args..., T>::Type;

		constexpr static auto fill(T &&value) noexcept -> Type {
			if constexpr (N <= 1)
				return std::make_tuple(std::forward(value));
			else
				return std::tuple_cat(std::make_tuple(std::forward(value)), TupleFiller<N-1, T>::fill(value));
		}
	};


	struct Version {
		std::uint32_t major {};
		std::uint32_t minor {};
		std::uint32_t patch {};
	};


	namespace literals {
		constexpr auto operator ""_ver(const char *str, std::size_t N) -> Version {
			Version version {};
			const char *lastDot {str};
			for (std::size_t index {0}; lastDot < str + N && index < 3; ++index, ++lastDot) {
				const char *currentDot {std::find(lastDot, str + N, '.')};
				sl::String stringNumber {lastDot, static_cast<std::size_t> (currentDot - lastDot)};
				lastDot = currentDot;

				std::optional<std::uint32_t> number {sl::utils::stringToNumber<std::uint32_t> (stringNumber)};
				if (!number)
					throw "Invalid version string";

				(&version.major)[index] = *number;
			}
			return version;
		}

	} // namespace literals

	using namespace sl::utils::literals;
} // namespace sl::utils
