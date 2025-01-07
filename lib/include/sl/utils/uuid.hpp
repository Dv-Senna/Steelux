#pragma once

#include <cstddef>
#include <concepts>

#include "sl/utils/hash.hpp"


namespace sl::utils {
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

			inline static auto generate() noexcept -> BasicUUID<T, HASH> {return BasicUUID(++s_lastUUID);}


		private:
			constexpr BasicUUID(T data) noexcept : m_data {data} {}

			static T s_lastUUID;
			T m_data;
	};
	
	template <std::unsigned_integral T, std::size_t HASH>
	T BasicUUID<T, HASH>::s_lastUUID {0};

} // namespace sl::utils


namespace sl {
	using UUID = sl::utils::BasicUUID<std::uint64_t, static_cast<std::size_t> (sl::utils::hash<sl::utils::Hash64> (__FILE__, sizeof(__FILE__))) + __LINE__>;

} // namespace sl
