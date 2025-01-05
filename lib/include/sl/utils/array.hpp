#pragma once

#include <cstddef>
#include <format>
#include <ranges>
#include <sstream>

#include "sl/utils/iterator.hpp"


namespace sl::utils {
	template <typename T, std::size_t D>
	class Array {
		public:
			using value_type = T;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using reference = value_type&;
			using const_reference = const value_type&;
			using pointer = T*;
			using const_pointer = const T*;
			
			using iterator = sl::utils::ContinousIterator<Array<T, D>, T>;
			using const_iterator = sl::utils::ContinousIterator<const Array<T, D>, const T>;
			using reverse_iterator = sl::utils::ReverseContinousIterator<Array<T, D>, T>;
			using const_reverse_iterator = sl::utils::ReverseContinousIterator<const Array<T, D>, const T>;

			Array() noexcept;
			template <std::convertible_to<T> T2>
			constexpr Array(T2 &&fill) noexcept;
			template <std::convertible_to<T> ...Args>
			requires (sizeof...(Args) <= D && 1 < sizeof...(Args))
			constexpr Array(Args &&...args) noexcept;
			constexpr ~Array() = default;

			constexpr Array(const Array<T, D> &array) noexcept = default;
			constexpr auto operator=(const Array<T, D> &array) noexcept -> Array<T, D>& = default;
			constexpr Array(Array<T, D> &&array) noexcept = default;
			constexpr auto operator=(Array<T, D> &&array) noexcept -> Array<T, D>& = default;

			constexpr auto operator==(const Array<T, D> &array) const noexcept -> bool;

			constexpr auto at(ptrdiff_t index) noexcept -> iterator;
			constexpr auto at(ptrdiff_t index) const noexcept -> const_iterator;
			constexpr auto operator[](ptrdiff_t index) noexcept -> reference {return *this->at(index);}
			constexpr auto operator[](ptrdiff_t index) const noexcept -> const_reference {return *this->at(index);}

			constexpr auto begin() noexcept -> iterator;
			constexpr auto end() noexcept -> iterator;
			constexpr auto cbegin() noexcept -> const_iterator;
			constexpr auto cend() noexcept -> const_iterator;
			constexpr auto begin() const noexcept -> const_iterator {return this->cbegin();}
			constexpr auto end() const noexcept -> const_iterator {return this->cend();}
			constexpr auto rbegin() noexcept -> reverse_iterator;
			constexpr auto rend() noexcept -> reverse_iterator;
			constexpr auto crbegin() noexcept -> const_reverse_iterator;
			constexpr auto crend() noexcept -> const_reverse_iterator;
			constexpr auto rbegin() const noexcept -> const_reverse_iterator {return this->crbegin();}
			constexpr auto rend() const noexcept -> const_reverse_iterator {return this->crend();}

			constexpr auto getData() const noexcept -> pointer {return m_data;}
			constexpr auto getSize() const noexcept -> size_type {return D;}

		private:
			constexpr auto m_normalizeIndex(ptrdiff_t index) const noexcept -> ptrdiff_t;

			T m_data[D];
	};

	
	static_assert(std::ranges::contiguous_range<Array<char, 10>>);

	template <typename T, std::size_t D>
	inline auto operator<<(std::ostream &stream, const Array<T, D> &array) noexcept -> std::ostream& {
		stream << "[";
		for (std::size_t i {0}; i < D; ++i) {
			stream << array[i];
			if (i != D - 1)
				stream << ", ";
		}
		return stream << "]";
	}

} // namespace sl::utils

#include "sl/utils/array.inl"

namespace sl {
	template <typename T, std::size_t D>
	using Array = sl::utils::Array<T, D>;
} // namespace sl


template <typename T, std::size_t D>
struct std::formatter<sl::Array<T, D>> {
	constexpr auto parse(std::format_parse_context &ctx) noexcept {
		return ctx.begin();
	}

	inline auto format(const sl::Array<T, D> &array, std::format_context &ctx) const noexcept {
		std::ostringstream stream {};
		stream << array;
		return std::ranges::copy(stream.str(), ctx.out()).out;
	}
};
