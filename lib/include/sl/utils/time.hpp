#pragma once

#include "sl/core.hpp"
#include "sl/utils/units.hpp"


namespace sl::utils {
	class SL_CORE TimePoint {
		public:
			constexpr TimePoint() noexcept = default;

			constexpr TimePoint(const TimePoint&) noexcept = default;
			constexpr auto operator=(const TimePoint&) noexcept -> TimePoint& = default;

			constexpr auto operator-(const TimePoint &timepoint) const noexcept {
				sl::utils::Millisecond ds {(static_cast<float> (m_seconds) - static_cast<float> (timepoint.m_seconds)) * 1000.f};
				sl::utils::Millisecond dµs {(static_cast<float> (m_microseconds) - static_cast<float> (timepoint.m_microseconds)) * 0.001f};
				return ds + dµs;
			}

			static auto now() noexcept -> TimePoint;


		private:
			std::uint32_t m_seconds;
			std::uint32_t m_microseconds;
	};


	SL_CORE auto sleepFor(sl::utils::Millisecond time) noexcept -> void;
	inline auto sleepUntil(TimePoint point) noexcept -> void {
		sleepFor(point - TimePoint::now());
	}

} // namespace sl::utils
