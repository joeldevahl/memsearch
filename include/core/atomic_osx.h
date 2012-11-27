#ifndef CORE_ATOMIC_OSX_H
#define CORE_ATOMIC_OSX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libkern/OSAtomic.h>

force_inline bool atomic_set(volatile uint32_t *var)
{
	return !OSAtomicCompareAndSwap32(0, 1, (volatile int32_t*)var);
}

force_inline void atomic_clear(volatile uint32_t *var)
{
	OSAtomicAnd32(0, (volatile uint32_t*)var);
}

force_inline uint32_t atomic_comp_and_swap(volatile uint32_t *var, uint32_t old_var, uint32_t new_var)
{
	return OSAtomicCompareAndSwap32(old_var, new_var, (volatile int32_t*)var);
}

force_inline uint32_t atomic_inc(volatile uint32_t *var)
{
	return OSAtomicIncrement32((volatile int32_t*)var);
}

force_inline uint32_t atomic_dec(volatile uint32_t *var)
{
	return OSAtomicDecrement32((volatile int32_t*)var);
}

#ifdef __cplusplus
}
#endif

#endif // CORE_ATOMIC_OSX_H
