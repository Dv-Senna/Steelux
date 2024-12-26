#pragma once


#ifdef SL_STATIC
	#define SL_CORE
#elif defined(SL_SHARED)
	#ifdef SL_WINDOWS
		#ifdef SL_BUILD_LIB
			#define SL_CORE __declspec(dllexport)
		#else
			#define SL_CORE __declspec(dllimport)
		#endif
	#else
		#define SL_CORE
	#endif
#else
	#error You must define either 'SL_STATIC' or 'SL_SHARED'
#endif
