#ifndef CORE_ALLOCATOR_H
#define CORE_ALLOCATOR_H

#include <core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct allocator;

typedef void* (*allocator_alloc_func_t)(struct allocator* allocator, uint32_t num_bytes, uint32_t align, const char* file, int line);
typedef void* (*allocator_realloc_func_t)(struct allocator* allocator, void* memory, uint32_t num_bytes, uint32_t align, const char* file, int line);
typedef void (*allocator_free_func_t)(struct allocator* allocator, void* memory);

/**
 * Helper struct to be used when passing around allocators.
 */
struct allocator
{
	allocator_alloc_func_t alloc;
	allocator_realloc_func_t realloc;
	allocator_free_func_t free;
};

extern struct allocator allocator_default;

#define ALLOC(allocator, num_bytes, align) ((allocator)->alloc(allocator, num_bytes, align, __FILE__, __LINE__))
#define REALLOC(allocator, memory, num_bytes, align) ((allocator)->realloc(allocator, memory, num_bytes, align, __FILE__, __LINE__))
#define FREE(allocator, mem) ((allocator)->free(allocator, mem))

struct allocator* allocator_incheap_create(struct allocator* parent, uint32_t num_bytes);
void allocator_incheap_destroy(struct allocator* parent, uint32_t num_bytes);
void allocator_incheap_reset(struct allocator* allocator);

struct alloc_helper
{
	struct allocator* allocator;
	uintptr_t base_ptr;
	uintptr_t curr_ptr;
	uintptr_t base_align;
	uintptr_t size;
};

struct alloc_helper alloc_helper_create(struct allocator* allocator);
void* alloc_helper_commit(struct alloc_helper* helper);
void alloc_helper_destroy(struct alloc_helper* helper);
void alloc_helper_add(struct alloc_helper* helper, uintptr_t size, uintptr_t align);
void* alloc_helper_get(struct alloc_helper* helper, uintptr_t size, uintptr_t align);

#define ALLOC_HELPER_ADD(helper, n, type) alloc_helper_add(helper, n*sizeof(type), alignof(type))
#define ALLOC_HELPER_GET(helper, n, type) (type*)alloc_helper_get(helper, n*sizeof(type), alignof(type))

#ifdef __cplusplus
}
#endif

#endif // CORE_ALLOCATOR_H
