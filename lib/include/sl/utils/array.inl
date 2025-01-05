#pragma once

#include "sl/utils/array.hpp"


namespace sl::utils {
	template <typename T, std::size_t D>
	Array<T, D>::Array() noexcept :
		m_data {}
	{

	}


	template <typename T, std::size_t D>
	template <std::convertible_to<T> T2>
	constexpr Array<T, D>::Array(T2 &&fill) noexcept :
		m_data {}
	{
		for (std::size_t i {0}; i < D; ++i)
			m_data[i] = fill;
	}


	template <typename T, std::size_t D>
	template <std::convertible_to<T> ...Args>
	requires (sizeof...(Args) <= D && 1 < sizeof...(Args))
	constexpr Array<T, D>::Array(Args &&...args) noexcept :
		m_data {args...}
	{

	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::operator==(const Array<T, D> &array) const noexcept -> bool {
		for (std::size_t i {0}; i < D; ++i) {
			if (m_data[i] != array.m_data[i])
				return false;
		}
		return true;
	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::at(ptrdiff_t index) noexcept -> iterator {
		index = this->m_normalizeIndex(index);
		return iterator(this, m_data + index);
	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::at(ptrdiff_t index) const noexcept -> const_iterator {
		index = this->m_normalizeIndex(index);
		return const_iterator(this, m_data + index);
	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::begin() noexcept -> iterator {
		return iterator(this, m_data);
	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::end() noexcept -> iterator {
		return iterator(this, m_data + D);
	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::cbegin() noexcept -> const_iterator {
		return const_iterator(this, m_data);
	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::cend() noexcept -> const_iterator {
		return const_iterator(this, m_data + D);
	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::rbegin() noexcept -> reverse_iterator {
		return reverse_iterator(this, m_data + D - 1);
	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::rend() noexcept -> reverse_iterator {
		return reverse_iterator(this, m_data - 1);
	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::crbegin() noexcept -> const_reverse_iterator {
		return const_reverse_iterator(this, m_data + D - 1);
	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::crend() noexcept -> const_reverse_iterator {
		return const_reverse_iterator(this, m_data - 1);
	}


	template <typename T, std::size_t D>
	constexpr auto Array<T, D>::m_normalizeIndex(ptrdiff_t index) const noexcept -> ptrdiff_t {
		index %= static_cast<difference_type> (D);
		if (index < 0)
			index += static_cast<difference_type> (D);
		return index;
	}

} // namespace sl::utils
