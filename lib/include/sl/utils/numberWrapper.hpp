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
			constexpr Self &operator=(const Self &) noexcept = default;
			constexpr UnsignedIntFlagWrapper(Self &&) noexcept = default;
			constexpr Self &operator=(Self &&) noexcept = default;

			constexpr operator T() const noexcept {return m_value & VALUE_MASK;}
			constexpr const Self &operator=(T value) noexcept {(m_value = m_value & FLAGS_MASK) | (value & VALUE_MASK); return *this;}

			template <std::unsigned_integral U>
			constexpr Self &operator+=(U value) noexcept {m_value = (m_value & FLAGS_MASK) | (((m_value & VALUE_MASK) + (static_cast<U> (value) & VALUE_MASK)) & VALUE_MASK); return *this;}
			template <std::unsigned_integral U>
			constexpr Self &operator-=(U value) noexcept {m_value = (m_value & FLAGS_MASK) | (((m_value & VALUE_MASK) - (static_cast<U> (value) & VALUE_MASK)) & VALUE_MASK); return *this;}
			template <std::unsigned_integral U>
			constexpr Self &operator*=(U value) noexcept {m_value = (m_value & FLAGS_MASK) | (((m_value & VALUE_MASK) * (static_cast<U> (value) & VALUE_MASK)) & VALUE_MASK); return *this;}
			template <std::unsigned_integral U>
			constexpr Self &operator/=(U value) noexcept {m_value = (m_value & FLAGS_MASK) | (((m_value & VALUE_MASK) / (static_cast<U> (value) & VALUE_MASK)) & VALUE_MASK); return *this;}

			constexpr Self &operator+=(Self wrapper) noexcept {return *this += wrapper.m_value;}
			constexpr Self &operator-=(Self wrapper) noexcept {return *this -= wrapper.m_value;}
			constexpr Self &operator*=(Self wrapper) noexcept {return *this *= wrapper.m_value;}
			constexpr Self &operator/=(Self wrapper) noexcept {return *this /= wrapper.m_value;}

			constexpr Self operator+(const Self &wrapper) noexcept {auto copy {*this}; return copy += wrapper;}
			constexpr Self operator-(const Self &wrapper) noexcept {auto copy {*this}; return copy -= wrapper;}
			constexpr Self operator*(const Self &wrapper) noexcept {auto copy {*this}; return copy *= wrapper;}
			constexpr Self operator/(const Self &wrapper) noexcept {auto copy {*this}; return copy /= wrapper;}

			constexpr Self &operator++() noexcept {return *this += static_cast<T> (1);}
			constexpr Self operator++(int) noexcept {auto tmp {*this}; *this += static_cast<T> (1); return tmp;}
			constexpr Self &operator--() noexcept {return *this -= static_cast<T> (1);}
			constexpr Self operator--(int) noexcept {auto tmp {*this}; *this -= static_cast<T> (1); return tmp;}

			constexpr bool operator==(Self wrapper) const noexcept {return m_value == wrapper.m_value;}
			constexpr bool operator==(T value) const noexcept {return (m_value & VALUE_MASK) == (value & VALUE_MASK);}
			constexpr auto operator<=>(Self wrapper) const noexcept {return (m_value & VALUE_MASK) <=> (wrapper.m_value & VALUE_MASK);}
			constexpr auto operator<=>(T value) const noexcept {return (m_value & VALUE_MASK) <=> (value & VALUE_MASK);}

			constexpr bool operator[](std::size_t index) const noexcept {return !!(s_getFlagMask(index) & m_value);}
			constexpr void setFlag(std::size_t index, bool value) noexcept {
				if (value)
					m_value = m_value | s_getFlagMask(index);
				else
					m_value = (m_value & VALUE_MASK) | (m_value & (FLAGS_MASK ^ s_getFlagMask(index)));
			}

			template <std::size_t INDEX>
			requires (INDEX < BIT_COUNT)
			constexpr bool getFlag() const noexcept {return !!(s_getFlagMask<INDEX> () & m_value);}

			template <std::size_t INDEX>
			requires (INDEX < BIT_COUNT)
			constexpr void setFlag(bool value) noexcept {
				if (value)
					m_value = m_value | s_getFlagMask<INDEX> ();
				else
					m_value = (m_value & VALUE_MASK) | (m_value & (FLAGS_MASK ^ s_getFlagMask<INDEX> ()));
			}


		private:
			static constexpr T FLAGS_MASK {[]() -> T {
				T mask {0};
				for (std::size_t i {0}; i < BIT_COUNT; ++i)
					mask |= (static_cast<T> (1) << (std::numeric_limits<T>::digits) - 1) >> i;
				return mask;
			} ()};
			static constexpr T VALUE_MASK {~FLAGS_MASK};

			static constexpr T s_getFlagMask(std::size_t i) noexcept {return (static_cast<T> (1) << (std::numeric_limits<T>::digits - 1)) >> i;}
			template <std::size_t INDEX>
			requires (INDEX < BIT_COUNT)
			static constexpr T s_getFlagMask() noexcept {return (static_cast<T> (1) << (std::numeric_limits<T>::digits - 1)) >> INDEX;}

			T m_value;
	};


	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr UnsignedIntFlagWrapper<T, BIT_COUNT> operator+(UnsignedIntFlagWrapper<T, BIT_COUNT> lhs, U rhs) noexcept {return lhs += rhs;}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr UnsignedIntFlagWrapper<T, BIT_COUNT> operator-(UnsignedIntFlagWrapper<T, BIT_COUNT> lhs, U rhs) noexcept {return lhs -= rhs;}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr UnsignedIntFlagWrapper<T, BIT_COUNT> operator*(UnsignedIntFlagWrapper<T, BIT_COUNT> lhs, U rhs) noexcept {return lhs *= rhs;}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr UnsignedIntFlagWrapper<T, BIT_COUNT> operator/(UnsignedIntFlagWrapper<T, BIT_COUNT> lhs, U rhs) noexcept {return lhs /= rhs;}

	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr UnsignedIntFlagWrapper<T, BIT_COUNT> operator+(U lhs, UnsignedIntFlagWrapper<T, BIT_COUNT> rhs) noexcept {UnsignedIntFlagWrapper<T, BIT_COUNT> tmp {lhs}; return tmp += rhs;}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr UnsignedIntFlagWrapper<T, BIT_COUNT> operator-(U lhs, UnsignedIntFlagWrapper<T, BIT_COUNT> rhs) noexcept {UnsignedIntFlagWrapper<T, BIT_COUNT> tmp {lhs}; return tmp -= rhs;}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr UnsignedIntFlagWrapper<T, BIT_COUNT> operator*(U lhs, UnsignedIntFlagWrapper<T, BIT_COUNT> rhs) noexcept {UnsignedIntFlagWrapper<T, BIT_COUNT> tmp {lhs}; return tmp *= rhs;}
	template <std::unsigned_integral T, std::size_t BIT_COUNT, std::unsigned_integral U>
	constexpr UnsignedIntFlagWrapper<T, BIT_COUNT> operator/(U lhs, UnsignedIntFlagWrapper<T, BIT_COUNT> rhs) noexcept {UnsignedIntFlagWrapper<T, BIT_COUNT> tmp {lhs}; return tmp /= rhs;}

} // namespace sl::utils
