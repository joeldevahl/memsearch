#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#ifdef FAMILY_UNIX
#	include <sys/uio.h>
#	include <unistd.h>
#endif
#include <fcntl.h>

#include <core/assert.h>
#include <core/file.h>
#include <core/allocator.h>

struct file* file_open(const char* file_name, enum file_open_mode mode)
{
	char* flags = 0;
	FILE* file;
	switch(mode)
	{
	case FILE_MODE_READ:
		flags = "rb";
		break;
	case FILE_MODE_WRITE:
		flags = "wb";
		break;
	default:
		BREAKPOINT();
	}

	file = fopen(file_name, flags);
	ASSERT(file, "Could not open file\n");
	return (struct file*)file;
}

void file_close(struct file* file)
{
	fclose((FILE*)file);
}

size_t file_read(struct file* file, void* dst, size_t num_bytes)
{
	return fread(dst, 1, num_bytes, (FILE*)file);
}

void file_write(struct file* file, void* src, size_t num_bytes)
{
	int num_written = fwrite(src, 1, num_bytes, (FILE*)file);
	ASSERT(num_written == num_bytes, "Error writing to file: incorrect number of bytes written\n");
}

size_t file_size(struct file* file)
{
	struct stat sb;
	int res = fstat(fileno((FILE*)file), &sb);
	ASSERT(res != -1, "Could not stat file\n");
	return sb.st_size;
}

void* file_open_read_all(const char* file_name, struct allocator* allocator)
{
	size_t size;
	void* data;
	struct file* file = file_open(file_name, FILE_MODE_READ);

	size = file_size(file);
	data = ALLOC(allocator, size, 16);
	file_read(file, data, size);

	return data;
}

char* file_open_read_all_text(const char* file_name, struct allocator* allocator)
{
	size_t size;
	char* data;
	struct file* file = file_open(file_name, FILE_MODE_READ);

	size = file_size(file);
	data = (char*)ALLOC(allocator, size+1, 16);
	file_read(file, data, size);
	data[size] = '\0';

	return data;
}
