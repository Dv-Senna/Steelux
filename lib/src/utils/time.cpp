#include "sl/utils/time.hpp"

#ifdef SL_LINUX
	#include <sys/time.h>
	#include <unistd.h>
#elifdef SL_WINDOWS
	#include <Windows.h>
#endif


namespace sl::utils {
	auto TimePoint::now() noexcept -> TimePoint {
		TimePoint timePoint {};

	#ifdef SL_LINUX
		timeval tv {};
		gettimeofday(&tv, nullptr);
		timePoint.m_seconds = tv.tv_sec;
		timePoint.m_microseconds = tv.tv_usec;

	#elifdef SL_WINDOWS
		LARGE_INTEGER frequency {};
		(void)QueryPerformanceFrequency(&frequency);
	#endif

		return timePoint;
	}


	auto sleepFor(sl::utils::Millisecond time) noexcept -> void {
	#ifdef SL_LINUX
		(void)usleep(static_cast<useconds_t> (sl::utils::Microsecond (time)));
	#elifdef SL_WINDOWS
	#endif
	}

} // namespace sl::utils
