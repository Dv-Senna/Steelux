#pragma once

#include "sl/utils/string.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "sl/utils/memory.hpp"


namespace sl::utils {
	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::BasicString(const Alloc &alloc) noexcept :
		m_content {},
		m_sso {}
	{
		if constexpr (sl::memory::IsAllocatorStatefull_v<Alloc>)
			m_content.allocator = alloc;
		sl::utils::memset<CharT> (m_sso.buffer, static_cast<CharT> ('\0'), MAX_SSO_CAPACITY);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::BasicString(const CharT *str, const Alloc &alloc) noexcept :
		BasicString<CharT, Alloc> (alloc)
	{
		for (const CharT *current {str}; *current != static_cast<CharT> ('\0'); ++current) ++m_content.size;
		if (m_content.size <= MAX_SSO_SIZE) {
			(void)sl::utils::memcpy<CharT> (m_sso.buffer, str, m_content.size + 1);
			return;
		}

		m_content.size.template setFlag<0> (true);
		m_heap.capacity = m_content.size + 1;
		m_heap.start = this->m_allocate(m_heap.capacity);
		(void)sl::utils::memcpy<CharT> (m_heap.start, str, m_heap.capacity);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::BasicString(const CharT *str, size_type size, const Alloc &alloc) noexcept :
		BasicString<CharT, Alloc> (alloc)
	{
		m_content.size = size;
		if (m_content.size <= MAX_SSO_SIZE) {
			(void)sl::utils::memcpy<CharT> (m_sso.buffer, str, m_content.size);
			m_sso.buffer[m_content.size] = static_cast<CharT> ('\0');
			return;
		}

		m_content.size.template setFlag<0> (true);
		m_heap.capacity = m_content.size + 1;
		m_heap.start = this->m_allocate(m_heap.capacity);
		(void)sl::utils::memcpy<CharT> (m_heap.start, str, m_content.size);
		m_heap.start[m_content.size] = static_cast<CharT> ('\0');
	}


	template<typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::~BasicString() {
		if (this->m_isSSO())
			return;
		this->m_deallocate(m_heap.start, m_heap.capacity);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::BasicString(const BasicString<CharT, Alloc> &str) noexcept :
		BasicString<CharT, Alloc> ()
	{
		m_content.size = str.m_content.size;
		if constexpr (sl::memory::IsAllocatorStatefull_v<Alloc>)
			m_content.allocator = std::allocator_traits<Alloc>::select_on_container_copy_construction(str.m_content.allocator);

		if (this->m_isSSO()) {
			(void)sl::utils::memcpy<CharT> (m_sso.buffer, str.m_sso.buffer, MAX_SSO_CAPACITY);
			return;
		}

		m_heap.capacity = m_content.size + 1;
		m_heap.start = this->m_allocate(m_heap.capacity);
		(void)sl::utils::memcpy<CharT> (m_heap.start, str.m_heap.start, m_heap.capacity);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc> &BasicString<CharT, Alloc>::operator=(const BasicString<CharT, Alloc> &str) noexcept {
		this->~BasicString<CharT, Alloc> ();

		m_content.size = str.m_content.size;
		if constexpr (sl::memory::IsAllocatorStatefull_v<Alloc>)
			m_content.allocator = std::allocator_traits<Alloc>::select_on_container_copy_construction(str.m_content.allocator);

		if (this->m_isSSO()) {
			(void)sl::utils::memcpy<CharT> (m_sso.buffer, str.m_sso.buffer, MAX_SSO_CAPACITY);
			return *this;
		}

		m_heap.capacity = m_content.size + 1;
		m_heap.start = this->m_allocate(m_heap.capacity);
		(void)sl::utils::memcpy<CharT> (m_heap.start, str.m_heap.start, m_heap.capacity);
		return *this;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::BasicString(BasicString<CharT, Alloc> &&str) noexcept :
		BasicString<CharT, Alloc> ()
	{
		m_content.size = str.m_content.size;
		str.m_content.size = 0;
		str.m_content.size.template setFlag<0> (false);
		if constexpr (sl::memory::IsAllocatorStatefull_v<Alloc>)
			m_content.allocator = std::move(str.m_content.allocator);

		if (this->m_isSSO())
			(void)sl::utils::memcpy<CharT> (m_sso.buffer, str.m_sso.buffer, MAX_SSO_CAPACITY);
		else {
			m_heap.capacity = str.m_heap.capacity;
			m_heap.start = str.m_heap.start;
		}

		(void)sl::utils::memset<CharT> (str.m_sso.buffer, static_cast<CharT> ('\0'), MAX_SSO_CAPACITY);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc> &BasicString<CharT, Alloc>::operator=(BasicString<CharT, Alloc> &&str) noexcept {
		this->~BasicString<CharT, Alloc> ();

		m_content.size = str.m_content.size;
		str.m_content.size = 0;
		str.m_content.size.template setFlag<0> (false);
		if constexpr (sl::memory::IsAllocatorStatefull_v<Alloc>)
			m_content.allocator = std::move(str.m_content.allocator);

		if (this->m_isSSO())
			(void)sl::utils::memcpy<CharT> (m_sso.buffer, str.m_sso.buffer, MAX_SSO_CAPACITY);
		else {
			m_heap.capacity = str.m_heap.capacity;
			m_heap.start = str.m_heap.start;
		}

		(void)sl::utils::memset<CharT> (str.m_sso.buffer, static_cast<CharT> ('\0'), MAX_SSO_CAPACITY);
		return *this;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::iterator BasicString<CharT, Alloc>::at(difference_type index) noexcept {
		index %= static_cast<difference_type> (m_content.size);
		if (index < 0)
			index += static_cast<difference_type> (m_content.size);
		if (this->m_isSSO())
			return this->begin() + index;
		return this->begin() + index;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::const_iterator BasicString<CharT, Alloc>::at(difference_type index) const noexcept {
		index %= static_cast<difference_type> (m_content.size);
		if (index < 0)
			index += static_cast<difference_type> (m_content.size);
		if (this->m_isSSO())
			return this->begin() + index;
		return this->begin() + index;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::iterator BasicString<CharT, Alloc>::begin() noexcept {
		if (this->m_isSSO())
			return iterator(this, this->m_sso.buffer);
		return iterator(this, this->m_heap.start);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::iterator BasicString<CharT, Alloc>::end() noexcept {
		if (this->m_isSSO())
			return iterator(this, this->m_sso.buffer + m_content.size);
		return iterator(this, this->m_heap.start + m_content.size);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::const_iterator BasicString<CharT, Alloc>::cbegin() const noexcept {
		if (this->m_isSSO())
			return const_iterator(this, this->m_sso.buffer);
		return const_iterator(this, this->m_heap.start);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::const_iterator BasicString<CharT, Alloc>::cend() const noexcept {
		if (this->m_isSSO())
			return const_iterator(this, this->m_sso.buffer + m_content.size);
		return const_iterator(this, this->m_heap.start + m_content.size);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::reverse_iterator BasicString<CharT, Alloc>::rbegin() noexcept {
		if (this->m_isSSO())
			return reverse_iterator(this, this->m_sso.buffer + m_content.size - 1);
		return reverse_iterator(this, this->m_heap.start + m_content.size - 1);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::reverse_iterator BasicString<CharT, Alloc>::rend() noexcept {
		if (this->m_isSSO())
			return reverse_iterator(this, this->m_sso.buffer - 1);
		return reverse_iterator(this, this->m_heap.start - 1);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::const_reverse_iterator BasicString<CharT, Alloc>::crbegin() const noexcept {
		if (this->m_isSSO())
			return const_reverse_iterator(this, this->m_sso.buffer + m_content.size - 1);
		return const_reverse_iterator(this, this->m_heap.start + m_content.size - 1);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::const_reverse_iterator BasicString<CharT, Alloc>::crend() const noexcept {
		if (this->m_isSSO())
			return const_reverse_iterator(this, this->m_sso.buffer - 1);
		return const_reverse_iterator(this, this->m_heap.start - 1);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr const CharT *BasicString<CharT, Alloc>::getData() const noexcept {
		if (this->m_isSSO())
			return m_sso.buffer;
		return &*m_heap.start;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::size_type BasicString<CharT, Alloc>::getSize() const noexcept {
		return m_content.size;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::size_type BasicString<CharT, Alloc>::getCapacity() const noexcept {
		if (this->m_isSSO())
			return MAX_SSO_CAPACITY;
		return m_heap.capacity;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr bool BasicString<CharT, Alloc>::m_isSSO() const noexcept {
		return !m_content.size.template getFlag<0> ();
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::pointer BasicString<CharT, Alloc>::m_allocate(size_type size) const noexcept {
		if constexpr (sl::memory::IsAllocatorStatefull_v<Alloc>)
			return std::allocator_traits<Alloc>::allocate(m_content.allocator, size);
		else {
			Alloc allocator {};
			return std::allocator_traits<Alloc>::allocate(allocator, size);
		}
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr void BasicString<CharT, Alloc>::m_deallocate(pointer res, size_type size) const noexcept {
		if constexpr (sl::memory::IsAllocatorStatefull_v<Alloc>)
			std::allocator_traits<Alloc>::deallocate(m_content.allocator, res, size);
		else {
			Alloc allocator {};
			std::allocator_traits<Alloc>::deallocate(allocator, res, size);
		}
	}


} // namespace sl::utils
