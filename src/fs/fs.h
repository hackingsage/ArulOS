#ifndef FS_H
#define FS_H

#include <stdint.h>

#define FS_MAX_FILES 64
#define FS_BLOCK_SIZE 512
#define FS_MAX_BLOCKS 1024
#define FS_FILENAME_MAX 16

typedef struct {
    char name[FS_FILENAME_MAX];
    uint32_t size;
    uint16_t start_block;
    uint8_t used;
} FileEntry;

void fs_init();
int fs_create(const char* name);
int fs_write(const char* name, const char* data, uint32_t size);
const char* fs_read(const char* name);
void fs_list_files();
int fs_get_file_count();
const char* fs_get_filename(int index);
int fs_delete(const char* name);

#endif
