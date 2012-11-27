#ifndef CORE_FILE_H
#define CORE_FILE_H

#include <core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

enum file_open_mode
{
	FILE_MODE_READ,
	FILE_MODE_WRITE,
};

enum file_seek_mode
{
	FILE_OFFSET_FROM_START,
	FILE_OFFSET_FROM_END,
	FILE_OFFSET_FROM_CURRENT,
};

struct allocator;

struct file* file_open(const char* file_name, enum file_open_mode mode);
void file_close(struct file* file);
size_t file_read(struct file* file, void* dst, size_t num_bytes);
void file_write(struct file* file, void* src, size_t num_bytes);
size_t file_size(struct file* file);
void* file_open_read_all(const char* file_name, struct allocator* allocator);
char* file_open_read_all_text(const char* file_name, struct allocator* allocator);

#ifdef __cplusplus
}
#endif

#endif // CORE_FILE_H
