#pragma once

#include <cstddef>
#include <cstring>
#include <type_traits>
#include <iterator>


namespace sl::utils {
	template <typename T, typename Ptr = T*, typename ConstPtr = const T*>
	constexpr auto memcpy(Ptr dest, ConstPtr src, std::size_t count) -> void {
		if constexpr ((std::same_as<T*, Ptr> && std::same_as<const T*, Ptr>) || (std::contiguous_iterator<Ptr> && std::contiguous_iterator<ConstPtr>)) {
			if (!std::is_constant_evaluated())
				return (void)std::memcpy(std::to_address(dest), std::to_address(src), sizeof(T) * count);
		}
		for (std::size_t i {0}; i < count; ++i)
			dest[i] = src[i];
	}

	template <typename T, typename Ptr = T*>
	constexpr auto memset(Ptr dest, const T &value, std::size_t count) -> void {
		for (std::size_t i {0}; i < count; ++i)
			dest[i] = value;
	}

	template <typename T, typename Ptr = T*, typename ConstPtr = const T*>
	constexpr auto memmove(Ptr dest, ConstPtr src, std::size_t count) -> void {
		if constexpr ((std::same_as<T*, Ptr> && std::same_as<const T*, Ptr>) || (std::contiguous_iterator<Ptr> && std::contiguous_iterator<ConstPtr>)) {
			if (!std::is_constant_evaluated())
				return (void)std::memmove(std::to_address(dest), std::to_address(src), sizeof(T) * count);
		}
		if (std::to_address(dest) > std::to_address(src)) {
			for (std::size_t i {count}; i-- != 0;)
				dest[i] = src[i];
			return;
		}

		for (std::size_t i {0}; i < count; ++i)
			dest[i] = src[i];
	}

} // namespace sl::utils
