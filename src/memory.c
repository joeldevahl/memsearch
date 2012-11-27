#include <memory.h>

#include <core/memory.h>


void memory_copy(void* restrict dst, const void* restrict src, uint32_t num_bytes)
{
	memcpy(dst, src, num_bytes);
}

void memory_zero(void* dst, uint32_t num_bytes)
{
	memset(dst, 0, num_bytes);
}

void memory_set(void* dst, uint32_t num_bytes, uint32_t val)
{
	memset(dst, val, num_bytes);
}
