#include <sys/types.h>
#include <dirent.h>

#include <string.h>
#include <stdio.h>

#include <core/allocator.h>
#include <core/assert.h>
#include <core/file.h>

#include "memsearch.h"

#define ALLOC_ONE(allocator, type) ALLOC(allocator, sizeof(type), alignof(type))
#define ALLOC_N(allocator, n, type) ALLOC(allocator, (n)*sizeof(type), alignof(type))

struct memfile
{
	uintptr_t filename_len;
	uintptr_t content_len;
	char* filename;
	char* content;
};

enum memtype
{
	MEMTYPE_DIR,
	MEMTYPE_FILE,
};

struct meminfo
{
	enum memtype type;
	char* name;
};

struct memsearch
{
	struct allocator* allocator;
	int capacity_files;
	int capacity_info;
	int num_files;
	int num_info;
	struct memfile* files;
	struct meminfo* info;
};

struct memsearch* memsearch_create(struct allocator* allocator)
{
	struct memsearch* memsearch = ALLOC_ONE(allocator, struct memsearch);
	memsearch->allocator = allocator;
	memsearch->capacity_files = 32;
	memsearch->capacity_info = 32;
	memsearch->num_files = 0;
	memsearch->num_info = 0;
	memsearch->files = ALLOC_N(allocator, memsearch->capacity_files, struct memfile);
	memsearch->info = ALLOC_N(allocator, memsearch->capacity_info, struct meminfo);
	return memsearch;
}

void memsearch_destroy(struct memsearch* memsearch)
{
	int i = 0;
	struct allocator* allocator = memsearch->allocator;
	struct memfile* files = memsearch->files;
	struct meminfo* info = memsearch->info;
	for(i = 0; i < memsearch->num_files; ++i)
	{
		FREE(allocator, files[i].filename);
		FREE(allocator, files[i].content);
	}
	for(i = 0; i < memsearch->num_info; ++i)
	{
		FREE(allocator, info[i].name);
	}
	FREE(allocator, memsearch->files);
	FREE(allocator, memsearch->info);
	FREE(allocator, memsearch);
}

static void memsearch_add_file_internal(struct memsearch* memsearch, const char* filename, memsearch_add_callback_t callback, void* callback_context)
{
	char* content = file_open_read_all_text(filename, memsearch->allocator);
	uint id = memsearch->num_files;

	if(memsearch->num_files == memsearch->capacity_files)
	{
		memsearch->capacity_files *= 2;
		memsearch->files = REALLOC(memsearch->allocator, memsearch->files, sizeof(struct memfile)*memsearch->capacity_files, alignof(struct memfile));
	}
	memsearch->num_files += 1;

	memsearch->files[id].filename_len = strlen(filename);
	memsearch->files[id].content_len = strlen(content);
	memsearch->files[id].filename = ALLOC_N(memsearch->allocator, memsearch->files[id].filename_len + 1, char);
	strncpy(memsearch->files[id].filename, filename, memsearch->files[id].filename_len + 1);
	memsearch->files[id].content = content;

	callback(callback_context, filename);
}

static void memsearch_add_info_internal(struct memsearch* memsearch, enum memtype type, const char* name)
{
	uint id = memsearch->num_info;
	uint len = strlen(name);
	if(memsearch->num_info == memsearch->capacity_info)
	{
		memsearch->capacity_info *= 2;
		memsearch->info = REALLOC(memsearch->allocator, memsearch->info, sizeof(struct meminfo)*memsearch->capacity_info, alignof(struct meminfo));
	}
	memsearch->num_info += 1;
	memsearch->info[id].type = type;
	memsearch->info[id].name = ALLOC_N(memsearch->allocator, len + 1, char);
	strncpy(memsearch->info[id].name, name, len + 1);
}

void memsearch_add_file(struct memsearch* memsearch, const char* filename, memsearch_add_callback_t callback, void* callback_context)
{
	memsearch_add_file_internal(memsearch, filename, callback, callback_context);
	memsearch_add_info_internal(memsearch, MEMTYPE_FILE, filename);
}

#define MAX_PATH 2048

void memsearch_add_dir(struct memsearch* memsearch, const char* dirname, const char** exts, uint num_exts, memsearch_add_callback_t callback, void* callback_context)
{
	DIR* dirp = opendir(dirname);
	struct dirent* dp;
	char buf[MAX_PATH];
	int i;
	while((dp = readdir(dirp)) != NULL)
	{
		snprintf(buf, MAX_PATH, "%s/%s", dirname, dp->d_name);
		switch(dp->d_type)
		{
			case DT_DIR:
				if(dp->d_name[0] != '.')
				{
					memsearch_add_dir(memsearch, buf, exts, num_exts, callback, callback_context);
				}
				break;
			case DT_REG:
				for(i = 0; i < num_exts; ++i)
				{
					int len = strlen(exts[i]);
					int namelen = strlen(dp->d_name);
					if(strcmp(dp->d_name + namelen - len, exts[i]) == 0)
					{
						memsearch_add_file_internal(memsearch, buf, callback, callback_context);
					}
				}
			default:
				break;
		}
	}
	closedir(dirp);
	memsearch_add_info_internal(memsearch, MEMTYPE_DIR, dirname);
}

void memsearch_find(const struct memsearch* memsearch, const char* cmp, memsearch_find_callback_t callback, void* callback_context)
{
	int i = 0, num_files = memsearch->num_files;
	const struct memfile* files = memsearch->files;
	int cmp_len = strlen(cmp);
	for(i = 0; i < num_files; ++i)
	{
		int col = 1;
		int line = 1;
		const char* ptr = files[i].content;
		const char* line_start = ptr;
		while(*ptr != '\0')
		{
			if(strncmp(ptr, cmp, cmp_len) == 0)
			{
				int line_len = strcspn(line_start, "\n\r");
				if(callback)
					callback(callback_context, files[i].filename, line_start, line, col);
				else
					printf("%s(%d): %.*s\n", files[i].filename, line, line_len, line_start);
				col = line_len;
				ptr = line_start + line_len;
			}
			else if(*ptr == '\n')
			{
				++line;
				col = 1;
				++ptr;
				line_start = ptr;
			}
			else
			{
				++col;
				++ptr;
			}
		}
	}
}
