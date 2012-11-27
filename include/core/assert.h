#ifndef CORE_ASSERT_H
#define CORE_ASSERT_H

#include <stdarg.h>

#include <core/detect.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ASSERT
#	undef ASSERT
#endif

#if defined(COMPILER_MSVC)
#	define BREAKPOINT() __debugbreak()
#elif defined(COMPILER_GCC) && defined(ARCH_X86)
#	define BREAKPOINT() __asm__ ( "int %0" : :"I"(3) )
#elif defined(COMPILER_GCC) && defined(ARCH_PPC)
#	define BREAKPOINT() __asm__ volatile ("trap");
#else
#	error not defined for this platform
#endif

int assert_global_handler(const char* file, int line, const char* message);

#define ASSERT(cond, message) do{ \
	if(!(cond)) \
	{ \
		if(assert_global_handler(__FILE__, __LINE__, message) != 0) \
			BREAKPOINT(); \
	} \
} while(0)

#define STATIC_ASSERT(cond) do { int static_assert_array[(cond) ? 1 : -1]; (void)static_assert_array[0]; } while(0)

#ifdef __cplusplus
}
#endif

#endif //CORE_ASSERT_H
