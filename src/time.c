#include <time.h>

#if defined(FAMILY_UNIX)
#	include <sys/time.h>
#elif defined(FAMILY_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 
#	define _WIN32_WINNT 0x0600
#	include <windows.h>
#endif

#include <core/time.h>

uint64_t time_current()
{
#if defined(FAMILY_UNIX)
	struct timeval val;
	gettimeofday(&val, NULL);
	return (uint64_t)val.tv_sec * (uint64_t)1000000UL + (uint64_t)val.tv_usec;
#elif defined(FAMILY_WINDOWS)
	uint64_t t;
	QueryPerformanceCounter((PLARGE_INTEGER)&t);
	return t;
#else
	#error not implemented for this platform
#endif
}

uint64_t time_frequency()
{
#if defined(FAMILY_UNIX)
	return 1000000UL;
#elif defined(FAMILY_WINDOWS)
	uint64_t t;
	QueryPerformanceFrequency((PLARGE_INTEGER)&t);
	return t;
#else
	#error not implemented for this platform
#endif
}
