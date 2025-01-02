#pragma once

#include <cstdint>
#include <compare>
#include <concepts>
#include <format>
#include <ostream>
#include <sstream>


namespace sl::utils {
	template <std::uintmax_t _NUM, std::uintmax_t _DEN>
	struct Ratio {
		static constexpr std::uintmax_t NUM {_NUM};
		static constexpr std::uintmax_t DEN {_DEN};
		static constexpr std::uintmax_t RATIO {NUM / DEN};
		static constexpr std::uintmax_t INVERSE_RATIO {DEN / NUM};
		static constexpr long double fNUM {static_cast<long double> (_NUM)};
		static constexpr long double fDEN {static_cast<long double> (_DEN)};
		static constexpr long double fRATIO {fNUM / fDEN};
		static constexpr long double fINVERSE_RATIO {fDEN / fNUM};
	};

	template <typename T>
	concept IsRatio = requires {
		{T::NUM} -> std::convertible_to<std::uintmax_t>;
		{T::DEN} -> std::convertible_to<std::uintmax_t>;
		{T::RATIO} -> std::convertible_to<std::uintmax_t>;
		{T::INVERSE_RATIO} -> std::convertible_to<std::uintmax_t>;
		{T::fNUM} -> std::convertible_to<long double>;
		{T::fDEN} -> std::convertible_to<long double>;
		{T::fRATIO} -> std::convertible_to<long double>;
		{T::fINVERSE_RATIO} -> std::convertible_to<long double>;
	};

	
	using Nano = Ratio<1, 1'000'000'000>;
	using Micro = Ratio<1, 1'000'000>;
	using Milli = Ratio<1, 1'000>;
	using Kilo = Ratio<1'000, 1>;
	using Mega = Ratio<1'000'000, 1>;
	using Giga = Ratio<1'000'000'000, 1>;

	using Kibi = Ratio<1024, 1>;
	using Mibi = Ratio<1024*1024, 1>;
	using Gibi = Ratio<1024*1024*1024, 1>;

	static_assert(IsRatio<Nano>);
	static_assert(IsRatio<Micro>);
	static_assert(IsRatio<Milli>);
	static_assert(IsRatio<Kilo>);
	static_assert(IsRatio<Mega>);
	static_assert(IsRatio<Giga>);
	static_assert(IsRatio<Kibi>);
	static_assert(IsRatio<Mibi>);
	static_assert(IsRatio<Gibi>);

	template <IsRatio R>
	struct RatioString {static constexpr const char str[] {""};};
	template <> struct RatioString<Nano> {static constexpr const char str[] {"n"};};
	template <> struct RatioString<Micro> {static constexpr const char str[] {"µ"};};
	template <> struct RatioString<Milli> {static constexpr const char str[] {"m"};};
	template <> struct RatioString<Kilo> {static constexpr const char str[] {"K"};};
	template <> struct RatioString<Mega> {static constexpr const char str[] {"M"};};
	template <> struct RatioString<Giga> {static constexpr const char str[] {"G"};};
	template <> struct RatioString<Kibi> {static constexpr const char str[] {"Ki"};};
	template <> struct RatioString<Mibi> {static constexpr const char str[] {"Mi"};};
	template <> struct RatioString<Gibi> {static constexpr const char str[] {"Gi"};};


	template <IsRatio R = Ratio<1, 1>>
	class ByteCount final {
		public:
			constexpr ByteCount() noexcept : m_data {0} {}
			constexpr ByteCount(std::size_t data) noexcept : m_data {static_cast<std::size_t> (data * R::fRATIO)} {}
			constexpr ByteCount(long double data) noexcept : m_data {static_cast<std::size_t> (data * R::fRATIO)} {}
			template <IsRatio R2>
			constexpr ByteCount(const ByteCount<R2> &ratio) noexcept : m_data {static_cast<std::size_t> (ratio)} {}
			template <IsRatio R2>
			constexpr ByteCount<R> &operator=(const ByteCount<R2> &ratio) noexcept {m_data = static_cast<std::size_t> (ratio); return *this;}
			template <IsRatio R2>
			constexpr ByteCount(ByteCount<R2> &&ratio) noexcept : m_data {static_cast<std::size_t> (ratio)} {}
			template <IsRatio R2>
			constexpr ByteCount<R> &operator=(ByteCount<R2> &&ratio) noexcept {m_data = static_cast<std::size_t> (ratio); return *this;}

			template <IsRatio R2>
			constexpr bool operator==(const ByteCount<R2> &ratio) const noexcept {return m_data == static_cast<std::size_t> (ratio);}
			template <IsRatio R2>
			constexpr auto operator<=>(const ByteCount<R2> &ratio) const noexcept {return m_data <=> static_cast<std::size_t> (ratio);}

			template <IsRatio R2>
			constexpr ByteCount<R> &operator+=(const ByteCount<R2> &ratio) noexcept {m_data += static_cast<std::size_t> (ratio); return *this;}
			template <IsRatio R2>
			constexpr ByteCount<R> &operator-=(const ByteCount<R2> &ratio) noexcept {m_data -= static_cast<std::size_t> (ratio); return *this;}
			constexpr ByteCount<R> &operator*=(std::size_t factor) noexcept {m_data *= factor; return *this;}
			constexpr ByteCount<R> &operator/=(std::size_t factor) noexcept {m_data /= factor; return *this;}

			template <IsRatio R2>
			constexpr ByteCount<R> operator+(const ByteCount<R2> &ratio) const noexcept {auto copy {*this}; return copy += ratio;}
			template <IsRatio R2>
			constexpr ByteCount<R> operator-(const ByteCount<R2> &ratio) const noexcept {auto copy {*this}; return copy -= ratio;}
			constexpr ByteCount<R> operator*(std::size_t factor) const noexcept {auto copy {*this}; return copy *= factor;}
			constexpr ByteCount<R> operator/(std::size_t factor) const noexcept {auto copy {*this}; return copy /= factor;}

			constexpr operator std::size_t() const noexcept {return m_data;}
			template <IsRatio R2>
			constexpr explicit operator ByteCount<R2> () const noexcept {return ByteCount<R2> (m_data);}


		private:
			std::size_t m_data;
	};

	template <IsRatio R>
	constexpr ByteCount<R> operator*(std::size_t factor, const ByteCount<R> &ratio) noexcept {return ratio * factor;}

	template <IsRatio R>
	inline std::ostream &operator<<(std::ostream &stream, const ByteCount<R> &ratio) noexcept {
		stream << static_cast<std::size_t> (ratio) * R::fINVERSE_RATIO << RatioString<R>::str << "B";
		return stream;
	}

	using Kilobytes = ByteCount<Kilo>;
	using Megabytes = ByteCount<Mega>;
	using Gigabytes = ByteCount<Giga>;
	using Kibibytes = ByteCount<Kibi>;
	using Mibibytes = ByteCount<Mibi>;
	using Gibibytes = ByteCount<Gibi>;
	using Bytes = ByteCount<>;

	namespace literals {
		constexpr Bytes operator ""_B(unsigned long long data) noexcept {return Bytes(static_cast<std::size_t> (data));}
		constexpr Bytes operator ""_B(long double data) noexcept {return Bytes(data);}
		constexpr Kilobytes operator ""_KB(unsigned long long data) noexcept {return Kilobytes(static_cast<std::size_t> (data));}
		constexpr Kilobytes operator ""_KB(long double data) noexcept {return Kilobytes(data);}
		constexpr Megabytes operator ""_MB(unsigned long long data) noexcept {return Megabytes(static_cast<std::size_t> (data));}
		constexpr Megabytes operator ""_MB(long double data) noexcept {return Megabytes(data);}
		constexpr Gigabytes operator ""_GB(unsigned long long data) noexcept {return Gigabytes(static_cast<std::size_t> (data));}
		constexpr Gigabytes operator ""_GB(long double data) noexcept {return Gigabytes(data);}
		constexpr Kibibytes operator ""_KiB(unsigned long long data) noexcept {return Kibibytes(static_cast<std::size_t> (data));}
		constexpr Kibibytes operator ""_KiB(long double data) noexcept {return Kibibytes(data);}
		constexpr Mibibytes operator ""_MiB(unsigned long long data) noexcept {return Mibibytes(static_cast<std::size_t> (data));}
		constexpr Mibibytes operator ""_MiB(long double data) noexcept {return Mibibytes(data);}
		constexpr Gibibytes operator ""_GiB(unsigned long long data) noexcept {return Gibibytes(static_cast<std::size_t> (data));}
		constexpr Gibibytes operator ""_GiB(long double data) noexcept {return Gibibytes(data);}
	} // namespace literals

	using namespace literals;
} // namespace sl::utils


template <sl::utils::IsRatio R>
struct std::formatter<sl::utils::ByteCount<R>> : public std::formatter<std::string> {
	inline auto format(const sl::utils::ByteCount<R> &bytes, std::format_context &ctx) const noexcept {
		std::ostringstream stream {};
		stream << bytes;
		return std::formatter<std::string>::format(stream.str(), ctx);
	}
};

