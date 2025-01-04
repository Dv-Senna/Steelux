#pragma once

#include <cstddef>
#include <cstring>
#include <optional>

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
			constexpr auto operator=(const BasicString<CharT, Alloc> &str) noexcept -> BasicString<CharT, Alloc>&;
			constexpr BasicString(BasicString<CharT, Alloc> &&str) noexcept;
			constexpr auto operator=(BasicString<CharT, Alloc> &&str) noexcept -> BasicString<CharT, Alloc>&;

			template <typename ...Types>
			requires std::same_as<Alloc, typename ConcatStringView<Types...>::Allocator>
			constexpr BasicString(const ConcatStringView<Types...> &csv) noexcept;
			template <typename ...Types>
			requires std::same_as<Alloc, typename ConcatStringView<Types...>::Allocator>
			constexpr auto operator=(const ConcatStringView<Types...> &csv) noexcept -> BasicString<CharT, Alloc>&;

			template <sl::memory::IsAllocator Alloc2>
			constexpr auto operator==(const BasicString<CharT, Alloc2> &str) const noexcept -> bool;
			constexpr auto operator==(const CharT *str) const noexcept -> bool;
			template <typename ...Types>
			constexpr auto operator==(const ConcatStringView<Types...> &csv) const noexcept -> bool;

			constexpr auto reserve(size_type newSize) noexcept -> size_type;
			constexpr auto shrinkToFit() noexcept -> size_type;

			constexpr auto insert(difference_type position, CharT value, size_type count = 1) noexcept -> iterator;
			constexpr auto insert(const iterator &position, CharT value, size_type count = 1) noexcept -> iterator {return this->insert(position - this->begin(), value, count);}
			constexpr auto insert(const iterator &start, const iterator &end, CharT value) noexcept -> iterator {return this->insert(start - this->begin(), value, end - start);}
			constexpr auto insert(const reverse_iterator &position, CharT value, size_type count = 1) noexcept -> iterator {
				return this->insert(this->rbegin() - position - 1, value, count);
			}
			constexpr auto insert(const reverse_iterator &start, const reverse_iterator &end, CharT value) noexcept -> iterator {
				return this->insert(this->rbegin() - end - 1, value, end - start);
			}
			constexpr auto pushFront(CharT value, size_type count = 1) noexcept -> iterator {return this->insert(this->begin(), value, count);}
			constexpr auto pushBack(CharT value, size_type count = 1) noexcept -> iterator {return this->insert(this->end(), value, count);}

			template <sl::memory::IsAllocator Alloc2>
			constexpr auto insert(difference_type position, const sl::utils::BasicString<CharT, Alloc2> &str) noexcept -> iterator;
			template <sl::memory::IsAllocator Alloc2>
			constexpr auto insert(const iterator &position, const sl::utils::BasicString<CharT, Alloc2> &str) noexcept -> iterator {return this->insert(position - this->begin(), str);}
			template <sl::memory::IsAllocator Alloc2>
			constexpr auto insert(const reverse_iterator &position, const sl::utils::BasicString<CharT, Alloc2> &str) noexcept -> iterator {
				return this->insert(this->rbegin() - position - 1, str);
			}
			template <sl::memory::IsAllocator Alloc2>
			constexpr auto pushFront(const sl::utils::BasicString<CharT, Alloc2> &str) noexcept -> iterator {return this->insert(this->begin(), str);}
			template <sl::memory::IsAllocator Alloc2>
			constexpr auto pushBack(const sl::utils::BasicString<CharT, Alloc2> &str) noexcept -> iterator {return this->insert(this->end(), str);}

			constexpr auto insert(difference_type position, const CharT *str) noexcept -> iterator {return this->insert(position, BasicString(str, this->m_copyAllocator()));}
			constexpr auto insert(const iterator &position, const CharT *str) noexcept -> iterator {return this->insert(position, BasicString(str, this->m_copyAllocator()));}
			constexpr auto insert(const reverse_iterator &position, const CharT *str) noexcept -> iterator {return this->insert(position, BasicString(str, this->m_copyAllocator()));}
			constexpr auto pushFront(const CharT *str) noexcept -> iterator {return this->pushFront(BasicString(str, this->m_copyAllocator()));}
			constexpr auto pushBack(const CharT *str) noexcept -> iterator {return this->pushBack(BasicString(str, this->m_copyAllocator()));}

			template <sl::memory::IsAllocator Alloc2>
			constexpr auto operator+=(const sl::utils::BasicString<CharT, Alloc2> &str) noexcept -> BasicString<CharT, Alloc>& {(void)this->pushBack(str); return *this;}
			constexpr auto operator+=(const CharT *str) noexcept -> BasicString<CharT, Alloc>& {(void)this->pushBack(str); return *this;}

			template <std::forward_iterator IT>
			requires std::convertible_to<typename std::iterator_traits<IT>::value_type, CharT>
			constexpr auto insert(difference_type position, const IT &start, const IT &end) noexcept -> iterator;
			template <std::forward_iterator IT>
			constexpr auto insert(const iterator &position, const IT &start, const IT &end) noexcept -> iterator {return this->insert(position - this->begin(), start, end);}
			template <std::forward_iterator IT>
			constexpr auto insert(const reverse_iterator &position, const IT &start, const IT &end) noexcept -> iterator {return this->insert(this->rbegin() - position - 1, start, end);}
			template <std::forward_iterator IT>
			constexpr auto pushFront(const IT &start, const IT &end) noexcept -> iterator {return this->insert(this->begin(), start, end);}
			template <std::forward_iterator IT>
			constexpr auto pushBack(const IT &start, const IT &end) noexcept -> iterator {return this->insert(this->end(), start, end);}

			template <std::ranges::range Range>
			requires (IsRange<Range>)
			constexpr auto insert(difference_type position, Range &&range) noexcept -> iterator {return this->insert(position, std::ranges::begin(range), std::ranges::end(range));}
			template <std::ranges::range Range>
			requires (IsRange<Range>)
			constexpr auto insert(const iterator &position, Range &&range) noexcept -> iterator {return this->insert(position, std::ranges::begin(range), std::ranges::end(range));}
			template <std::ranges::range Range>
			requires (IsRange<Range>)
			constexpr auto pushFront(Range &&range) noexcept -> iterator {return this->pushFront(std::ranges::begin(range), std::ranges::end(range));}
			template <std::ranges::range Range>
			requires (IsRange<Range>)
			constexpr auto pushBack(Range &&range) noexcept -> iterator {return this->pushBack(std::ranges::begin(range), std::ranges::end(range));}

			constexpr auto erase(difference_type position, size_type count = 1) noexcept -> iterator;
			constexpr auto erase(const iterator &position, size_type count = 1) noexcept -> iterator {return this->erase(position - this->begin(), count);}
			constexpr auto erase(const iterator &start, const iterator &end) noexcept -> iterator {return this->erase(this->rbegin() - end, end - start);}
			constexpr auto erase(const reverse_iterator &position, size_type count = 1) noexcept -> iterator {return this->erase(this->rbegin() - position - 1, count);}
			constexpr auto erase(const reverse_iterator &start, const reverse_iterator &end) noexcept -> iterator {return this->erase(start - this->rbegin() - 1, end - start);}
			constexpr auto popFront(size_type count = 1) noexcept -> iterator {return this->erase(this->begin(), count);}
			constexpr auto popBack(size_type count = 1) noexcept -> iterator {return this->erase(this->end() - count, count);}

			constexpr auto at(difference_type index) noexcept -> iterator;
			constexpr auto at(difference_type index) const noexcept -> const_iterator;

			constexpr auto operator[](difference_type index) noexcept -> reference {return *this->at(index);}
			constexpr auto operator[](difference_type index) const noexcept -> const_reference {return *this->at(index);}

			constexpr auto begin() noexcept -> iterator;
			constexpr auto end() noexcept -> iterator;
			constexpr auto cbegin() const noexcept -> const_iterator;
			constexpr auto cend() const noexcept -> const_iterator;
			constexpr auto begin() const noexcept -> const_iterator {return this->cbegin();}
			constexpr auto end() const noexcept -> const_iterator {return this->cend();}
			constexpr auto rbegin() noexcept -> reverse_iterator;
			constexpr auto rend() noexcept -> reverse_iterator;
			constexpr auto crbegin() const noexcept -> const_reverse_iterator;
			constexpr auto crend() const noexcept -> const_reverse_iterator;
			constexpr auto rbegin() const noexcept -> const_reverse_iterator {return this->crbegin();}
			constexpr auto rend() const noexcept -> const_reverse_iterator {return this->crend();}

			constexpr auto isEmpty() const noexcept -> bool {return this->getSize() == 0;}
			constexpr auto getData() const noexcept -> const CharT*;
			constexpr auto getSize() const noexcept -> size_type;
			constexpr auto getCapacity() const noexcept -> size_type;


		private:
			constexpr auto m_isSSO() const noexcept -> bool;
			constexpr auto m_allocate(size_type size) noexcept -> pointer;
			constexpr auto m_deallocate(pointer res, size_type size) noexcept -> void;
			constexpr auto m_normalizeIndex(difference_type index, size_type size = 0) const noexcept -> difference_type;
			constexpr auto m_copyAllocator() const noexcept -> Alloc;

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

			static constexpr auto s_createContent(const Alloc &alloc) noexcept -> Content<Alloc>;

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
	constexpr auto getSize(const CharT *str) noexcept -> std::size_t {return std::strlen(str);}
	template <typename CharT, typename Alloc>
	constexpr auto getSize(const BasicString<CharT, Alloc> &str) noexcept -> BasicString<CharT, Alloc>::size_type {return str.getSize();}


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
			constexpr auto getTuple() const noexcept -> const AddressTuple& {return m_strings;}

		private:
			template <typename T>
			constexpr auto s_makePointer(const T &value) noexcept -> const T* {return &value;}
			template <typename T>
			constexpr auto s_makePointer(T *ptr) noexcept -> const typename RemoveArray<T>::Type* {return ptr;}

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


	template <std::integral T, typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto stringToNumber(const sl::utils::BasicString<CharT, Alloc> &string) noexcept -> std::optional<T>;
	template <std::floating_point T, typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto stringToNumber(const sl::utils::BasicString<CharT, Alloc> &string) noexcept -> std::optional<T>;

	
	template <typename CharT, sl::memory::IsAllocator Alloc>
	inline auto operator<<(std::ostream &stream, const sl::utils::BasicString<CharT, Alloc> &str) noexcept -> std::ostream& {
		return stream << str.getData();
	}

	namespace literals {
		constexpr auto operator ""_s(const char *str, std::size_t length) noexcept -> sl::utils::BasicString<char> {
			return sl::utils::BasicString<char> (str, length);
		}
	} // namespace literals

} // namespace sl::utils


template <typename CharT, sl::memory::IsAllocator Alloc>
struct std::formatter<sl::utils::BasicString<CharT, Alloc>> : public std::formatter<const CharT*> {
	auto format(const sl::utils::BasicString<CharT, Alloc> &str, std::format_context &ctx) const {
		return std::formatter<const CharT*>::format(str.getData(), ctx);
	}
};

#include "sl/utils/string.inl"


namespace sl {
	using String = sl::utils::BasicString<char>;
} // namespace sl
