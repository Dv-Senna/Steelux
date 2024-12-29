#pragma once

#include <cstddef>

#include "sl/core.hpp"
#include "sl/memory/allocator.hpp"
#include "sl/utils/iterator.hpp"


namespace sl::utils {
	/**
	 * @brief A class that handles strings in Steelux
	 */
	template <typename CharT, sl::memory::IsAllocator Alloc = sl::memory::DefaultAllocator<CharT>>
	class SL_CORE BasicString final {
		public:
			using iterator = sl::utils::ContinousIterator<BasicString<Alloc>, CharT>;
			using const_iterator = sl::utils::ContinousIterator<const BasicString<Alloc>, const CharT>;
			using reverse_iterator = sl::utils::ReverseContinousIterator<BasicString<Alloc>, CharT>;
			using const_reverse_iterator = sl::utils::ReverseContinousIterator<const BasicString<Alloc>, const CharT>;


		private:
			// size does not include the null-terminating character
			template <typename Alloc2>
			struct Content {
				Alloc2 allocator;
				std::size_t size;
			};

			template <sl::memory::IsAllocator Alloc2>
			requires (!sl::memory::IsAllocatorStatefull_v<Alloc2>)
			struct Content<Alloc2> {
				std::size_t size;
			};

			constexpr bool m_isSSO() const noexcept;

			Content<Alloc> m_content;

			union {
				struct {
					CharT *start;
					std::ptrdiff_t capacity;
				} m_heap;

				struct {
					CharT buffer[sizeof(m_heap) / sizeof(CharT)];
				} m_sso;
			};

			static constexpr std::ptrdiff_t MAX_SSO_SIZE {sizeof(m_heap) / sizeof(CharT) - 1};
	};


//	static_assert(std::ranges::random_access_range<String>, "String type must fullfill std::ranges::random_access_range concept");

} // namespace sl::utils


//#include "sl/utils/string.inl"


namespace sl {
	using String = sl::utils::BasicString<char>;
} // namespace sl
