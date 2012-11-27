#ifndef CORE_HASH_H
#define CORE_HASH_H

#include <core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t hash_string(const char* str);
uint32_t hash_buffer(const void* buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif // CORE_HASH_H
