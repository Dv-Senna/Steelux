#pragma once

#include <cstddef>
#include <concepts>

#include "sl/core.hpp"
#include "sl/utils/hash.hpp"


namespace sl::utils {
	SL_CORE auto generateUUID() noexcept -> std::uintmax_t;


	template <std::unsigned_integral T, std::size_t HASH>
	class BasicUUID {
		public:
			constexpr BasicUUID() noexcept = default;

			constexpr BasicUUID(const BasicUUID<T, HASH> &) noexcept = default;
			constexpr auto operator=(const BasicUUID<T, HASH> &) noexcept -> BasicUUID<T, HASH>& = default;
			constexpr BasicUUID(BasicUUID<T, HASH> &&) noexcept = default;
			constexpr auto operator=(BasicUUID<T, HASH> &&) noexcept -> BasicUUID<T, HASH>& = default;

			constexpr auto operator==(const BasicUUID<T, HASH> &uuid) const noexcept -> bool {return m_data == uuid.m_data;}
			constexpr auto operator<=>(const BasicUUID<T, HASH> &uuid) const noexcept {return m_data <=> uuid.m_data;}

			template <std::unsigned_integral U>
			constexpr explicit operator U() const noexcept {return static_cast<U> (m_data);}

			inline static auto generate() noexcept -> BasicUUID<T, HASH> {return BasicUUID(static_cast<T> (generateUUID()));}

		private:
			constexpr BasicUUID(T data) noexcept : m_data {data} {}

			T m_data;
	};
	
} // namespace sl::utils


namespace sl {
	using UUID = sl::utils::BasicUUID<std::uint64_t, static_cast<std::size_t> (sl::utils::hash<sl::utils::Hash64> (__FILE__, sizeof(__FILE__))) + __LINE__>;

} // namespace sl
