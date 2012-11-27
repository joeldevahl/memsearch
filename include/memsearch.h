#ifndef MEMSEARCH_H_INCLUDED
#define MEMSEARCH_H_INCLUDED

#include <core/types.h>

struct memsearch;
struct allocator;

typedef void (*memsearch_find_callback_t)(void* ctx, const char* file, const char* str, uint line, uint col);
typedef void (*memsearch_add_callback_t)(void* ctx, const char* path);

struct memsearch* memsearch_create(struct allocator* allocator);

void memsearch_destroy(struct memsearch* memsearch);

void memsearch_add_file(struct memsearch* memsearch, const char* filename, memsearch_add_callback_t callback, void* callback_context);

void memsearch_add_dir(struct memsearch* memsearch, const char* dirname, const char** exts, uint num_exts, memsearch_add_callback_t callback, void* callback_context);

void memsearch_find(const struct memsearch* memsearch, const char* cmp, memsearch_find_callback_t callback, void* callback_context);

#endif // #define MEMSEARCH_H_INCLUDED
