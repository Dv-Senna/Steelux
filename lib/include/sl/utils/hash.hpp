#pragma once

#include <format>
#include <ostream>
#include <sstream>

#include "sl/utils/string.hpp"


namespace sl::utils {
	template <std::unsigned_integral T>
	class Hash {
		public:
			using HashType = T;

			constexpr Hash() noexcept = default;
			constexpr Hash(T value) noexcept : m_value {value} {}
			
			constexpr Hash(const Hash<T> &hash) noexcept = default;
			constexpr auto operator=(const Hash<T> &hash) noexcept -> Hash<T>& = default;
			constexpr Hash(Hash<T> &&hash) noexcept = default;
			constexpr auto operator=(Hash<T> &&hash) noexcept -> Hash<T>& = default;

			constexpr auto operator==(const Hash<T> &hash) const noexcept -> bool {return m_value == hash.m_value;}
			constexpr auto operator<=>(const Hash<T> &hash) const noexcept {return m_value <=> hash.m_value;}

			template <std::unsigned_integral U>
			constexpr explicit operator U() const noexcept {return static_cast<U> (m_value);}


		private:
			T m_value;
	};

	template <typename T>
	concept IsHash = std::unsigned_integral<typename T::HashType> && requires(const T val){
		{static_cast<typename T::HashType> (val)} -> std::convertible_to<typename T::HashType>;
	};


	template <std::unsigned_integral T, typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto hash(const sl::utils::BasicString<CharT, Alloc> &string) noexcept -> Hash<T>;
	template <std::unsigned_integral T>
	constexpr auto hash(const char *str, std::size_t N) noexcept -> Hash<T>;

	template <IsHash Hash, typename CharT, sl::memory::IsAllocator Alloc>
	constexpr auto hash(const sl::utils::BasicString<CharT, Alloc> &string) noexcept -> Hash {return hash<typename Hash::HashType, CharT, Alloc> (string);}
	template <IsHash Hash>
	constexpr auto hash(const char *str, std::size_t N) noexcept -> Hash {return hash<typename Hash::HashType> (str, N);}

	using Hash8  = Hash<std::uint8_t>;
	using Hash16 = Hash<std::uint16_t>;
	using Hash32 = Hash<std::uint32_t>;
	using Hash64 = Hash<std::uint64_t>;

	static_assert(IsHash<Hash8>);
	static_assert(IsHash<Hash16>);
	static_assert(IsHash<Hash32>);
	static_assert(IsHash<Hash64>);

	namespace literals {
		constexpr auto operator ""_hash8(const char *str, std::size_t N) noexcept -> Hash8 {return hash<std::uint8_t> (str, N);}
		constexpr auto operator ""_hash16(const char *str, std::size_t N) noexcept -> Hash16 {return hash<std::uint16_t> (str, N);}
		constexpr auto operator ""_hash32(const char *str, std::size_t N) noexcept -> Hash32 {return hash<std::uint32_t> (str, N);}
		constexpr auto operator ""_hash64(const char *str, std::size_t N) noexcept -> Hash64 {return hash<std::uint64_t> (str, N);}
	} // namespace literals

	using namespace sl::utils::literals;


	template <std::unsigned_integral T>
	inline auto operator<<(std::ostream &stream, const Hash<T> &hash) noexcept -> std::ostream& {
		return stream << std::hex << static_cast<std::uintmax_t> (static_cast<T> (hash)) << "_h" << std::dec << (sizeof(T) * 8);
	}

} // namespace sl::utils

#include "sl/utils/hash.inl"

template <std::unsigned_integral T>
struct std::formatter<sl::utils::Hash<T>> {
	constexpr auto parse(std::format_parse_context &ctx) noexcept {
		return ctx.begin();
	}

	inline auto format(const sl::utils::Hash<T> &hash, std::format_context &ctx) const noexcept {
		std::ostringstream stream {};
		stream << hash;
		return std::ranges::copy(stream.str(), ctx.out()).out;
	}
};

