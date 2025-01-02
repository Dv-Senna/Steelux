#pragma once

#include <concepts>
#include <limits>


namespace sl::utils {
	template <std::unsigned_integral T, std::size_t BIT_COUNT>
	requires (BIT_COUNT < std::numeric_limits<T>::digits)
	class UnsignedIntFlagWrapper final {
		using Self = UnsignedIntFlagWrapper<T, BIT_COUNT>;

		public:
			constexpr UnsignedIntFlagWrapper() noexcept : m_value {} {}
			template <std::unsigned_integral U>
			constexpr UnsignedIntFlagWrapper(U value) noexcept : m_value {static_cast<U> (value) & VALUE_MASK} {}
			constexpr ~UnsignedIntFlagWrapper() = default;

			constexpr UnsignedIntFlagWrapper(const Self &) noexcept = default;
			constexpr auto operator=(const Self &) noexcept -> Self& = default;
			constexpr UnsignedIntFlagWrapper(Self &&) noexcept = default;
			constexpr auto operator=(Self &&) noexcept -> Self& = default;

			constexpr operator T() const noexcept {return m_value & VALUE_MASK;}
			constexpr auto operator=(T value) noexcept -> const Self& {m_value = (m_value & FLAGS_MASK) | (value & VALUE_MASK); return *this;}

			template <std::unsigned_integral U>
			constexpr auto operator+=(U value) noexcept -> Self& {
				m_value = (m_value & FLAGS_MASK) | (((m_value & VALUE_MASK) + (static_cast<U> (value) & VALUE_MASK)) & VALUE_MASK); return *this;
			}
			template <std::unsigned_integral U>
			constexpr auto operator-=(U value) noexcept -> Self& {
				m_value = (m_value & FLAGS_MASK) | (((m_value & VALUE_MASK) - (static_cast<U> (value) & VALUE_MASK)) & VALUE_MASK); return *this;
			}
			template <std::unsigned_integral U>
			constexpr auto operator*=(U value) noexcept -> Self& {
				m_value = (m_value & FLAGS_MASK) | (((m_value & VALUE_MASK) * (static_cast<U> (value) & VALUE_MASK)) & VALUE_MASK); return *this;
			}
			template <std::unsigned_integral U>
			constexpr auto operator/=(U value) noexcept -> Self& {
				m_value = (m_value & FLAGS_MASK) | (((m_value & VALUE_MASK) / (static_cast<U> (value) & VALUE_MASK)) & VALUE_MASK); return *this;
			}

			constexpr auto operator+=(Self wrapper) noexcept -> Self& {return *this += wrapper.m_value;}
			constexpr auto operator-=(Self wrapper) noexcept -> Self& {return *this -= wrapper.m_value;}
			constexpr auto operator*=(Self wrapper) noexcept -> Self& {return *this *= wrapper.m_value;}
			constexpr auto operator/=(Self wrapper) noexcept -> Self& {return *this /= wrapper.m_value;}

			constexpr auto operator+(const Self &wrapper) noexcept -> Self {auto copy {*this}; return copy += wrapper;}
			constexpr auto operator-(const Self &wrapper) noexcept -> Self {auto copy {*this}; return copy -= wrapper;}
			constexpr auto operator*(const Self &wrapper) noexcept -> Self {auto copy {*this}; return copy *= wrapper;}
			constexpr auto operator/(const Self &wrapper) noexcept -> Self {auto copy {*this}; return copy /= wrapper;}

			constexpr auto operator++() noexcept -> Self& {return *this += static_cast<T> (1);}
			constexpr auto operator++(int) noexcept -> Self {auto tmp {*this}; *this += static_cast<T> (1); return tmp;}
			constexpr auto operator--() noexcept -> Self& {return *this -= static_cast<T> (1);}
			constexpr auto operator--(int) noexcept -> Self {auto tmp {*this}; *this -= static_cast<T> (1); return tmp;}

			constexpr auto operator==(Self wrapper) const noexcept -> bool {return m_value == wrapper.m_value;}
			constexpr auto operator==(T value) const noexcept -> bool {return (m_value & VALUE_MASK) == (value & VALUE_MASK);}
			constexpr auto operator<=>(Self wrapper) const noexcept -> auto {return (m_value & VALUE_MASK) <=> (wrapper.m_value & VALUE_MASK);}
			constexpr auto operator<=>(T value) const noexcept -> auto {return (m_value & VALUE_MASK) <=> (value & VALUE_MASK);}

			constexpr auto operator[](std::size_t index) const noexcept -> bool {return !!(s_getFlagMask(index) & m_value);}
			constexpr auto setFlag(std::size_t index, bool value) noexcept -> void {
				if (value)
					m_value = m_value | s_getFlagMask(index);
				else
					m_value = (m_value & VALUE_MASK) | (m_value & (FLAGS_MASK ^ s_getFlagMask(index)));
			}

			template <std::size_t INDEX>
			requires (INDEX < BIT_COUNT)
			constexpr auto getFlag() const noexcept -> bool {return !!(s_getFlagMask<INDEX> () & m_value);}

			template <std::size_t INDEX>
			requires (INDEX < BIT_COUNT)
			constexpr auto setFlag(bool value) noexcept -> void {
				if (value)
					m_value = m_value | s_getFlagMask<INDEX> ();
				else
					m_value = (m_value & VALUE_MASK) | (m_value & (FLAGS_MASK ^ s_getFlagMask<INDEX> ()));
			}


		private:
			static constexpr T FLAGS_MASK {[]() -> T {
				T mask {0};
				for (std::size_t i {0}; i < BIT_COUNT; ++i)
					mask |= (static_cast<T> (1) << (std::numeric_limits<T>::digits - 1)) >> i;
				return mask;
			} ()};
			static constexpr T VALUE_MASK {~FLAGS_MASK};

			static constexpr auto s_getFlagMask(std::size_t i) noexcept -> T {return (static_cast<T> (1) << (std::numeric_limits<T>::digits - 1)) >> i;}
			template <std::size_t INDEX>
			requires (INDEX < BIT_COUNT)
			static constexpr auto s_getFlagMask() noexcept -> T {return (static_cast<T> (1) << (std::numeric_limits<T>::digits - 1)) >> INDEX;}

			T m_value;
	};


	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr auto operator+(UnsignedIntFlagWrapper<T, BIT_COUNT> lhs, U rhs) noexcept -> UnsignedIntFlagWrapper<T, BIT_COUNT> {return lhs += rhs;}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr auto operator-(UnsignedIntFlagWrapper<T, BIT_COUNT> lhs, U rhs) noexcept -> UnsignedIntFlagWrapper<T, BIT_COUNT> {return lhs -= rhs;}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr auto operator*(UnsignedIntFlagWrapper<T, BIT_COUNT> lhs, U rhs) noexcept -> UnsignedIntFlagWrapper<T, BIT_COUNT> {return lhs *= rhs;}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr auto operator/(UnsignedIntFlagWrapper<T, BIT_COUNT> lhs, U rhs) noexcept -> UnsignedIntFlagWrapper<T, BIT_COUNT> {return lhs /= rhs;}

	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr auto operator+(U lhs, UnsignedIntFlagWrapper<T, BIT_COUNT> rhs) noexcept -> UnsignedIntFlagWrapper<T, BIT_COUNT> {
		UnsignedIntFlagWrapper<T, BIT_COUNT> tmp {lhs}; return tmp += rhs;
	}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr auto operator-(U lhs, UnsignedIntFlagWrapper<T, BIT_COUNT> rhs) noexcept -> UnsignedIntFlagWrapper<T, BIT_COUNT> {
		UnsignedIntFlagWrapper<T, BIT_COUNT> tmp {lhs}; return tmp -= rhs;
	}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr auto operator*(U lhs, UnsignedIntFlagWrapper<T, BIT_COUNT> rhs) noexcept -> UnsignedIntFlagWrapper<T, BIT_COUNT> {
		UnsignedIntFlagWrapper<T, BIT_COUNT> tmp {lhs}; return tmp *= rhs;
	}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr auto operator/(U lhs, UnsignedIntFlagWrapper<T, BIT_COUNT> rhs) noexcept -> UnsignedIntFlagWrapper<T, BIT_COUNT> {
		UnsignedIntFlagWrapper<T, BIT_COUNT> tmp {lhs}; return tmp /= rhs;
	}

} // namespace sl::utils
