#include <stdlib.h>
#include <core/allocator.h>
#include <core/assert.h>

void* allocator_default_alloc(struct allocator* allocator, uint32_t num_bytes, uint32_t align, const char* file, int line)
{
	return malloc(num_bytes);
}

void* allocator_default_realloc(struct allocator* allocator, void* memory, uint32_t num_bytes, uint32_t align, const char* file, int line)
{
	return realloc(memory, num_bytes);
}

void allocator_default_free(struct allocator* allocator, void* memory)
{
	free(memory);
}

struct allocator allocator_default = {
	allocator_default_alloc,
	allocator_default_realloc,
	allocator_default_free
};

struct allocator_incheap
{
	allocator_alloc_func_t alloc;
	allocator_realloc_func_t realloc;
	allocator_free_func_t free;

	struct allocator* parent;
	
	uint8_t* baseptr;
	uint32_t offset;
	uint32_t size;
};

void* allocator_incheap_alloc(struct allocator* allocator, uint32_t num_bytes, uint32_t align, const char* file, int line)
{
	struct allocator_incheap* incheap = (struct allocator_incheap*)allocator;
	void* res = incheap->baseptr + incheap->offset;
	ASSERT(incheap->offset + num_bytes <= incheap->size, "Incheap out of space.");
	incheap->offset += num_bytes;
	return res;
}

void* allocator_incheap_realloc(struct allocator* allocator, void* memory, uint32_t num_bytes, uint32_t align, const char* file, int line)
{
	ASSERT(0, "Cannot realloc on incheap");
	return 0x0;
}

void allocator_incheap_free(struct allocator* allocator, void* memory)
{
	// NOP.
}

void allocator_incheap_reset(struct allocator* allocator)
{
	struct allocator_incheap* incheap = (struct allocator_incheap*)allocator;
	incheap->offset = 0;
}

struct allocator* allocator_incheap_create(struct allocator* parent, uint32_t num_bytes)
{
	struct allocator_incheap* incheap = 0x0;
	uint8_t* ptr = ALLOC(parent, num_bytes + sizeof(struct allocator_incheap), 16);
	ASSERT(ptr, "Could not allocate incheap data");

	incheap = (struct allocator_incheap*)ptr;
	incheap->alloc = allocator_incheap_alloc;
	incheap->realloc = allocator_incheap_realloc;
	incheap->free = allocator_incheap_free;
	incheap->parent = parent;
	incheap->baseptr = ptr + sizeof(struct allocator_incheap);
	incheap->offset = 0;
	incheap->size = num_bytes;

	return (struct allocator*)incheap;
}

void allocator_incheap_destroy(struct allocator* allocator, uint32_t num_bytes)
{
	struct allocator_incheap* incheap = (struct allocator_incheap*)allocator;
	FREE(incheap->parent, incheap);
}

struct alloc_helper alloc_helper_create(struct allocator* allocator)
{
	struct alloc_helper res = {
		allocator,
		0,
		0,
		0,
	};

	return res;
}

void* alloc_helper_commit(struct alloc_helper* helper)
{
	//ASSERT(helper->base_ptr == 0, "Cannot commit twice");
	//ASSERT(helper->allocator != NULL, "No allocator");
	void* res = ALLOC(helper->allocator, helper->size, helper->base_align);
	helper->base_ptr = helper->curr_ptr = (uintptr_t)res;
	return res;
}

void alloc_helper_destroy(struct alloc_helper* helper)
{
	ASSERT(helper->allocator != NULL, "No allocator");
	FREE(helper->allocator, (void*)helper->base_ptr);
}

void alloc_helper_add(struct alloc_helper* helper, uintptr_t size, uintptr_t align)
{
	// base align if this is the first add
	if(helper->base_align == 0)
		helper->base_align = align;
	else
		size += align-1; // reserve room for worst case misaligned start address
	helper->size += size;
}

void* alloc_helper_get(struct alloc_helper* helper, uintptr_t size, uintptr_t align)
{
	uintptr_t ptr = ALIGN_UP(helper->curr_ptr, align);
	void* res = (void*)ptr;
	ASSERT(ptr + size - helper->base_ptr <= helper->size, "Buffer overrun");
	helper->curr_ptr = ptr + size;
	return res;
}
