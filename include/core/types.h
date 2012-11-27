#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <stdint.h>
#include <stdlib.h>

#include <core/detect.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef float fp32_t;
typedef double fp64_t;

typedef unsigned int uint;

#define BIT(b) (1<<b)
#define CARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))
#define ALIGN_UP(ptr, align) (((uintptr_t)(ptr) + ((align)-1)) & ~((align)-1))

#if defined(COMPILER_GCC)
#	define align(a) __attribute__ ((aligned (a)))
#elif defined(COMPILER_MSVC)
#	define align(a) __declspec(align(a))
#else
#	error align not defined for this compiler
#endif

#if defined(COMPILER_GCC) || defined(COMPILER_MSVC)
#	define alignof __alignof
#else
#	error align not defined for this compiler
#endif

#if defined(COMPILER_GCC)
#	define force_inline static inline __attribute__((always_inline))
#elif defined(COMPILER_MSVC)
#	define force_inline __inline
#else
#	error force_inline not defined for this compiler
#endif

#if defined(COMPILER_GCC)
#	define restrict __restrict__
#elif defined(COMPILER_MSVC)
#	define restrict __restrict
#else
#	error restrict not defined for this compiler
#endif

#ifdef __cplusplus
}
#endif

#endif //CORE_TYPES_H
