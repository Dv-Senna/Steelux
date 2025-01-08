#pragma once


#ifdef SL_WINDOWS
	#include <intrin.h>
	#define SL_DEBUG_BREAKPOINT __debugbreak()
#else
	#define SL_DEBUG_BREAKPOINT __asm__("int3")
#endif

#ifndef SL_NO_SLOW_ASSERT
	#ifdef SL_NO_ASSERT
		#define SL_NO_SLOW_ASSERT
	#elifdef NDEBUG
		#define SL_NO_SLOW_ASSERT
	#endif
#endif

#ifdef SL_NO_ASSERT
	#define SL_ASSERT(expr)
	#define SL_TEXT_ASSERT(expr, msg)
#else
	#include <cstdio>
	#define SL_ASSERT(expr)	if (!!(expr)) {} else {std::printf("ASSERT " __FILE__ ":%d failed : " #expr, __LINE__); SL_DEBUG_BREAKPOINT;}
	#define SL_TEXT_ASSERT(expr, msg) if (!!(expr)) {} else {std::printf("ASSERT " __FILE__ ":%d failed : " msg, __LINE__); SL_DEBUG_BREAKPOINT;}
#endif

#ifdef SL_NO_SLOW_ASSERT
	#define SL_SLOW_ASSERT(expr)
	#define SL_TEXT_SLOW_ASSERT(expr, msg)
#else
	#define SL_SLOW_ASSERT(expr) SL_ASSERT(expr)
	#define SL_TEXT_SLOW_ASSERT(expr, msg) SL_TEXT_ASSERT(expr, msg)
#endif
