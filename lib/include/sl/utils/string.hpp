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

			using iterator = sl::utils::ContinousIterator<BasicString<Alloc>, CharT>;
			using const_iterator = sl::utils::ContinousIterator<const BasicString<Alloc>, const CharT>;
			using reverse_iterator = sl::utils::ReverseContinousIterator<BasicString<Alloc>, CharT>;
			using const_reverse_iterator = sl::utils::ReverseContinousIterator<const BasicString<Alloc>, const CharT>;


			constexpr BasicString(const Alloc &alloc = Alloc()) noexcept;
			constexpr BasicString(const CharT *str, const Alloc &alloc = Alloc()) noexcept;
			constexpr BasicString(const CharT *str, size_type size, const Alloc &alloc = Alloc()) noexcept;
			constexpr ~BasicString();

			constexpr BasicString(const BasicString<CharT, Alloc> &str) noexcept;
			constexpr BasicString<CharT, Alloc> &operator=(const BasicString<CharT, Alloc> &str) noexcept;
			constexpr BasicString(BasicString<CharT, Alloc> &&str) noexcept;
			constexpr BasicString<CharT, Alloc> &operator=(BasicString<CharT, Alloc> &&str) noexcept;

			constexpr const CharT *getData() const noexcept;
			constexpr size_type getSize() const noexcept;
			constexpr size_type getCapacity() const noexcept;


		private:
			constexpr bool m_isSSO() const noexcept;
			constexpr pointer m_allocate(size_type size) const noexcept;
			constexpr void m_deallocate(pointer res, size_type size) const noexcept;

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

			union {
				struct {
					pointer start;
					size_type capacity;
				} m_heap;

				struct {
					CharT buffer[sizeof(m_heap) / sizeof(CharT)];
				} m_sso;
			};

			static constexpr size_type MAX_SSO_CAPACITY {sizeof(m_heap) / sizeof(CharT)};
			static constexpr size_type MAX_SSO_SIZE {MAX_SSO_CAPACITY - 1};
	};


//	static_assert(std::ranges::random_access_range<String>, "String type must fullfill std::ranges::random_access_range concept");

} // namespace sl::utils


#include "sl/utils/string.inl"


namespace sl {
	using String = sl::utils::BasicString<char>;
} // namespace sl
