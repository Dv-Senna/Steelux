#pragma once

#include <compare>
#include <iterator>


namespace sl::utils {
	template <typename Cont, typename T>
	class ContinousIterator final {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			constexpr ContinousIterator() noexcept;
			constexpr ContinousIterator(Cont *container, pointer ptr) noexcept;
			constexpr ~ContinousIterator() = default;

			constexpr ContinousIterator(const ContinousIterator<Cont, T> &iterator) noexcept;
			constexpr ContinousIterator<Cont, T> &operator=(const ContinousIterator<Cont, T> &iterator) noexcept;

			constexpr operator bool() const noexcept;

			constexpr bool operator==(const ContinousIterator<Cont, T> &iterator) const noexcept;
			constexpr std::partial_ordering operator<=>(const ContinousIterator<Cont, T> &iterator) const noexcept;

			constexpr ContinousIterator<Cont, T> &operator+=(difference_type diff) noexcept;
			constexpr ContinousIterator<Cont, T> &operator-=(difference_type diff) noexcept;
			template <std::integral DIFF>
			constexpr ContinousIterator<Cont, T> &operator+=(DIFF diff) noexcept {return *this += static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr ContinousIterator<Cont, T> &operator-=(DIFF diff) noexcept {return *this -= static_cast<difference_type> (diff);}

			constexpr ContinousIterator<Cont, T> &operator++() noexcept;
			constexpr ContinousIterator<Cont, T> operator++(int) noexcept;
			constexpr ContinousIterator<Cont, T> &operator--() noexcept;
			constexpr ContinousIterator<Cont, T> operator--(int) noexcept;

			constexpr ContinousIterator<Cont, T> operator+(difference_type diff) const noexcept;
			constexpr ContinousIterator<Cont, T> operator-(difference_type diff) const noexcept;
			template <std::integral DIFF>
			constexpr ContinousIterator<Cont, T> operator+(DIFF diff) const noexcept {return *this + static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr ContinousIterator<Cont, T> operator-(DIFF diff) const noexcept {return *this - static_cast<difference_type> (diff);}

			constexpr difference_type operator-(const ContinousIterator<Cont, T> &iterator) const noexcept;

			constexpr reference operator*() const noexcept;
			constexpr pointer operator->() const noexcept;

			constexpr reference operator[](difference_type diff) const noexcept;

			constexpr Cont *getContainer() const noexcept {return m_container;}
			constexpr pointer getPtr() const noexcept {return m_ptr;}


		protected:
			Cont *m_container;
			pointer m_ptr;
	};

	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T> operator+(typename ContinousIterator<Cont, T>::difference_type diff, const ContinousIterator<Cont, T> &iterator) noexcept {
		return iterator + diff;
	}



	template <typename Cont, typename T>
	class ReverseContinousIterator final {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			constexpr ReverseContinousIterator() noexcept;
			constexpr ReverseContinousIterator(Cont *container, pointer ptr) noexcept;
			constexpr ~ReverseContinousIterator() = default;

			constexpr ReverseContinousIterator(const ReverseContinousIterator<Cont, T> &iterator) noexcept;
			constexpr ReverseContinousIterator<Cont, T> &operator=(const ReverseContinousIterator<Cont, T> &iterator) noexcept;

			constexpr operator bool() const noexcept;

			constexpr bool operator==(const ReverseContinousIterator<Cont, T> &iterator) const noexcept;
			constexpr std::partial_ordering operator<=>(const ReverseContinousIterator<Cont, T> &iterator) const noexcept;

			constexpr ReverseContinousIterator<Cont, T> &operator+=(difference_type diff) noexcept;
			constexpr ReverseContinousIterator<Cont, T> &operator-=(difference_type diff) noexcept;
			template <std::integral DIFF>
			constexpr ContinousIterator<Cont, T> &operator+=(DIFF diff) noexcept {return *this += static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr ContinousIterator<Cont, T> &operator-=(DIFF diff) noexcept {return *this -= static_cast<difference_type> (diff);}

			constexpr ReverseContinousIterator<Cont, T> &operator++() noexcept;
			constexpr ReverseContinousIterator<Cont, T> operator++(int) noexcept;
			constexpr ReverseContinousIterator<Cont, T> &operator--() noexcept;
			constexpr ReverseContinousIterator<Cont, T> operator--(int) noexcept;

			constexpr ReverseContinousIterator<Cont, T> operator+(difference_type diff) const noexcept;
			constexpr ReverseContinousIterator<Cont, T> operator-(difference_type diff) const noexcept;
			template <std::integral DIFF>
			constexpr ContinousIterator<Cont, T> operator+(DIFF diff) const noexcept {return *this + static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr ContinousIterator<Cont, T> operator-(DIFF diff) const noexcept {return *this - static_cast<difference_type> (diff);}

			constexpr difference_type operator-(const ReverseContinousIterator<Cont, T> &iterator) const noexcept;

			constexpr reference operator*() const noexcept;
			constexpr pointer operator->() const noexcept;

			constexpr reference operator[](difference_type diff) const noexcept;

			constexpr Cont *getContainer() const noexcept {return m_container;}
			constexpr pointer getPtr() const noexcept {return m_ptr;}


		protected:
			Cont *m_container;
			pointer m_ptr;
	};

	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T> operator+(typename ReverseContinousIterator<Cont, T>::difference_type diff, const ReverseContinousIterator<Cont, T> &iterator) noexcept {
		return iterator + diff;
	}



	static_assert(std::random_access_iterator<ContinousIterator<void, char>>, "ContinousIterator must fullfill std::random_access_iterator");
	static_assert(std::random_access_iterator<ReverseContinousIterator<void, char>>, "ReverseContinousIteartor must fullfill std::random_access_iterator");
} // namespace sl::utils

#include "sl/utils/iterator.inl"
