#include <string.h>

#include <core/hash.h>

uint32_t hash_string(const char* str)
{
	return hash_buffer(str, strlen(str));
}

uint32_t hash_buffer(const void* buf, size_t size)
{
    uint32_t *ptr = (uint32_t *) buf;
    uint32_t hash = 0xbaadf00d;
    size_t i = 0;

    for(; i<(size/4); i++)
    {
        hash ^= (i << 4 ^ *ptr << 8 ^ *ptr);
        ptr++;
    }

    return hash;
}
