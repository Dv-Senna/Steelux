#pragma once

#include "sl/utils/string.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>

#include "sl/utils/memory.hpp"


namespace sl::utils {
	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::BasicString(const Alloc &alloc) noexcept :
		m_content {s_createContent(alloc)},
		m_sso {}
	{
		if constexpr (sl::memory::IsAllocatorStatefull_v<Alloc>) {
			m_content.allocator.~Alloc();
			new(&m_content.allocator) Alloc(alloc);
		}
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
		if (this->m_isSSO()) {
			m_content.size = 0;
			(void)sl::utils::memset<CharT> (m_sso.buffer, static_cast<CharT> ('\0'), MAX_SSO_CAPACITY);
			return;
		}
		this->m_deallocate(m_heap.start, m_heap.capacity);
		m_content.size = 0;
		m_content.size.template setFlag<0> (false);
		m_heap.start = nullptr;
		m_heap.capacity = 0;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::BasicString(const BasicString<CharT, Alloc> &str) noexcept :
		BasicString<CharT, Alloc> (std::allocator_traits<Alloc>::select_on_container_copy_construction(str.m_copyAllocator()))
	{
		m_content.size = str.m_content.size;

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
		BasicString<CharT, Alloc> (std::move(str.m_copyAllocator()))
	{
		m_content.size = str.m_content.size;
		str.m_content.size = 0;
		str.m_content.size.template setFlag<0> (false);

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
	constexpr const auto &__BasicString_dereference(const sl::utils::BasicString<CharT, Alloc> *ptr) noexcept {return *ptr;}
	template <typename CharT>
	constexpr CharT *__BasicString_dereference(CharT *value) noexcept {return value;}

	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto __BasicString_begin(const sl::utils::BasicString<CharT, Alloc> &str) noexcept {return std::begin(str);}
	template <typename CharT>
	constexpr auto __BasicString_begin(CharT *value) noexcept {return value;}
	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto __BasicString_end(const sl::utils::BasicString<CharT, Alloc> &str) noexcept {return std::end(str);}
	template <typename CharT>
	constexpr auto __BasicString_end(CharT *value) noexcept {return value + sl::utils::getSize(value);}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	template <typename ...Types>
	requires std::same_as<Alloc, typename ConcatStringView<Types...>::Allocator>
	constexpr BasicString<CharT, Alloc>::BasicString(const ConcatStringView<Types...> &csv) noexcept :
		BasicString<CharT, Alloc> ()
	{
		size_type size {};
		std::apply([&size](auto &&...args) noexcept {
			size = (static_cast<size_type> (sl::utils::getSize(__BasicString_dereference(args))) + ...);
		}, csv.getTuple());

		this->reserve(size);

		std::apply([this](auto &&...args) noexcept {
			(this->pushBack(__BasicString_begin(__BasicString_dereference(args)), __BasicString_end(__BasicString_dereference(args))), ...);
		}, csv.getTuple());
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	template <typename ...Types>
	requires std::same_as<Alloc, typename ConcatStringView<Types...>::Allocator>
	constexpr BasicString<CharT, Alloc> &BasicString<CharT, Alloc>::operator=(const ConcatStringView<Types...> &csv) noexcept {
		this->~BasicString<CharT, Alloc> ();

		size_type size {};
		std::apply([&size](auto &&...args) noexcept {
			size = (static_cast<size_type> (sl::utils::getSize(__BasicString_dereference(args))) + ...);
		}, csv.getTuple());

		this->reserve(size);

		std::apply([this](auto &&...args) noexcept {
			(this->pushBack(__BasicString_begin(__BasicString_dereference(args)), __BasicString_end(__BasicString_dereference(args))), ...);
		}, csv.getTuple());

		return *this;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	template <sl::memory::IsAllocator Alloc2>
	constexpr auto BasicString<CharT, Alloc>::operator==(const BasicString<CharT, Alloc2> &str) const noexcept -> bool {
		if (m_content.size != str.getSize())
			return false;

		for (size_type i {0}; i < m_content.size; ++i) {
			if ((*this)[i] != str[i])
				return false;
		}
		return true;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto BasicString<CharT, Alloc>::operator==(const CharT *str) const noexcept -> bool {
		for (size_type i {0}; i < m_content.size; ++i) {
			if ((*this)[i] != str[i])
				return false;
		}
		return true;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	template <typename ...Types>
	constexpr auto BasicString<CharT, Alloc>::operator==(const ConcatStringView<Types...> &csv) const noexcept -> bool {
		return *this == BasicString<CharT, typename ConcatStringView<Types...>::Allocator> (csv);
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::size_type BasicString<CharT, Alloc>::reserve(size_type newSize) noexcept {
		size_type capacity {this->getCapacity()};
		if (newSize < capacity)
			return capacity - 1;
		
		size_type newCapacity {newSize + 1};
		pointer buffer {this->m_allocate(newCapacity)};
		if (this->m_isSSO())
			(void)sl::utils::memcpy<CharT> (buffer, m_sso.buffer, capacity);
		else {
			(void)sl::utils::memmove<CharT> (buffer, m_heap.start, newCapacity);
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
			(void)sl::utils::memcpy<CharT> (m_sso.buffer, m_heap.start, m_content.size + 1);
			this->m_deallocate(buffer, capacity);
			m_content.size.template setFlag<0> (false);
			return MAX_SSO_SIZE;
		}

		if (m_heap.capacity == m_content.size + 1)
			return m_content.size;

		pointer buffer {this->m_allocate(m_content.size + 1)};
		(void)sl::utils::memcpy<CharT> (buffer, m_heap.start, m_content.size + 1);
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
			(void)sl::utils::memmove<CharT> (m_sso.buffer + position + count, m_sso.buffer + position, m_content.size - position + 1);
			for (size_type i {0}; i < count; ++i)
				m_sso.buffer[position + i] = value;
			m_content.size += count;
			return this->begin() + position;
		}

		(void)sl::utils::memmove<CharT> (m_heap.start + position + count, m_heap.start + position, m_content.size - position + 1);
		for (size_type i {0}; i < count; ++i)
			m_heap.start[position + i] = value;
		m_content.size += count;
		return this->begin() + position;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	template <sl::memory::IsAllocator Alloc2>
	constexpr BasicString<CharT, Alloc>::iterator BasicString<CharT, Alloc>::insert(difference_type position, const sl::utils::BasicString<CharT, Alloc2> &str) noexcept {
		position = this->m_normalizeIndex(position, m_content.size + 1);
		(void)this->reserve(m_content.size + str.m_content.size);

		if (this->m_isSSO()) {
			(void)sl::utils::memmove<CharT> (m_sso.buffer + position + str.m_content.size, m_sso.buffer + position, m_content.size - position + 1);
			(void)sl::utils::memmove<CharT> (m_sso.buffer + position, str.m_sso.buffer, str.m_content.size);
			m_content.size += str.m_content.size;
			return this->begin() + position;
		}

		(void)sl::utils::memmove<CharT> (m_heap.start + position + str.m_content.size, m_heap.start + position, m_content.size - position + 1);
		if (str.m_isSSO())
			(void)sl::utils::memmove<CharT> (m_heap.start + position, str.m_sso.buffer, str.m_content.size);
		else
			(void)sl::utils::memmove<CharT> (m_heap.start + position, str.m_heap.start, str.m_content.size);

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
			(void)sl::utils::memmove<CharT> (m_sso.buffer + position + rangeSize, m_sso.buffer + position, m_content.size - position + 1);
			size_type i {0};
			for (IT it {start}; it != end; ++it) {
				(m_sso.buffer + position)[i] = static_cast<CharT> (*it);
				++i;
			}
			m_content.size += rangeSize;
			return this->begin() + position;
		}

		(void)sl::utils::memmove<CharT> (m_heap.start + position + rangeSize, m_heap.start + position, m_content.size - position + 1);
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
			(void)sl::utils::memmove<CharT> (m_sso.buffer + position, m_sso.buffer + position + count, m_content.size - position - count + 1);
		else
			(void)sl::utils::memmove<CharT> (m_heap.start + position, m_heap.start + position + count, m_content.size - position - count + 1);

		m_content.size -= count;
		return this->begin() + position;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::iterator BasicString<CharT, Alloc>::at(difference_type index) noexcept {
		index = this->m_normalizeIndex(index);
		return this->begin() + index;
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr BasicString<CharT, Alloc>::const_iterator BasicString<CharT, Alloc>::at(difference_type index) const noexcept {
		index = this->m_normalizeIndex(index);
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
	constexpr BasicString<CharT, Alloc>::pointer BasicString<CharT, Alloc>::m_allocate(size_type size) noexcept {
		if constexpr (sl::memory::IsAllocatorStatefull_v<Alloc>)
			return std::allocator_traits<Alloc>::allocate(m_content.allocator, size);
		else {
			Alloc allocator {};
			return std::allocator_traits<Alloc>::allocate(allocator, size);
		}
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr void BasicString<CharT, Alloc>::m_deallocate(pointer res, size_type size) noexcept {
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


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto BasicString<CharT, Alloc>::m_copyAllocator() const noexcept -> Alloc {
		if constexpr (sl::memory::IsAllocatorStatefull_v<Alloc>)
			return m_content.allocator;
		else
			return Alloc();
	}


	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto BasicString<CharT, Alloc>::s_createContent(const Alloc &alloc) noexcept -> Content<Alloc> {
		if constexpr (sl::memory::IsAllocatorStatefull_v<Alloc>)
			return Content<Alloc> {alloc, sl::utils::UnsignedIntFlagWrapper<size_type, 1> (0ULL)};
		else
			return Content<Alloc> {sl::utils::UnsignedIntFlagWrapper<size_type, 1> (0ULL)};
	}





	template <typename ...Types>
	template <typename LeftT, typename RightT>
	constexpr ConcatStringView<Types...>::ConcatStringView(const LeftT &lhs, const RightT &rhs) noexcept :
		m_strings {std::make_tuple(s_makePointer(lhs), s_makePointer(rhs))}
	{

	}


	template <typename ...Types>
	template <typename ...Types2, typename RightT>
	constexpr ConcatStringView<Types...>::ConcatStringView(const ConcatStringView<Types2...> &csv, const RightT &rhs) noexcept :
		m_strings {std::tuple_cat(csv.getTuple(), std::make_tuple(s_makePointer(rhs)))}
	{

	}


	template <typename ...Types>
	template <typename LeftT, typename ...Types2>
	constexpr ConcatStringView<Types...>::ConcatStringView(const LeftT &lhs, const ConcatStringView<Types2...> &csv) noexcept :
		m_strings {std::tuple_cat(std::make_tuple(s_makePointer(lhs)), csv.getTuple())}
	{

	}


	template <typename ...Types>
	template <typename ...LeftTs, typename ...RightTs>
	constexpr ConcatStringView<Types...>::ConcatStringView(const ConcatStringView<LeftTs...> &lhs, const ConcatStringView<RightTs...> &rhs) noexcept :
		m_strings {std::tuple_cat(lhs.getTuple(), rhs.getTuple())}
	{

	}


	template <typename ...Types>
	constexpr ConcatStringView<Types...>::ConcatStringView(ConcatStringView<Types...> &&csv) noexcept :
		m_strings {csv.m_strings}
	{

	}




	template <std::integral T, typename CharT, sl::memory::IsAllocator Alloc>
	constexpr std::optional<T> stringToNumber(const sl::utils::BasicString<CharT, Alloc> &string) noexcept {
		const sl::utils::BasicString<char> allowedDigits[17] {{}, {},
			"01", {}, {}, {}, {}, {},
			"01234567", {},
			"0123456789", {}, {}, {}, {}, {},
			"0123456789abcdef"
		};

		T result {0};
		if (string.isEmpty())
			return std::nullopt;
		bool isPositive {true};

		auto it {string.begin()};
		if (*it == '-') {
			if constexpr (std::unsigned_integral<T>)
				return std::nullopt;
			else {
				if (string.getSize() == 1)
					return std::nullopt;
				isPositive = false;
				++it;
			}
		}

		else if (*it == '+')
			++it;

		T base {10};
		if (*it == '0') {
			++it;
			if (*it == 'b') {
				base = 2;
				++it;
			}
			else if (*it == 'x') {
				base = 16;
				++it;
			}
			else if (*it == 'o') {
				base = 8;
				++it;
			}
		}

		const sl::utils::BasicString<char> &baseDigits {allowedDigits[base]};

		for (; it != string.end(); ++it) {
			auto digit {std::ranges::find(baseDigits, *it)};
			if (digit == baseDigits.end())
				return std::nullopt;
			T value {static_cast<T> (digit - baseDigits.begin())};
			const T position {static_cast<T> (string.end() - it - 1)};
			value *= std::pow(base, position);
			result += value;
		}

		return isPositive ? result : -result;
	}


	template <std::floating_point T, typename CharT, sl::memory::IsAllocator Alloc>
	constexpr std::optional<T> stringToNumber(const sl::utils::BasicString<CharT, Alloc> &string) noexcept {
		const sl::utils::BasicString<char> digits {"0123456789"};

		T result {0};
		T exponant {0};
		if (string.isEmpty())
			return std::nullopt;
		bool isPositive {true};
		auto it {string.begin()};
		if (*it == '-') {
			if (string.getSize() == 1)
				return std::nullopt;
			isPositive = false;
			++it;
		}

		if (std::ranges::count(string, '.') > 1)
			return std::nullopt;
		if (std::ranges::count(string, 'e') > 1)
			return std::nullopt;
		auto dotIt {std::ranges::find(string, '.')};
		auto sciIt {std::ranges::find(string, 'e')};
		if (dotIt != string.end() && dotIt > sciIt)
			return std::nullopt;

		bool exponentIsPositive {true};
		for (; it != string.end(); ++it) {
			if (it == dotIt || it == sciIt)
				continue;

			if (it == sciIt + 1) {
				if (*it == '-') {
					exponentIsPositive = false;
					continue;
				}
				if (*it == '+')
					continue;
			}

			auto digit {std::ranges::find(digits, *it)};
			if (digit == digits.end())
				return std::nullopt;
			T value {static_cast<T> (digit - digits.begin())};

			if (it > sciIt) {
				const T position {static_cast<T> (string.end() - it - 1)};
				value *= std::pow(static_cast<T> (10), position);
				exponant += value;
				continue;
			}

			T position {};
			if (it > dotIt)
				position = static_cast<T> (dotIt - it);
			else
				position = static_cast<T> (std::min(dotIt, sciIt) - it - 1);
			value *= std::pow(static_cast<T> (10), position);
			result += value;
		}

		result *= std::pow(static_cast<T> (10), exponentIsPositive ? exponant : -exponant);
		return isPositive ? result : -result;
	}

 } // namespace sl::utils

