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
	constexpr const CharT *BasicString<CharT, Alloc>::getData() const noexcept {
		if constexpr (!std::is_same_v<pointer, CharT*>)
			return nullptr;
		else {
			if (this->m_isSSO())
				return m_sso.buffer;
			return m_heap.start;
		}
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
