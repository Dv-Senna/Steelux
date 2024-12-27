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

			ContinousIterator() noexcept;
			ContinousIterator(Cont *container, pointer ptr) noexcept;
			~ContinousIterator() = default;

			ContinousIterator(const ContinousIterator<Cont, T> &iterator) noexcept;
			ContinousIterator<Cont, T> &operator=(const ContinousIterator<Cont, T> &iterator) noexcept;

			operator bool() const noexcept;

			bool operator==(const ContinousIterator<Cont, T> &iterator) const noexcept;
			std::partial_ordering operator<=>(const ContinousIterator<Cont, T> &iterator) const noexcept;

			ContinousIterator<Cont, T> &operator+=(difference_type diff) noexcept;
			ContinousIterator<Cont, T> &operator-=(difference_type diff) noexcept;
			template <std::integral DIFF>
			inline ContinousIterator<Cont, T> &operator+=(DIFF diff) noexcept {return *this += static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			inline ContinousIterator<Cont, T> &operator-=(DIFF diff) noexcept {return *this -= static_cast<difference_type> (diff);}

			ContinousIterator<Cont, T> &operator++() noexcept;
			ContinousIterator<Cont, T> operator++(int) noexcept;
			ContinousIterator<Cont, T> &operator--() noexcept;
			ContinousIterator<Cont, T> operator--(int) noexcept;

			ContinousIterator<Cont, T> operator+(difference_type diff) const noexcept;
			ContinousIterator<Cont, T> operator-(difference_type diff) const noexcept;
			template <std::integral DIFF>
			inline ContinousIterator<Cont, T> operator+(DIFF diff) const noexcept {return *this + static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			inline ContinousIterator<Cont, T> operator-(DIFF diff) const noexcept {return *this - static_cast<difference_type> (diff);}

			difference_type operator-(const ContinousIterator<Cont, T> &iterator) const noexcept;

			reference operator*() const noexcept;
			pointer operator->() const noexcept;

			reference operator[](difference_type diff) const noexcept;

			Cont *getContainer() const noexcept {return m_container;}
			pointer getPtr() const noexcept {return m_ptr;}


		protected:
			Cont *m_container;
			pointer m_ptr;
	};

	template <typename Cont, typename T>
	inline ContinousIterator<Cont, T> operator+(typename ContinousIterator<Cont, T>::difference_type diff, const ContinousIterator<Cont, T> &iterator) noexcept {
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

			ReverseContinousIterator() noexcept;
			ReverseContinousIterator(Cont *container, pointer ptr) noexcept;
			~ReverseContinousIterator() = default;

			ReverseContinousIterator(const ReverseContinousIterator<Cont, T> &iterator) noexcept;
			ReverseContinousIterator<Cont, T> &operator=(const ReverseContinousIterator<Cont, T> &iterator) noexcept;

			operator bool() const noexcept;

			bool operator==(const ReverseContinousIterator<Cont, T> &iterator) const noexcept;
			std::partial_ordering operator<=>(const ReverseContinousIterator<Cont, T> &iterator) const noexcept;

			ReverseContinousIterator<Cont, T> &operator+=(difference_type diff) noexcept;
			ReverseContinousIterator<Cont, T> &operator-=(difference_type diff) noexcept;
			template <std::integral DIFF>
			inline ContinousIterator<Cont, T> &operator+=(DIFF diff) noexcept {return *this += static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			inline ContinousIterator<Cont, T> &operator-=(DIFF diff) noexcept {return *this -= static_cast<difference_type> (diff);}

			ReverseContinousIterator<Cont, T> &operator++() noexcept;
			ReverseContinousIterator<Cont, T> operator++(int) noexcept;
			ReverseContinousIterator<Cont, T> &operator--() noexcept;
			ReverseContinousIterator<Cont, T> operator--(int) noexcept;

			ReverseContinousIterator<Cont, T> operator+(difference_type diff) const noexcept;
			ReverseContinousIterator<Cont, T> operator-(difference_type diff) const noexcept;
			template <std::integral DIFF>
			inline ContinousIterator<Cont, T> operator+(DIFF diff) const noexcept {return *this + static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			inline ContinousIterator<Cont, T> operator-(DIFF diff) const noexcept {return *this - static_cast<difference_type> (diff);}

			difference_type operator-(const ReverseContinousIterator<Cont, T> &iterator) const noexcept;

			reference operator*() const noexcept;
			pointer operator->() const noexcept;

			reference operator[](difference_type diff) const noexcept;

			Cont *getContainer() const noexcept {return m_container;}
			pointer getPtr() const noexcept {return m_ptr;}


		protected:
			Cont *m_container;
			pointer m_ptr;
	};

	template <typename Cont, typename T>
	inline ReverseContinousIterator<Cont, T> operator+(typename ReverseContinousIterator<Cont, T>::difference_type diff, const ReverseContinousIterator<Cont, T> &iterator) noexcept {
		return iterator + diff;
	}



	static_assert(std::random_access_iterator<ContinousIterator<void, char>>, "ContinousIterator must fullfill std::random_access_iterator");
	static_assert(std::random_access_iterator<ReverseContinousIterator<void, char>>, "ReverseContinousIteartor must fullfill std::random_access_iterator");
} // namespace sl::utils

#include "sl/utils/iterator.inl"
