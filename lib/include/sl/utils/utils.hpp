#pragma once

#include <tuple>


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

} // namespace sl::utils
