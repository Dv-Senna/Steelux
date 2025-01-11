#pragma once

#include <ranges>
#include <source_location>
#include <stack>

#include "sl/core.hpp"
#include "sl/result.hpp"
#include "sl/utils/string.hpp"


namespace sl::utils {
	struct ErrorInfos {
		sl::String text;
		std::source_location location;
		sl::Result result;
	};

	class ErrorStack;

	class SL_CORE ErrorStackIterator final {
		friend class ErrorStack;

		public:
			using iterator_category = std::input_iterator_tag;
			using value_type = ErrorInfos;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using reference = value_type&;

			constexpr ErrorStackIterator() noexcept = default;
			constexpr ~ErrorStackIterator() = default;
		
			constexpr ErrorStackIterator(const ErrorStackIterator &) noexcept = default;
			constexpr auto operator=(const ErrorStackIterator &) noexcept -> ErrorStackIterator& = default;
			constexpr ErrorStackIterator(ErrorStackIterator &&) noexcept = default;
			constexpr auto operator=(ErrorStackIterator &&) noexcept -> ErrorStackIterator& = default;

			constexpr auto operator==(const ErrorStackIterator &it) const noexcept -> bool {return m_infos == it.m_infos;}

			auto operator++() noexcept -> ErrorStackIterator&;
			inline auto operator++(int) noexcept -> ErrorStackIterator {auto tmp {*this}; (void)++(*this); return tmp;}

			constexpr auto operator*() const noexcept -> reference {return *m_infos;}
			constexpr auto operator->() const noexcept -> pointer {return m_infos;}



		protected:
			constexpr ErrorStackIterator(ErrorInfos *infos) noexcept : m_infos {infos} {}

		private:
			ErrorInfos *m_infos;
	};

	static_assert(std::input_iterator<ErrorStackIterator>);

	class SL_CORE ErrorStackRange final {
		public:
			using iterator = ErrorStackIterator;

			constexpr ErrorStackRange() noexcept = default;
			constexpr ~ErrorStackRange() noexcept = default;

			constexpr ErrorStackRange(const ErrorStackRange &) noexcept = default;
			constexpr auto operator=(const ErrorStackRange &) noexcept -> ErrorStackRange& = default;
			constexpr ErrorStackRange(ErrorStackRange &&) noexcept = default;
			constexpr auto operator=(ErrorStackRange &&) noexcept -> ErrorStackRange& = default;

			inline auto begin() const noexcept -> iterator;
			inline auto end() const noexcept -> iterator;
	};

	static_assert(std::ranges::input_range<ErrorStackRange>);

	class SL_CORE ErrorStack final {
		friend class ErrorStackIterator;

		public:
			static constexpr std::size_t STACK_DEPTH {128};

			ErrorStack() = delete;

			template <typename T>
			inline static auto push(T &&retValue, const sl::String &text, std::source_location location = std::source_location::current()) noexcept -> T {
				if constexpr (std::same_as<T, sl::Result>)
					s_stack.push({text, location, retValue});
				else
					s_stack.push({text, location, sl::Result::eUnknown});
				return retValue;
			}

			inline static auto isEmpty() noexcept -> bool {return s_stack.empty();}
			static auto clear() noexcept -> void;

			inline static auto begin() noexcept -> ErrorStackIterator {return ErrorStackIterator(&s_stack.top());}
			inline static auto end() noexcept -> ErrorStackIterator {return ErrorStackIterator(nullptr);}

			constexpr static auto range() noexcept -> ErrorStackRange {return ErrorStackRange();}

		protected:
			static std::stack<ErrorInfos> s_stack;
	};


	auto ErrorStackRange::begin() const noexcept -> iterator {return ErrorStack::begin();}
	auto ErrorStackRange::end() const noexcept -> iterator {return ErrorStack::end();}

} // namespace sl::utils


namespace sl {
	using ErrorStack = sl::utils::ErrorStack;
} // namespace sl
