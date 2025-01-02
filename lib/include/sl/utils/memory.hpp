#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>


namespace sl::utils {
	template <typename T>
	constexpr auto memcpy(T *dest, const T *src, std::size_t count) -> T* {
		if (!std::is_constant_evaluated())
			return reinterpret_cast<T*> (std::memcpy(dest, src, sizeof(T) * count));
		for (std::size_t i {0}; i < count; ++i)
			dest[i] = src[i];
		return dest;
	}

	template <typename T>
	constexpr auto memset(T *dest, const T &value, std::size_t count) -> T* {
		for (std::size_t i {0}; i < count; ++i)
			dest[i] = value;
		return dest;
	}

	template <typename T>
	constexpr auto memmove(T *dest, const T *src, std::size_t count) -> T* {
		if (!std::is_constant_evaluated())
			return reinterpret_cast<T*> (std::memmove(dest, src, sizeof(T) * count));
		if (dest > src) {
			for (std::size_t i {count}; i-- != 0;)
				dest[i] = src[i];
			return dest;
		}

		for (std::size_t i {0}; i < count; ++i)
			dest[i] = src[i];
		return dest;
	}

} // namespace sl::utils
