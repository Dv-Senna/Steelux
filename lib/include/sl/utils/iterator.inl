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
	constexpr auto ContinousIterator<Cont, T>::operator==(const ContinousIterator<Cont, T> &iterator) const noexcept -> bool {
		return m_container == iterator.m_container && m_ptr == iterator.m_ptr;
	}

	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator<=>(const ContinousIterator<Cont, T> &iterator) const noexcept -> std::partial_ordering {
		if (m_container == nullptr || iterator.m_container == nullptr)
			return std::partial_ordering::unordered;
		if (m_container != iterator.m_container)
			return std::partial_ordering::unordered;
		return m_ptr <=> iterator.m_ptr;
	}

	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator+=(difference_type diff) noexcept -> ContinousIterator<Cont, T>& {m_ptr += diff; return *this;}
	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator-=(difference_type diff) noexcept -> ContinousIterator<Cont, T>& {m_ptr -= diff; return *this;}

	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator++() noexcept -> ContinousIterator<Cont, T>& {++m_ptr; return *this;}
	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator++(int) noexcept -> ContinousIterator<Cont, T> {auto tmp {*this}; ++m_ptr; return tmp;}

	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator--() noexcept -> ContinousIterator<Cont, T>& {--m_ptr; return *this;}
	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator--(int) noexcept -> ContinousIterator<Cont, T> {auto tmp {*this}; --m_ptr; return tmp;}

	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator+(difference_type diff) const noexcept -> ContinousIterator<Cont, T> {auto tmp {*this}; return tmp += diff;}
	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator-(difference_type diff) const noexcept -> ContinousIterator<Cont, T> {auto tmp {*this}; return tmp -= diff;}

	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator-(const ContinousIterator<Cont, T> &iterator) const noexcept -> ContinousIterator<Cont, T>::difference_type {
		return m_ptr - iterator.m_ptr;
	}

	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator*() const noexcept -> ContinousIterator<Cont, T>::reference {return *m_ptr;}
	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator->() const noexcept -> ContinousIterator<Cont, T>::pointer {return m_ptr;}
	template <typename Cont, typename T>
	constexpr auto ContinousIterator<Cont, T>::operator[](difference_type diff) const noexcept -> ContinousIterator<Cont, T>::reference {return m_ptr[diff];}




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
	constexpr auto ReverseContinousIterator<Cont, T>::operator==(const ReverseContinousIterator<Cont, T> &iterator) const noexcept -> bool {
		return m_container == iterator.m_container && m_ptr == iterator.m_ptr;
	}

	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator<=>(const ReverseContinousIterator<Cont, T> &iterator) const noexcept -> std::partial_ordering {
		if (m_container == nullptr || iterator.m_container == nullptr)
			return std::partial_ordering::unordered;
		if (m_container != iterator.m_container)
			return std::partial_ordering::unordered;
		return iterator.m_ptr <=> m_ptr;
	}

	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator+=(difference_type diff) noexcept -> ReverseContinousIterator<Cont, T>& {m_ptr -= diff; return *this;}
	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator-=(difference_type diff) noexcept -> ReverseContinousIterator<Cont, T>& {m_ptr += diff; return *this;}

	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator++() noexcept -> ReverseContinousIterator<Cont, T>& {--m_ptr; return *this;}
	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator++(int) noexcept -> ReverseContinousIterator<Cont, T> {auto tmp {*this}; --m_ptr; return tmp;}

	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator--() noexcept -> ReverseContinousIterator<Cont, T>& {++m_ptr; return *this;}
	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator--(int) noexcept -> ReverseContinousIterator<Cont, T> {auto tmp {*this}; ++m_ptr; return tmp;}

	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator+(difference_type diff) const noexcept -> ReverseContinousIterator<Cont, T> {auto tmp {*this}; return tmp += diff;}
	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator-(difference_type diff) const noexcept -> ReverseContinousIterator<Cont, T> {auto tmp {*this}; return tmp -= diff;}

	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator-(const ReverseContinousIterator<Cont, T> &iterator) const noexcept -> ReverseContinousIterator<Cont, T>::difference_type {
		return iterator.m_ptr - m_ptr;
	}

	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator*() const noexcept -> ReverseContinousIterator<Cont, T>::reference {return *m_ptr;}
	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator->() const noexcept -> ReverseContinousIterator<Cont, T>::pointer {return m_ptr;}
	template <typename Cont, typename T>
	constexpr auto ReverseContinousIterator<Cont, T>::operator[](difference_type diff) const noexcept -> ReverseContinousIterator<Cont, T>::reference {return m_ptr[-diff];}







	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr>::BasicTwoTypesContinousIterator() noexcept : m_container {nullptr}, m_isFirst {true}, m_ptr {} {m_ptr.first = nullptr;}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr>::BasicTwoTypesContinousIterator(Cont *container, pointer ptr) noexcept :
		m_container {container},
		m_isFirst {true},
		m_ptr {}
	{
		m_ptr.first = ptr;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr>::BasicTwoTypesContinousIterator(Cont *container, const Ptr &ptr) noexcept :
		m_container {container},
		m_isFirst {false},
		m_ptr {}
	{
		m_ptr.second = ptr;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr>::BasicTwoTypesContinousIterator(const BasicTwoTypesContinousIterator<Cont, T, Ptr> &iterator) noexcept :
		m_container {iterator.m_container},
		m_isFirst {iterator.m_isFirst},
		m_ptr {}
	{
		if (m_isFirst)
			m_ptr.first = iterator.m_ptr.first;
		else
			m_ptr.second = iterator.m_ptr.second;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr> &BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator=(const BasicTwoTypesContinousIterator<Cont, T, Ptr> &iterator) noexcept {
		m_container = iterator.m_container;
		m_isFirst = iterator.m_isFirst;
		if (m_isFirst)
			m_ptr.first = iterator.m_ptr.first;
		else
			m_ptr.second = iterator.m_ptr.second;
		return *this;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator bool() const noexcept {return m_container != nullptr;}
	template <typename Cont, typename T, typename Ptr>
	constexpr bool BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator==(const BasicTwoTypesContinousIterator<Cont, T, Ptr> &iterator) const noexcept {
		if (m_isFirst != iterator.m_isFirst)
			return false;
		if (m_isFirst)
			return m_container == iterator.m_container && m_ptr.first == iterator.m_ptr.first;
		return m_container == iterator.m_container && m_ptr.second == iterator.m_ptr.second;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr std::partial_ordering BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator<=>(const BasicTwoTypesContinousIterator<Cont, T, Ptr> &iterator) const noexcept {
		if (m_container == nullptr || iterator.m_container == nullptr)
			return std::partial_ordering::unordered;
		if (m_container != iterator.m_container)
			return std::partial_ordering::unordered;
		if (m_isFirst != iterator.m_isFirst)
			return std::partial_ordering::unordered;
		if (m_isFirst)
			return m_ptr.first <=> iterator.m_ptr.first;
		return m_ptr.second <=> iterator.m_ptr.second;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr> &BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator+=(difference_type diff) noexcept {
		if (m_isFirst)
			m_ptr.first += diff;
		else
			m_ptr.second += diff;
		return *this;
	}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr> &BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator-=(difference_type diff) noexcept {
		if (m_isFirst)
			m_ptr.first -= diff;
		else
			m_ptr.second -= diff;
		return *this;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr> &BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator++() noexcept {
		if (m_isFirst)
			++m_ptr.first;
		else
			++m_ptr.second;
		return *this;
	}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr> BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator++(int) noexcept {auto tmp {*this}; ++(*this); return tmp;}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr> &BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator--() noexcept {
		if (m_isFirst)
			--m_ptr.first;
		else
			--m_ptr.second;
		return *this;
	}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr> BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator--(int) noexcept {auto tmp {*this}; --(*this); return tmp;}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr> BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator+(difference_type diff) const noexcept {auto tmp {*this}; return tmp += diff;}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr> BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator-(difference_type diff) const noexcept {auto tmp {*this}; return tmp -= diff;}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr>::difference_type BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator-(
			const BasicTwoTypesContinousIterator<Cont, T, Ptr> &iterator
	) const noexcept {
		if (m_container != iterator.m_container || m_isFirst != iterator.m_isFirst)
			return static_cast<difference_type> (0);
		if (m_isFirst)
			return m_ptr.first - iterator.m_ptr.first;
		return m_ptr.second - iterator.m_ptr.second;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr>::reference BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator*() const noexcept {
		if (m_isFirst)
			return *m_ptr.first;
		return *m_ptr.second;
	}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr>::pointer BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator->() const noexcept {
		if (m_isFirst)
			return m_ptr.first;
		return &*m_ptr.second;
	}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicTwoTypesContinousIterator<Cont, T, Ptr>::reference BasicTwoTypesContinousIterator<Cont, T, Ptr>::operator[](difference_type diff) const noexcept {
		if (m_isFirst)
			return m_ptr.first[diff];
		return m_ptr.second[diff];
	}




	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::BasicReverseTwoTypesContinousIterator() noexcept : m_container {nullptr}, m_isFirst {true}, m_ptr {} {m_ptr.first = nullptr;}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::BasicReverseTwoTypesContinousIterator(Cont *container, pointer ptr) noexcept :
		m_container {container},
		m_isFirst {true},
		m_ptr {}
	{
		m_ptr.first = ptr;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::BasicReverseTwoTypesContinousIterator(Cont *container, const Ptr &ptr) noexcept :
		m_container {container},
		m_isFirst {false},
		m_ptr {}
	{
		m_ptr.second = ptr;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::BasicReverseTwoTypesContinousIterator(const BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &iterator) noexcept :
		m_container {iterator.m_container},
		m_isFirst {iterator.m_isFirst},
		m_ptr {}
	{
		if (m_isFirst)
			m_ptr.first = iterator.m_ptr.first;
		else
			m_ptr.second = iterator.m_ptr.second;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator=(const BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &iterator) noexcept {
		m_container = iterator.m_container;
		m_isFirst = iterator.m_isFirst;
		if (m_isFirst)
			m_ptr.first = iterator.m_ptr.first;
		else
			m_ptr.second = iterator.m_ptr.second;
		return *this;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator bool() const noexcept {return m_container != nullptr;}
	template <typename Cont, typename T, typename Ptr>
	constexpr bool BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator==(const BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &iterator) const noexcept {
		if (m_isFirst != iterator.m_isFirst)
			return false;
		if (m_isFirst)
			return m_container == iterator.m_container && m_ptr.first == iterator.m_ptr.first;
		return m_container == iterator.m_container && m_ptr.second == iterator.m_ptr.second;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr std::partial_ordering BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator<=>(const BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &iterator) const noexcept {
		if (m_container == nullptr || iterator.m_container == nullptr)
			return std::partial_ordering::unordered;
		if (m_container != iterator.m_container)
			return std::partial_ordering::unordered;
		if (m_isFirst != iterator.m_isFirst)
			return std::partial_ordering::unordered;
		if (m_isFirst)
			return iterator.m_ptr.first <=> m_ptr.first;
		return iterator.m_ptr.second <=> m_ptr.second;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator+=(difference_type diff) noexcept {
		if (m_isFirst)
			m_ptr.first -= diff;
		else
			m_ptr.second -= diff;
		return *this;
	}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator-=(difference_type diff) noexcept {
		if (m_isFirst)
			m_ptr.first += diff;
		else
			m_ptr.second += diff;
		return *this;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator++() noexcept {
		if (m_isFirst)
			--m_ptr.first;
		else
			--m_ptr.second;
		return *this;
	}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator++(int) noexcept {auto tmp {*this}; ++(*this); return tmp;}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator--() noexcept {
		if (m_isFirst)
			++m_ptr.first;
		else
			++m_ptr.second;
		return *this;
	}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator--(int) noexcept {auto tmp {*this}; --(*this); return tmp;}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator+(difference_type diff) const noexcept {
		auto tmp {*this}; return tmp += diff;
	}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator-(difference_type diff) const noexcept {
		auto tmp {*this}; return tmp -= diff;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::difference_type BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator-(
			const BasicReverseTwoTypesContinousIterator<Cont, T, Ptr> &iterator
	) const noexcept {
		if (m_container != iterator.m_container || m_isFirst != iterator.m_isFirst)
			return static_cast<difference_type> (0);
		if (m_isFirst)
			return iterator.m_ptr.first - m_ptr.first;
		return iterator.m_ptr.second - m_ptr.second;
	}

	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::reference BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator*() const noexcept {
		if (m_isFirst)
			return *m_ptr.first;
		return *m_ptr.second;
	}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::pointer BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator->() const noexcept {
		if (m_isFirst)
			return m_ptr.first;
		return &*m_ptr.second;
	}
	template <typename Cont, typename T, typename Ptr>
	constexpr BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::reference BasicReverseTwoTypesContinousIterator<Cont, T, Ptr>::operator[](difference_type diff) const noexcept {
		if (m_isFirst)
			return m_ptr.first[-diff];
		return m_ptr.second[-diff];
	}

} // namespace sl::utils
