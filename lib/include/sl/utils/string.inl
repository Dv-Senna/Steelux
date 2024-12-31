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
		(void)sl::utils::memcpy<CharT> (&*m_heap.start, str, m_heap.capacity);
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
		(void)sl::utils::memcpy<CharT> (&*m_heap.start, str, m_content.size);
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
		(void)sl::utils::memcpy<CharT> (&*m_heap.start, &*str.m_heap.start, m_heap.capacity);
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
		(void)sl::utils::memcpy<CharT> (&*m_heap.start, &*str.m_heap.start, m_heap.capacity);
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
	constexpr BasicString<CharT, Alloc>::size_type BasicString<CharT, Alloc>::reserve(size_type newSize) noexcept {
		size_type capacity {this->getCapacity()};
		if (newSize < capacity)
			return capacity - 1;
		
		size_type newCapacity {newSize + 1};
		pointer buffer {this->m_allocate(newCapacity)};
		if (this->m_isSSO())
			(void)sl::utils::memcpy(buffer, m_sso.buffer, capacity);
		else {
			(void)sl::utils::memmove(buffer, m_heap.start, newCapacity);
			this->m_deallocate(m_heap.start, m_heap.capacity);
		}

		m_heap.capacity = newCapacity;
		m_heap.start = buffer;
		m_content.size.template setFlag<0> (true);
		return newCapacity - 1;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::size_type BasicString<CharT, Alloc>::shrinkToFit() noexcept {
		if (m_content.size <= MAX_SSO_SIZE) {
			if (this->m_isSSO())
				return MAX_SSO_SIZE;

			pointer buffer {m_heap.start};
			size_type capacity {m_heap.capacity};
			(void)std::memcpy(m_sso.buffer, m_heap.start, m_content.size + 1);
			this->m_deallocate(buffer, capacity);
			m_content.size.template setFlag<0> (false);
			return MAX_SSO_SIZE;
		}

		if (m_heap.capacity == m_content.size + 1)
			return m_content.size;

		pointer buffer {this->m_allocate(m_content.size + 1)};
		(void)std::memcpy(buffer, m_heap.start, m_content.size + 1);
		this->m_deallocate(m_heap.start, m_heap.capacity);
		m_heap.start = buffer;
		m_heap.capacity = m_content.size + 1;
		return m_content.size;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::iterator BasicString<CharT, Alloc>::insert(difference_type position, CharT value, size_type count) noexcept {
		position = this->m_normalizeIndex(position, m_content.size + 1);
		size_type targetCapacity {m_content.size + count + 1};
		if (this->getCapacity() < targetCapacity) {
			constexpr long double CAPACITY_INCREASE_FACTOR {1.5};
			size_type newCapacity {this->getCapacity()};
			while (newCapacity < targetCapacity)
				newCapacity *= CAPACITY_INCREASE_FACTOR;
			(void)this->reserve(newCapacity - 1);
		}

		if (this->m_isSSO()) {
			(void)sl::utils::memmove(m_sso.buffer + position + count, m_sso.buffer + position, m_content.size - position + 1);
			for (size_type i {0}; i < count; ++i)
				m_sso.buffer[position + i] = value;
			m_content.size += count;
			return this->begin() + position;
		}

		(void)sl::utils::memmove(m_heap.start + position + count, m_heap.start + position, m_content.size - position + 1);
		for (size_type i {0}; i < count; ++i)
			m_heap.start[position + i] = value;
		m_content.size += count;
		return this->begin() + position;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::iterator BasicString<CharT, Alloc>::insert(difference_type position, const sl::utils::BasicString<CharT, Alloc> &str) noexcept {
		position = this->m_normalizeIndex(position, m_content.size + 1);
		(void)this->reserve(m_content.size + str.m_content.size);

		if (this->m_isSSO()) {
			(void)sl::utils::memmove(m_sso.buffer + position + str.m_content.size, m_sso.buffer + position, m_content.size - position + 1);
			(void)sl::utils::memmove(m_sso.buffer + position, str.m_sso.buffer, str.m_content.size);
			m_content.size += str.m_content.size;
			return this->begin() + position;
		}

		(void)sl::utils::memmove(m_heap.start + position + str.m_content.size, m_heap.start + position, m_content.size - position + 1);
		if (str.m_isSSO())
			(void)sl::utils::memmove(m_heap.start + position, str.m_sso.buffer, str.m_content.size);
		else
			(void)sl::utils::memmove(m_heap.start + position, str.m_heap.start, str.m_content.size);

		m_content.size += str.m_content.size;
		return this->begin() + position;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	template <std::forward_iterator IT>
	requires std::convertible_to<typename std::iterator_traits<IT>::value_type, CharT>
	constexpr BasicString<CharT, Alloc>::iterator BasicString<CharT, Alloc>::insert(difference_type position, const IT &start, const IT &end) noexcept {
		size_type rangeSize {static_cast<size_type> (std::distance(start, end))};
		position = this->m_normalizeIndex(position, m_content.size + 1);
		(void)this->reserve(m_content.size + rangeSize);

		if (this->m_isSSO()) {
			(void)sl::utils::memmove(m_sso.buffer + position + rangeSize, m_sso.buffer + position, m_content.size - position + 1);
			size_type i {0};
			for (IT it {start}; it != end; ++it) {
				(m_sso.buffer + position)[i] = static_cast<CharT> (*it);
				++i;
			}
			m_content.size += rangeSize;
			return this->begin() + position;
		}

		(void)sl::utils::memmove(m_heap.start + position + rangeSize, m_heap.start + position, m_content.size - position + 1);
		size_type i {0};
		for (IT it {start}; it != end; ++it) {
			(m_heap.start + position)[i] = static_cast<CharT> (*it);
			++i;
		}
		m_content.size += rangeSize;
		return this->begin() + position;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::iterator BasicString<CharT, Alloc>::erase(difference_type position, size_type count) noexcept {
		position = this->m_normalizeIndex(position);
		if (count > m_content.size)
			count = m_content.size;

		if (this->m_isSSO())
			(void)sl::utils::memmove(m_sso.buffer + position, m_sso.buffer + position + count, m_content.size - position - count + 1);
		else
			(void)sl::utils::memmove(m_heap.start + position, m_heap.start + position + count, m_content.size - position - count + 1);

		m_content.size -= count;
		return this->begin() + position;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::iterator BasicString<CharT, Alloc>::at(difference_type index) noexcept {
		index = this->m_normalizeIndex(index);
		if (this->m_isSSO())
			return this->begin() + index;
		return this->begin() + index;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::const_iterator BasicString<CharT, Alloc>::at(difference_type index) const noexcept {
		index = this->m_normalizeIndex(index);
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


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::difference_type BasicString<CharT, Alloc>::m_normalizeIndex(difference_type index, size_type size) const noexcept {
		if (size == 0)
			size = m_content.size;
		index %= static_cast<difference_type> (size);
		if (index < 0)
			index += static_cast<difference_type> (size);
		return index;
	}






	template <typename LeftT, typename RightT>
	ConcatStringView<LeftT, RightT>::ConcatStringView(const LeftT &left, const RightT &right) noexcept :
		m_strings {s_concatenateTuples(left, right)}
	{

	}


	template <typename LeftT, typename RightT>
	ConcatStringView<LeftT, RightT>::AddressTuple ConcatStringView<LeftT, RightT>::s_concatenateTuples(const LeftT &left, const RightT &right) noexcept {
		if constexpr (std::is_same_v<typename GetTupleIfAny<LeftT>::Type, LeftT> && std::is_same_v<typename GetTupleIfAny<RightT>::Type, RightT>)
			return s_concatenateTuplesInternal(left, right);
		else if constexpr (std::is_same_v<typename GetTupleIfAny<LeftT>::Type, LeftT>)
			return s_concatenateTuplesInternal(left, right.getTuple());
		else if constexpr (std::is_same_v<typename GetTupleIfAny<RightT>::Type, RightT>)
			return s_concatenateTuplesInternal(left.getTuple(), right);
		else
			return s_concatenateTuplesInternal(left.getTuple(), right.getTuple());
	}


	template <typename LeftT, typename RightT>
	template <typename LeftT2, typename RightT2>
	ConcatStringView<LeftT, RightT>::AddressTuple ConcatStringView<LeftT, RightT>::s_concatenateTuplesInternal(const LeftT2 &left, const RightT2 &right) noexcept {
		if constexpr (std::is_same_v<typename AddPointerToType<LeftT2>::Type, LeftT2> && std::is_same_v<typename AddPointerToType<RightT2>::Type, RightT2>)
			return std::make_tuple(left, right);
		else if constexpr (std::is_same_v<typename AddPointerToType<LeftT2>::Type, LeftT2>)
			return std::make_tuple(left, &right);
		else if constexpr (std::is_same_v<typename AddPointerToType<RightT2>::Type, RightT2>)
			return std::make_tuple(&left, right);
		else
			return std::make_tuple(&left, &right);
	}


	template <typename LeftT, typename RightT>
	template <typename ...LeftArgs, typename RightT2>
	ConcatStringView<LeftT, RightT>::AddressTuple ConcatStringView<LeftT, RightT>::s_concatenateTuplesInternal(const std::tuple<LeftArgs...> &left, const RightT2 &right) noexcept {
		if constexpr (std::is_same_v<typename AddPointerToType<RightT2>::Type, RightT2>)
			return std::tuple_cat(left, std::make_tuple(right));
		else
			return std::tuple_cat(left, std::make_tuple(&right));
	}


	template <typename LeftT, typename RightT>
	template <typename LeftT2, typename ...RightArgs>
	ConcatStringView<LeftT, RightT>::AddressTuple ConcatStringView<LeftT, RightT>::s_concatenateTuplesInternal(const LeftT2 &left, const std::tuple<RightArgs...> &right) noexcept {
		if constexpr (std::is_same_v<typename AddPointerToType<LeftT2>::Type, LeftT2>)
			return std::tuple_cat(std::make_tuple(left), right);
		else
			return std::tuple_cat(std::make_tuple(&left), right);
	}


	template <typename LeftT, typename RightT>
	template <typename ...LeftArgs, typename ...RightArgs>
	ConcatStringView<LeftT, RightT>::AddressTuple ConcatStringView<LeftT, RightT>::s_concatenateTuplesInternal(
		const std::tuple<LeftArgs...> &left, const std::tuple<RightArgs...> &right
	) noexcept {
		return std::tuple_cat(left, right);
	}

} // namespace sl::utils
