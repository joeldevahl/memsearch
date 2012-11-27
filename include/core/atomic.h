#ifndef CORE_ATOMIC_H
#define CORE_ATOMIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <core/types.h>

force_inline bool atomic_set(volatile uint32_t *var);
force_inline void atomic_clear(volatile uint32_t *var);
force_inline uint32_t atomic_comp_and_swap(volatile uint32_t *var, uint32_t old_var, uint32_t new_var);
force_inline uint32_t atomic_inc(volatile uint32_t *var);
force_inline uint32_t atomic_dec(volatile uint32_t *var);

#ifdef PLATFORM_OSX
#	include <core/atomic_osx.h>
#else
#	error "Atomics not implemented for this platform"
#endif

#ifdef __cplusplus
}
#endif

#endif // CORE_ATOMIC_H
