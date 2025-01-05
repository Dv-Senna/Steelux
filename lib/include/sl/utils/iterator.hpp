#pragma once

#include <compare>
#include <iterator>


namespace sl::utils {
	template <typename Cont, typename T>
	class ContinousIterator {
		public:
			using iterator_category = std::contiguous_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			constexpr ContinousIterator() noexcept;
			constexpr ContinousIterator(Cont *container, pointer ptr) noexcept;
			constexpr ~ContinousIterator() = default;

			constexpr ContinousIterator(const ContinousIterator<Cont, T> &iterator) noexcept;
			constexpr auto operator=(const ContinousIterator<Cont, T> &iterator) noexcept -> ContinousIterator<Cont, T>&;

			constexpr operator bool() const noexcept;

			constexpr auto operator==(const ContinousIterator<Cont, T> &iterator) const noexcept -> bool;
			constexpr auto operator<=>(const ContinousIterator<Cont, T> &iterator) const noexcept -> std::partial_ordering;

			constexpr auto operator+=(difference_type diff) noexcept -> ContinousIterator<Cont, T>&;
			constexpr auto operator-=(difference_type diff) noexcept -> ContinousIterator<Cont, T>&;
			template <std::integral DIFF>
			constexpr auto operator+=(DIFF diff) noexcept -> ContinousIterator<Cont, T>& {return *this += static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr auto operator-=(DIFF diff) noexcept -> ContinousIterator<Cont, T>& {return *this -= static_cast<difference_type> (diff);}

			constexpr auto operator++() noexcept -> ContinousIterator<Cont, T>&;
			constexpr auto operator++(int) noexcept -> ContinousIterator<Cont, T>;
			constexpr auto operator--() noexcept -> ContinousIterator<Cont, T>&;
			constexpr auto operator--(int) noexcept -> ContinousIterator<Cont, T>;

			constexpr auto operator+(difference_type diff) const noexcept -> ContinousIterator<Cont, T>;
			constexpr auto operator-(difference_type diff) const noexcept -> ContinousIterator<Cont, T>;
			template <std::integral DIFF>
			constexpr auto operator+(DIFF diff) const noexcept -> ContinousIterator<Cont, T> {return *this + static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr auto operator-(DIFF diff) const noexcept -> ContinousIterator<Cont, T> {return *this - static_cast<difference_type> (diff);}

			constexpr auto operator-(const ContinousIterator<Cont, T> &iterator) const noexcept -> difference_type;

			constexpr auto operator*() const noexcept -> reference;
			constexpr auto operator->() const noexcept -> pointer;

			constexpr auto operator[](difference_type diff) const noexcept -> reference;

			constexpr auto getContainer() const noexcept -> Cont* {return m_container;}
			constexpr auto getPtr() const noexcept -> pointer {return m_ptr;}


		protected:
			Cont *m_container;
			pointer m_ptr;
	};

	template <typename Cont, typename T>
	constexpr auto operator+(typename ContinousIterator<Cont, T>::difference_type diff, const ContinousIterator<Cont, T> &iterator) noexcept -> ContinousIterator<Cont, T> {
		return iterator + diff;
	}



	template <typename Cont, typename T>
	class ReverseContinousIterator {
		public:
			using iterator_category = std::contiguous_iterator_tag;
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

			constexpr auto operator==(const ReverseContinousIterator<Cont, T> &iterator) const noexcept -> bool;
			constexpr auto operator<=>(const ReverseContinousIterator<Cont, T> &iterator) const noexcept -> std::partial_ordering;

			constexpr auto operator+=(difference_type diff) noexcept -> ReverseContinousIterator<Cont, T>&;
			constexpr auto operator-=(difference_type diff) noexcept -> ReverseContinousIterator<Cont, T>&;
			template <std::integral DIFF>
			constexpr auto operator+=(DIFF diff) noexcept -> ReverseContinousIterator<Cont, T>& {return *this += static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr auto operator-=(DIFF diff) noexcept -> ReverseContinousIterator<Cont, T>& {return *this -= static_cast<difference_type> (diff);}

			constexpr auto operator++() noexcept -> ReverseContinousIterator<Cont, T>&;
			constexpr auto operator++(int) noexcept -> ReverseContinousIterator<Cont, T>;
			constexpr auto operator--() noexcept -> ReverseContinousIterator<Cont, T>&;
			constexpr auto operator--(int) noexcept -> ReverseContinousIterator<Cont, T>;

			constexpr auto operator+(difference_type diff) const noexcept -> ReverseContinousIterator<Cont, T>;
			constexpr auto operator-(difference_type diff) const noexcept -> ReverseContinousIterator<Cont, T>;
			template <std::integral DIFF>
			constexpr auto operator+(DIFF diff) const noexcept -> ReverseContinousIterator<Cont, T> {return *this + static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr auto operator-(DIFF diff) const noexcept -> ReverseContinousIterator<Cont, T> {return *this - static_cast<difference_type> (diff);}

			constexpr auto operator-(const ReverseContinousIterator<Cont, T> &iterator) const noexcept -> difference_type;

			constexpr auto operator*() const noexcept -> reference;
			constexpr auto operator->() const noexcept -> pointer;

			constexpr auto operator[](difference_type diff) const noexcept -> reference;

			constexpr auto getContainer() const noexcept -> Cont* {return m_container;}
			constexpr auto getPtr() const noexcept -> pointer {return m_ptr;}


		protected:
			Cont *m_container;
			pointer m_ptr;
	};

	template <typename Cont, typename T>
	constexpr auto operator+(
		typename ReverseContinousIterator<Cont, T>::difference_type diff, const ReverseContinousIterator<Cont, T> &iterator
	) noexcept -> ReverseContinousIterator<Cont, T> {
		return iterator + diff;
	}

	static_assert(std::contiguous_iterator<ContinousIterator<void, char>>, "ContinousIterator must fullfill std::random_access_iterator");
	static_assert(std::contiguous_iterator<ReverseContinousIterator<void, char>>, "ReverseContinousIteartor must fullfill std::random_access_iterator");



	template <typename Cont, typename T, typename Ptr>
	class BasicTwoTypesContinousIterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			constexpr BasicTwoTypesContinousIterator() noexcept;
			constexpr BasicTwoTypesContinousIterator(Cont *container, pointer ptr) noexcept;
			constexpr BasicTwoTypesContinousIterator(Cont *container, const Ptr &ptr) noexcept;
			constexpr ~BasicTwoTypesContinousIterator() = default;

			constexpr BasicTwoTypesContinousIterator(const BasicTwoTypesContinousIterator<Cont, T, Ptr> &iterator) noexcept;
			constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr> &operator=(const BasicTwoTypesContinousIterator<Cont, T, Ptr> &iterator) noexcept;

			constexpr operator bool() const noexcept;

			constexpr auto operator==(const BasicTwoTypesContinousIterator<Cont, T, Ptr> &iterator) const noexcept -> bool;
			constexpr auto operator<=>(const BasicTwoTypesContinousIterator<Cont, T, Ptr> &iterator) const noexcept -> std::partial_ordering;

			constexpr auto operator+=(difference_type diff) noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr>&;
			constexpr auto operator-=(difference_type diff) noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr>&;
			template <std::integral DIFF>
			constexpr auto operator+=(DIFF diff) noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr>& {return *this += static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr auto operator-=(DIFF diff) noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr>& {return *this -= static_cast<difference_type> (diff);}

			constexpr auto operator++() noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr>&;
			constexpr auto operator++(int) noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr>;
			constexpr auto operator--() noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr>&;
			constexpr auto operator--(int) noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr>;

			constexpr auto operator+(difference_type diff) const noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr>;
			constexpr auto operator-(difference_type diff) const noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr>;
			template <std::integral DIFF>
			constexpr auto operator+(DIFF diff) const noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr> {return *this + static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr auto operator-(DIFF diff) const noexcept -> BasicTwoTypesContinousIterator<Cont, T, Ptr> {return *this - static_cast<difference_type> (diff);}

			constexpr auto operator-(const BasicTwoTypesContinousIterator<Cont, T, Ptr> &iterator) const noexcept -> difference_type;

			constexpr auto operator*() const noexcept -> reference;
			constexpr auto operator->() const noexcept -> pointer;

			constexpr auto operator[](difference_type diff) const noexcept -> reference;

			constexpr auto getContainer() const noexcept -> Cont* {return m_container;}
			constexpr auto getPtr() const noexcept -> pointer {return m_isFirst ? m_ptr.first : &*m_ptr.second;}


		protected:
			Cont *m_container;
			bool m_isFirst;
			union {
				pointer first;
				Ptr second;
			} m_ptr;
	};

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr> operator+(
		typename BasicTwoTypesContinousIterator<Cont, T, Ptr>::difference_type diff,
		const BasicTwoTypesContinousIterator<Cont, T, Ptr> &iterator
	) noexcept {
		return iterator + diff;
	}



	template <typename Cont, typename T, typename Ptr>
	class BasicReverseTwoTypesContinousIterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

			constexpr BasicReverseTwoTypesContinousIterator() noexcept;
			constexpr BasicReverseTwoTypesContinousIterator(Cont *container, pointer ptr) noexcept;
			constexpr BasicReverseTwoTypesContinousIterator(Cont *container, const Ptr &ptr) noexcept;
			constexpr ~BasicReverseTwoTypesContinousIterator() = default;

			constexpr BasicReverseTwoTypesContinousIterator(const BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &iterator) noexcept;
			constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &operator=(const BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &iterator) noexcept;

			constexpr operator bool() const noexcept;

			constexpr auto operator==(const BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &iterator) const noexcept -> bool;
			constexpr auto operator<=>(const BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &iterator) const noexcept -> std::partial_ordering;

			constexpr auto operator+=(difference_type diff) noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>&;
			constexpr auto operator-=(difference_type diff) noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>&;
			template <std::integral DIFF>
			constexpr auto operator+=(DIFF diff) noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>& {return *this += static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr auto operator-=(DIFF diff) noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>& {return *this -= static_cast<difference_type> (diff);}

			constexpr auto operator++() noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>&;
			constexpr auto operator++(int) noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>;
			constexpr auto operator--() noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>&;
			constexpr auto operator--(int) noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>;

			constexpr auto operator+(difference_type diff) const noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>;
			constexpr auto operator-(difference_type diff) const noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>;
			template <std::integral DIFF>
			constexpr auto operator+(DIFF diff) const noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> {return *this + static_cast<difference_type> (diff);}
			template <std::integral DIFF>
			constexpr auto operator-(DIFF diff) const noexcept -> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> {return *this - static_cast<difference_type> (diff);}

			constexpr auto operator-(const BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &iterator) const noexcept -> difference_type;

			constexpr auto operator*() const noexcept -> reference;
			constexpr auto operator->() const noexcept -> pointer;

			constexpr auto operator[](difference_type diff) const noexcept -> reference;

			constexpr auto getContainer() const noexcept -> Cont* {return m_container;}
			constexpr auto getPtr() const noexcept -> pointer {return m_isFirst ? m_ptr.first : &*m_ptr.second;}


		protected:
			Cont *m_container;
			bool m_isFirst;
			union {
				pointer first;
				Ptr second;
			} m_ptr;
	};

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> operator+(
		typename BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::difference_type diff,
		const BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &iterator
	) noexcept {
		return iterator + diff;
	}


	template <typename Cont, typename T, typename Ptr>
	struct BasicTwoTypesContinousIteratorAlias {using Type = BasicTwoTypesContinousIterator<Cont, T, Ptr>;};
	template <typename Cont, typename T>
	struct BasicTwoTypesContinousIteratorAlias<Cont, T, T*> {using Type = ContinousIterator<Cont, T>;};
	template <typename Cont, typename T, typename Ptr>
	struct BasicReverseTwoTypesContinousIteratorAlias {using Type = BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>;};
	template <typename Cont, typename T>
	struct BasicReverseTwoTypesContinousIteratorAlias<Cont, T, T*> {using Type = ReverseContinousIterator<Cont, T>;};

	template <typename Cont, typename T, typename Ptr>
	using TwoTypesContinousIterator = typename BasicTwoTypesContinousIteratorAlias<Cont, T, Ptr>::Type;
	template <typename Cont, typename T, typename Ptr>
	using ReverseTwoTypesContinousIterator = typename BasicReverseTwoTypesContinousIteratorAlias<Cont, T, Ptr>::Type;


	static_assert(std::random_access_iterator<BasicTwoTypesContinousIterator<void, char, int*>>, "TwoTypesContinousIterator must fullfill std::random_access_iterator");
	static_assert(std::random_access_iterator<BasicReverseTwoTypesContinousIterator<void, char, int*>>, "ReverseTwoTypesContinousIteartor must fullfill std::random_access_iterator");
} // namespace sl::utils

#include "sl/utils/iterator.inl"
