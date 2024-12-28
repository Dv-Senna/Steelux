#pragma once

#include "sl/utils/iterator.hpp"


namespace sl::utils {
	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T>::ContinousIterator() noexcept : m_container {nullptr}, m_ptr {nullptr} {}
	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T>::ContinousIterator(Cont *container, pointer ptr) noexcept : m_container {container}, m_ptr {ptr} {}

	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T>::ContinousIterator(const ContinousIterator<Cont, T> &iterator) noexcept : m_container {iterator.m_container}, m_ptr {iterator.m_ptr} {}
	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T> &ContinousIterator<Cont, T>::operator=(const ContinousIterator<Cont, T> &iterator) noexcept {
		m_container = iterator.m_container;
		m_ptr = iterator.m_ptr;
		return *this;
	}

	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T>::operator bool() const noexcept {return m_container != nullptr;}
	template <typename Cont, typename T>
	constexpr bool ContinousIterator<Cont, T>::operator==(const ContinousIterator<Cont, T> &iterator) const noexcept {return m_container == iterator.m_container && m_ptr == iterator.m_ptr;}

	template <typename Cont, typename T>
	constexpr std::partial_ordering ContinousIterator<Cont, T>::operator<=>(const ContinousIterator<Cont, T> &iterator) const noexcept {
		if (m_container == nullptr || iterator.m_container == nullptr)
			return std::partial_ordering::unordered;
		if (m_container != iterator.m_container)
			return std::partial_ordering::unordered;
		return m_ptr <=> iterator.m_ptr;
	}

	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T> &ContinousIterator<Cont, T>::operator+=(difference_type diff) noexcept {m_ptr += diff; return *this;}
	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T> &ContinousIterator<Cont, T>::operator-=(difference_type diff) noexcept {m_ptr -= diff; return *this;}

	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T> &ContinousIterator<Cont, T>::operator++() noexcept {++m_ptr; return *this;}
	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T> ContinousIterator<Cont, T>::operator++(int) noexcept {auto tmp {*this}; ++m_ptr; return tmp;}

	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T> &ContinousIterator<Cont, T>::operator--() noexcept {--m_ptr; return *this;}
	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T> ContinousIterator<Cont, T>::operator--(int) noexcept {auto tmp {*this}; --m_ptr; return tmp;}

	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T> ContinousIterator<Cont, T>::operator+(difference_type diff) const noexcept {auto tmp {*this}; return tmp += diff;}
	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T> ContinousIterator<Cont, T>::operator-(difference_type diff) const noexcept {auto tmp {*this}; return tmp -= diff;}

	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T>::difference_type ContinousIterator<Cont, T>::operator-(const ContinousIterator<Cont, T> &iterator) const noexcept {return m_ptr - iterator.m_ptr;}

	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T>::reference ContinousIterator<Cont, T>::operator*() const noexcept {return *m_ptr;}
	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T>::pointer ContinousIterator<Cont, T>::operator->() const noexcept {return m_ptr;}
	template <typename Cont, typename T>
	constexpr ContinousIterator<Cont, T>::reference ContinousIterator<Cont, T>::operator[](difference_type diff) const noexcept {return m_ptr[diff];}




	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T>::ReverseContinousIterator() noexcept : m_container {nullptr}, m_ptr {nullptr} {}
	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T>::ReverseContinousIterator(Cont *container, pointer ptr) noexcept : m_container {container}, m_ptr {ptr} {}

	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T>::ReverseContinousIterator(const ReverseContinousIterator<Cont, T> &iterator) noexcept :
		m_container {iterator.m_container}, m_ptr {iterator.m_ptr} {}
	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T> &ReverseContinousIterator<Cont, T>::operator=(const ReverseContinousIterator<Cont, T> &iterator) noexcept {
		m_container = iterator.m_container;
		m_ptr = iterator.m_ptr;
		return *this;
	}

	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T>::operator bool() const noexcept {return m_container != nullptr;}
	template <typename Cont, typename T>
	constexpr bool ReverseContinousIterator<Cont, T>::operator==(const ReverseContinousIterator<Cont, T> &iterator) const noexcept {
		return m_container == iterator.m_container && m_ptr == iterator.m_ptr;
	}

	template <typename Cont, typename T>
	constexpr std::partial_ordering ReverseContinousIterator<Cont, T>::operator<=>(const ReverseContinousIterator<Cont, T> &iterator) const noexcept {
		if (m_container == nullptr || iterator.m_container == nullptr)
			return std::partial_ordering::unordered;
		if (m_container != iterator.m_container)
			return std::partial_ordering::unordered;
		return iterator.m_ptr <=> m_ptr;
	}

	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T> &ReverseContinousIterator<Cont, T>::operator+=(difference_type diff) noexcept {m_ptr -= diff; return *this;}
	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T> &ReverseContinousIterator<Cont, T>::operator-=(difference_type diff) noexcept {m_ptr += diff; return *this;}

	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T> &ReverseContinousIterator<Cont, T>::operator++() noexcept {--m_ptr; return *this;}
	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T> ReverseContinousIterator<Cont, T>::operator++(int) noexcept {auto tmp {*this}; --m_ptr; return tmp;}

	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T> &ReverseContinousIterator<Cont, T>::operator--() noexcept {++m_ptr; return *this;}
	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T> ReverseContinousIterator<Cont, T>::operator--(int) noexcept {auto tmp {*this}; ++m_ptr; return tmp;}

	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T> ReverseContinousIterator<Cont, T>::operator+(difference_type diff) const noexcept {auto tmp {*this}; return tmp += diff;}
	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T> ReverseContinousIterator<Cont, T>::operator-(difference_type diff) const noexcept {auto tmp {*this}; return tmp -= diff;}

	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T>::difference_type ReverseContinousIterator<Cont, T>::operator-(const ReverseContinousIterator<Cont, T> &iterator) const noexcept {
		return iterator.m_ptr - m_ptr;
	}

	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T>::reference ReverseContinousIterator<Cont, T>::operator*() const noexcept {return *m_ptr;}
	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T>::pointer ReverseContinousIterator<Cont, T>::operator->() const noexcept {return m_ptr;}
	template <typename Cont, typename T>
	constexpr ReverseContinousIterator<Cont, T>::reference ReverseContinousIterator<Cont, T>::operator[](difference_type diff) const noexcept {return m_ptr[-diff];}

} // namespace sl::utils
