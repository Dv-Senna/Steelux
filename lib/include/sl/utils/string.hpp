#pragma once

#include <cstddef>
#include <cstring>

#include "sl/memory/allocator.hpp"
#include "sl/utils/iterator.hpp"
#include "sl/utils/numberWrapper.hpp"


namespace sl::utils {
	template <typename ...Types>
	class ConcatStringView;


	/**
	 * @brief A class that handles strings in Steelux
	 */
	template <typename CharT, sl::memory::IsAllocator Alloc = sl::memory::DefaultAllocator<CharT>>
	class BasicString final {
		template <typename T>
		static constexpr bool IsRange = std::ranges::range<T>
			&& !std::convertible_to<T, BasicString<CharT, Alloc>>
			&& !std::convertible_to<T, CharT*>
			&& !std::convertible_to<T, const CharT*>;

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

			template <typename ...Types>
			requires std::same_as<Alloc, typename ConcatStringView<Types...>::Allocator>
			constexpr BasicString(const ConcatStringView<Types...> &csv) noexcept;
			template <typename ...Types>
			requires std::same_as<Alloc, typename ConcatStringView<Types...>::Allocator>
			constexpr BasicString<CharT, Alloc> &operator=(const ConcatStringView<Types...> &csv) noexcept;

			constexpr size_type reserve(size_type newSize) noexcept;
			constexpr size_type shrinkToFit() noexcept;

			constexpr iterator insert(difference_type position, CharT value, size_type count = 1) noexcept;
			constexpr iterator insert(const iterator &position, CharT value, size_type count = 1) noexcept {return this->insert(position - this->begin(), value, count);}
			constexpr iterator insert(const iterator &start, const iterator &end, CharT value) noexcept {return this->insert(start - this->begin(), value, end - start);}
			constexpr iterator insert(const reverse_iterator &position, CharT value, size_type count = 1) noexcept {return this->insert(this->rbegin() - position - 1, value, count);}
			constexpr iterator insert(const reverse_iterator &start, const reverse_iterator &end, CharT value) noexcept {return this->insert(this->rbegin() - end - 1, value, end - start);}
			constexpr iterator pushFront(CharT value, size_type count = 1) noexcept {return this->insert(this->begin(), value, count);}
			constexpr iterator pushBack(CharT value, size_type count = 1) noexcept {return this->insert(this->end(), value, count);}

			template <sl::memory::IsAllocator Alloc2>
			constexpr iterator insert(difference_type position, const sl::utils::BasicString<CharT, Alloc2> &str) noexcept;
			template <sl::memory::IsAllocator Alloc2>
			constexpr iterator insert(const iterator &position, const sl::utils::BasicString<CharT, Alloc2> &str) noexcept {return this->insert(position - this->begin(), str);}
			template <sl::memory::IsAllocator Alloc2>
			constexpr iterator insert(const reverse_iterator &position, const sl::utils::BasicString<CharT, Alloc2> &str) noexcept {return this->insert(this->rbegin() - position - 1, str);}
			template <sl::memory::IsAllocator Alloc2>
			constexpr iterator pushFront(const sl::utils::BasicString<CharT, Alloc2> &str) noexcept {return this->insert(this->begin(), str);}
			template <sl::memory::IsAllocator Alloc2>
			constexpr iterator pushBack(const sl::utils::BasicString<CharT, Alloc2> &str) noexcept {return this->insert(this->end(), str);}

			constexpr iterator insert(difference_type position, const CharT *str) noexcept {return this->insert(position, BasicString(str));}
			constexpr iterator insert(const iterator &position, const CharT *str) noexcept {return this->insert(position, BasicString(str));}
			constexpr iterator insert(const reverse_iterator &position, const CharT *str) noexcept {return this->insert(position, BasicString(str));}
			constexpr iterator pushFront(const CharT *str) noexcept {return this->pushFront(BasicString(str));}
			constexpr iterator pushBack(const CharT *str) noexcept {return this->pushBack(BasicString(str));}

			template <sl::memory::IsAllocator Alloc2>
			constexpr BasicString<CharT, Alloc> &operator+=(const sl::utils::BasicString<CharT, Alloc2> &str) noexcept {(void)this->pushBack(str); return *this;}
			constexpr BasicString<CharT, Alloc> &operator+=(const CharT *str) noexcept {(void)this->pushBack(str); return *this;}

			template <std::forward_iterator IT>
			requires std::convertible_to<typename std::iterator_traits<IT>::value_type, CharT>
			constexpr iterator insert(difference_type position, const IT &start, const IT &end) noexcept;
			template <std::forward_iterator IT>
			constexpr iterator insert(const iterator &position, const IT &start, const IT &end) noexcept {return this->insert(position - this->begin(), start, end);}
			template <std::forward_iterator IT>
			constexpr iterator insert(const reverse_iterator &position, const IT &start, const IT &end) noexcept {return this->insert(this->rbegin() - position - 1, start, end);}
			template <std::forward_iterator IT>
			constexpr iterator pushFront(const IT &start, const IT &end) noexcept {return this->insert(this->begin(), start, end);}
			template <std::forward_iterator IT>
			constexpr iterator pushBack(const IT &start, const IT &end) noexcept {return this->insert(this->end(), start, end);}

			template <std::ranges::range Range>
			requires (IsRange<Range>)
			constexpr iterator insert(difference_type position, Range &&range) noexcept {return this->insert(position, std::ranges::begin(range), std::ranges::end(range));}
			template <std::ranges::range Range>
			requires (IsRange<Range>)
			constexpr iterator insert(const iterator &position, Range &&range) noexcept {return this->insert(position, std::ranges::begin(range), std::ranges::end(range));}
			template <std::ranges::range Range>
			requires (IsRange<Range>)
			constexpr iterator pushFront(Range &&range) noexcept {return this->pushFront(std::ranges::begin(range), std::ranges::end(range));}
			template <std::ranges::range Range>
			requires (IsRange<Range>)
			constexpr iterator pushBack(Range &&range) noexcept {return this->pushBack(std::ranges::begin(range), std::ranges::end(range));}

			constexpr iterator erase(difference_type position, size_type count = 1) noexcept;
			constexpr iterator erase(const iterator &position, size_type count = 1) noexcept {return this->erase(position - this->begin(), count);}
			constexpr iterator erase(const iterator &start, const iterator &end) noexcept {return this->erase(this->rbegin() - end, end - start);}
			constexpr iterator erase(const reverse_iterator &position, size_type count = 1) noexcept {return this->erase(this->rbegin() - position - 1, count);}
			constexpr iterator erase(const reverse_iterator &start, const reverse_iterator &end) noexcept {return this->erase(start - this->rbegin() - 1, end - start);}
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

	template <typename CharT>
	constexpr std::size_t getSize(const CharT *str) noexcept {return std::strlen(str);}
	template <typename CharT, typename Alloc>
	constexpr BasicString<CharT, Alloc>::size_type getSize(const BasicString<CharT, Alloc> &str) noexcept {return str.getSize();}


	template <typename ...Types>
	class ConcatStringView final {
		template <typename ...>
		using void_t = void;


		template <typename T>
		struct FirstStringFinder {
			using Type = void;
		};

		template <typename CharT, sl::memory::IsAllocator Alloc, typename ...Args>
		struct FirstStringFinder<std::tuple<sl::utils::BasicString<CharT, Alloc>, Args...>> {
			using Type = sl::utils::BasicString<CharT, Alloc>;
		};

		template <typename T, typename ...Args>
		struct FirstStringFinder<std::tuple<T, Args...>> {
			using Type = FirstStringFinder<std::tuple<Args...>>::Type;
		};

		
		template <typename T>
		struct RemoveArray {
			using Type = T;
		};

		template <typename T, std::size_t N>
		struct RemoveArray<T[N]> {
			using Type = T;
		};


		template <typename T>
		struct AddPointer {
			using Type = std::conditional_t<
				std::is_pointer_v<std::remove_reference_t<T>>,
				std::remove_reference_t<T>,
				std::add_pointer_t<std::add_const_t<std::conditional_t<
					std::is_array_v<std::remove_reference_t<T>>,
					typename RemoveArray<std::remove_reference_t<T>>::Type,
					std::remove_reference_t<T>
				>>>
			>;
		};


		public:
			using Tuple = std::tuple<Types...>;
			using AddressTuple = std::tuple<typename AddPointer<Types>::Type...>;
			using FirstString = FirstStringFinder<Tuple>::Type;
			using CharT = typename FirstString::value_type;
			using Allocator = typename FirstString::allocator_type;


			template <typename LeftT, typename RightT>
			constexpr ConcatStringView(const LeftT &lhs, const RightT &rhs) noexcept;
			template <typename ...Types2, typename RightT>
			constexpr ConcatStringView(const ConcatStringView<Types2...> &csv, const RightT &rhs) noexcept;
			template <typename LeftT, typename ...Types2>
			constexpr ConcatStringView(const LeftT &lhs, const ConcatStringView<Types2...> &csv) noexcept;
			template <typename ...LeftTs, typename ...RightTs>
			constexpr ConcatStringView(const ConcatStringView<LeftTs...> &lhs, const ConcatStringView<RightTs...> &rhs) noexcept;

			constexpr ConcatStringView(ConcatStringView<Types...> &&csv) noexcept;

			template <typename ...Types2>
			constexpr auto operator+(const ConcatStringView<Types2...> &csv) const noexcept {
				return ConcatStringView<Types..., Types2...> (*this, csv);
			}

			constexpr operator FirstString() const noexcept {return FirstString(*this);}
			constexpr const AddressTuple &getTuple() const noexcept {return m_strings;}

		private:
			template <typename T>
			constexpr const T *s_makePointer(const T &value) noexcept {return &value;}
/*			template <typename T, std::size_t N>
			constexpr const T *s_makePointer(const T (&value)[N]) noexcept {return *&value;}*/
			template <typename T>
			constexpr const typename RemoveArray<T>::Type *s_makePointer(T *ptr) noexcept {return ptr;}

			AddressTuple m_strings;
	};

	template <typename ...Types, typename T>
	constexpr auto operator+(const ConcatStringView<Types...> &csv, const T &rhs) noexcept {
		return ConcatStringView<Types..., T> (csv, rhs);
	}

	template <typename ...Types, typename T>
	constexpr auto operator+(const T &lhs, const ConcatStringView<Types...> &csv) noexcept {
		return ConcatStringView<T, Types...> (lhs, csv);
	}

	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto operator+(const sl::utils::BasicString<CharT, Alloc> &str, const CharT *rhs) noexcept {
		return ConcatStringView<sl::utils::BasicString<CharT, Alloc>, const CharT *> (str, rhs);
	}

	template <typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto operator+(const CharT *lhs, const sl::utils::BasicString<CharT, Alloc> &str) noexcept {
		return ConcatStringView<const CharT *, sl::utils::BasicString<CharT, Alloc>> (lhs, str);
	}

	template <typename CharT, sl::memory::IsAllocator Alloc, sl::memory::IsAllocator Alloc2>
	constexpr auto operator+(const sl::utils::BasicString<CharT, Alloc> &lhs, const sl::utils::BasicString<CharT, Alloc2> &rhs) noexcept {
		return ConcatStringView<sl::utils::BasicString<CharT, Alloc>, sl::utils::BasicString<CharT, Alloc2>> (lhs, rhs);
	}

} // namespace sl::utils


#include "sl/utils/string.inl"


namespace sl {
	using String = sl::utils::BasicString<char>;
} // namespace sl
