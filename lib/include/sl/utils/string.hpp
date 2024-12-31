#pragma once

#include <cstddef>

#include "sl/memory/allocator.hpp"
#include "sl/utils/iterator.hpp"
#include "sl/utils/numberWrapper.hpp"


namespace sl::utils {
	template <typename Left, typename Right>
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

			constexpr size_type reserve(size_type newSize) noexcept;
			constexpr size_type shrinkToFit() noexcept;

			constexpr iterator insert(difference_type position, CharT value, size_type count = 1) noexcept;
			constexpr iterator insert(const iterator &position, CharT value, size_type count = 1) noexcept {return this->insert(position - this->begin(), value, count);}
			constexpr iterator insert(const iterator &start, const iterator &end, CharT value) noexcept {return this->insert(start - this->begin(), value, end - start);}
			constexpr iterator insert(const reverse_iterator &position, CharT value, size_type count = 1) noexcept {return this->insert(this->rbegin() - position - 1, value, count);}
			constexpr iterator insert(const reverse_iterator &start, const reverse_iterator &end, CharT value) noexcept {return this->insert(this->rbegin() - end - 1, value, end - start);}
			constexpr iterator pushFront(CharT value, size_type count = 1) noexcept {return this->insert(this->begin(), value, count);}
			constexpr iterator pushBack(CharT value, size_type count = 1) noexcept {return this->insert(this->end(), value, count);}

			constexpr iterator insert(difference_type position, const sl::utils::BasicString<CharT, Alloc> &str) noexcept;
			constexpr iterator insert(const iterator &position, const sl::utils::BasicString<CharT, Alloc> &str) noexcept {return this->insert(position - this->begin(), str);}
			constexpr iterator insert(const reverse_iterator &position, const sl::utils::BasicString<CharT, Alloc> &str) noexcept {return this->insert(this->rbegin() - position - 1, str);}
			constexpr iterator pushFront(const sl::utils::BasicString<CharT, Alloc> &str) noexcept {return this->insert(this->begin(), str);}
			constexpr iterator pushBack(const sl::utils::BasicString<CharT, Alloc> &str) noexcept {return this->insert(this->end(), str);}

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



	template <typename LeftT, typename RightT>
	class ConcatStringView final {
		template <typename ...Args>
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


		template <typename T, typename = void>
		struct GetTupleIfAny {
			using Type = T;
		};

		template <typename T>
		struct GetTupleIfAny<T, void_t<typename T::Tuple>> {
			using Type = typename T::Tuple;
		};


		template <typename LeftT2, typename RightT2>
		struct TupleConcatenater {
			using Type = std::tuple<LeftT2, RightT2>;
		};

		template <typename ...LeftArgs, typename RightT2>
		struct TupleConcatenater<std::tuple<LeftArgs...>, RightT2> {
			using Type = std::tuple<LeftArgs..., RightT2>;
		};

		template <typename LeftT2, typename ...RightArgs>
		struct TupleConcatenater<LeftT2, std::tuple<RightArgs...>> {
			using Type = std::tuple<LeftT2, RightArgs...>;
		};

		template <typename ...LeftArgs, typename ...RightArgs>
		struct TupleConcatenater<std::tuple<LeftArgs...>, std::tuple<RightArgs...>> {
			using Type = std::tuple<LeftArgs..., RightArgs...>;
		};


		template <typename T>
		struct RemoveArrayFromType {
			using Type = T;
		};

		template <typename T, std::size_t N>
		struct RemoveArrayFromType<const T(&)[N]> {
			using Type = const T;
		};


		template <typename T>
		struct AddPointerToType {
			using Type = std::conditional_t<
				std::is_pointer_v<std::remove_reference_t<T>> || std::is_array_v<std::remove_reference_t<T>>,
				T,
				std::add_pointer_t<std::add_const_t<T>>
/*				std::add_pointer_t<std::conditional_t<
					std::is_array_v<std::remove_reference_t<T>>,
					typename RemoveArrayFromType<T>::Type,
					std::add_const_t<T>
				>>*/
			>;
		};


		template <typename T>
		struct AddPointerToTupleElement {
			using Type = void;
		};

		template <typename ...Args>
		struct AddPointerToTupleElement<std::tuple<Args...>> {
			using Type = std::tuple<typename AddPointerToType<Args>::Type ...>;
		};

		public:
			using Left = LeftT;
			using Right = RightT;
			using Tuple = TupleConcatenater<typename GetTupleIfAny<LeftT>::Type, typename GetTupleIfAny<RightT>::Type>::Type;
			using AddressTuple = AddPointerToTupleElement<Tuple>::Type;
			using FirstString = FirstStringFinder<Tuple>::Type;
			using CharT = typename FirstString::value_type;
			using Allocator = typename FirstString::allocator_type;


			ConcatStringView(const LeftT &left, const RightT &right) noexcept;

			constexpr const AddressTuple &getTuple() const noexcept {return m_strings;}

		private:
			static AddressTuple s_concatenateTuples(const LeftT &left, const RightT &right) noexcept;

			template <typename LeftT2, typename RightT2>
			static AddressTuple s_concatenateTuplesInternal(const LeftT2 &left, const RightT2 &right) noexcept;
			template <typename ...LeftArgs, typename RightT2>
			static AddressTuple s_concatenateTuplesInternal(const std::tuple<LeftArgs...> &left, const RightT2 &right) noexcept;
			template <typename LeftT2, typename ...RightArgs>
			static AddressTuple s_concatenateTuplesInternal(const LeftT2 &left, const std::tuple<RightArgs...> &right) noexcept;
			template <typename ...LeftArgs, typename ...RightArgs>
			static AddressTuple s_concatenateTuplesInternal(const std::tuple<LeftArgs...> &left, const std::tuple<RightArgs...> &right) noexcept;

			AddressTuple m_strings;
	};

} // namespace sl::utils


#include "sl/utils/string.inl"


namespace sl {
	using String = sl::utils::BasicString<char>;
} // namespace sl
