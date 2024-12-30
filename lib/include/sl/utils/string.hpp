#pragma once

#include <cstddef>

#include "sl/core.hpp"
#include "sl/memory/allocator.hpp"
#include "sl/utils/iterator.hpp"
#include "sl/utils/numberWrapper.hpp"


namespace sl::utils {
	/**
	 * @brief A class that handles strings in Steelux
	 */
	template <typename CharT, sl::memory::IsAllocator Alloc = sl::memory::DefaultAllocator<CharT>>
	class SL_CORE BasicString final {
		public:
			using value_type = CharT;
			using allocator_type = Alloc;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using reference = value_type&;
			using const_reference = const value_type&;
			using pointer = std::allocator_traits<Alloc>::pointer;
			using const_pointer = std::allocator_traits<Alloc>::const_pointer;

			using iterator = sl::utils::TwoTypesContinousIterator<BasicString<CharT, Alloc>, CharT, pointer>;
			using const_iterator = sl::utils::TwoTypesContinousIterator<const BasicString<CharT, Alloc>, const CharT, const_pointer>;
			using reverse_iterator = sl::utils::ReverseTwoTypesContinousIterator<BasicString<CharT, Alloc>, CharT, pointer>;
			using const_reverse_iterator = sl::utils::ReverseTwoTypesContinousIterator<const BasicString<CharT, Alloc>, const CharT, const_pointer>;


			constexpr BasicString(const Alloc &alloc = Alloc()) noexcept;
			constexpr BasicString(const CharT *str, const Alloc &alloc = Alloc()) noexcept;
			constexpr BasicString(const CharT *str, size_type size, const Alloc &alloc = Alloc()) noexcept;
			constexpr ~BasicString();

			constexpr BasicString(const BasicString<CharT, Alloc> &str) noexcept;
			constexpr BasicString<CharT, Alloc> &operator=(const BasicString<CharT, Alloc> &str) noexcept;
			constexpr BasicString(BasicString<CharT, Alloc> &&str) noexcept;
			constexpr BasicString<CharT, Alloc> &operator=(BasicString<CharT, Alloc> &&str) noexcept;

			constexpr size_type reserve(size_type newSize) noexcept;
			constexpr size_type shrinkToFit() noexcept;

			constexpr iterator insert(difference_type position, CharT value, size_type count = 1) noexcept;
			constexpr iterator insert(const iterator &position, CharT value, size_type count = 1) noexcept {return this->insert(position - this->begin(), value, count);}
			constexpr iterator pushFront(CharT value, size_type count = 1) noexcept {return this->insert(this->begin(), value, count);}
			constexpr iterator pushBack(CharT value, size_type count = 1) noexcept {return this->insert(this->end(), value, count);}

			constexpr iterator erase(difference_type position, size_type count = 1) noexcept;
			constexpr iterator erase(const iterator &position, size_type count = 1) noexcept {return this->erase(position - this->begin(), count);}
			constexpr iterator popFront(size_type count = 1) noexcept {return this->erase(this->begin(), count);}
			constexpr iterator popBack(size_type count = 1) noexcept {return this->erase(this->end() - count, count);}

			constexpr iterator at(difference_type index) noexcept;
			constexpr const_iterator at(difference_type index) const noexcept;

			constexpr reference operator[](difference_type index) noexcept {return *this->at(index);}
			constexpr const_reference operator[](difference_type index) const noexcept {return *this->at(index);}

			constexpr iterator begin() noexcept;
			constexpr iterator end() noexcept;
			constexpr const_iterator cbegin() const noexcept;
			constexpr const_iterator cend() const noexcept;
			constexpr const_iterator begin() const noexcept {return this->cbegin();}
			constexpr const_iterator end() const noexcept {return this->cend();}
			constexpr reverse_iterator rbegin() noexcept;
			constexpr reverse_iterator rend() noexcept;
			constexpr const_reverse_iterator crbegin() const noexcept;
			constexpr const_reverse_iterator crend() const noexcept;
			constexpr const_reverse_iterator rbegin() const noexcept {return this->crbegin();}
			constexpr const_reverse_iterator rend() const noexcept {return this->crend();}

			constexpr bool isEmpty() const noexcept {return m_content.size == 0;}
			constexpr const CharT *getData() const noexcept;
			constexpr size_type getSize() const noexcept;
			constexpr size_type getCapacity() const noexcept;


		private:
			constexpr bool m_isSSO() const noexcept;
			constexpr pointer m_allocate(size_type size) const noexcept;
			constexpr void m_deallocate(pointer res, size_type size) const noexcept;
			constexpr difference_type m_normalizeIndex(difference_type index, size_type size = 0) const noexcept;

			// size does not include the null-terminating character
			template <typename Alloc2>
			struct Content {
				Alloc2 allocator;
				sl::utils::UnsignedIntFlagWrapper<size_type, 1> size;
			};

			template <sl::memory::IsAllocator Alloc2>
			requires (!sl::memory::IsAllocatorStatefull_v<Alloc2>)
			struct Content<Alloc2> {
				sl::utils::UnsignedIntFlagWrapper<size_type, 1> size;
			};

			Content<Alloc> m_content;

			struct Heap {
				pointer start;
				size_type capacity;
			};

			struct SSO {
				CharT buffer[sizeof(Heap) / sizeof(CharT)];
			};

			union {
				Heap m_heap;
				SSO m_sso;
			};

			static constexpr size_type MAX_SSO_CAPACITY {sizeof(m_heap) / sizeof(CharT)};
			static constexpr size_type MAX_SSO_SIZE {MAX_SSO_CAPACITY - 1};
	};


	static_assert(std::ranges::random_access_range<BasicString<char>>, "String type must fullfill std::ranges::random_access_range concept");

} // namespace sl::utils


#include "sl/utils/string.inl"


namespace sl {
	using String = sl::utils::BasicString<char>;
} // namespace sl
