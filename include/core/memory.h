#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include <core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void memory_copy(void* restrict dst, const void* restrict src, uint32_t num_bytes);

void memory_zero(void* dst, uint32_t num_bytes);

void memory_set(void* dst, uint32_t num_bytes, uint32_t val);

#ifdef __cplusplus
}
#endif

#endif //CORE_MEMORY_H
