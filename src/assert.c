#include <stdio.h>
#include <stdarg.h>

#if defined(FAMILY_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 
#	define _WIN32_WINNT 0x0600
#	include <windows.h>
#endif

#include <core/assert.h>
#include <core/types.h>

int assert_global_handler(const char* file, int line, const char* message)
{
	/*
	char buf[1024*4] = {0};
	va_list args;
	va_start(args, message);
	vsnprintf(buf, CARRAY_LENGTH(buf), message, args);
#ifdef FAMILY_WINDOWS
	OutputDebugStringA(buf);
	OutputDebugStringA("\n");
#endif
	fprintf(stderr, buf);
	fprintf(stderr, "\n");
	va_end(args);
	*/
	printf(message);
	return -1;
}
