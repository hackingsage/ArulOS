#include "fs.h"
#include "string/string.h"
#include "screen/screen.h"

static uint8_t fs_disk[FS_BLOCK_SIZE * FS_MAX_BLOCKS];
static uint8_t block_bitmap[FS_MAX_BLOCKS];
static FileEntry file_table[FS_MAX_FILES];

void fs_init() {
    memset(fs_disk, 0, sizeof(fs_disk));
    memset(block_bitmap, 0, sizeof(block_bitmap));
    memset(file_table, 0, sizeof(file_table));
    print("[ArulFS] Filesystem initialized.\n");
}

int find_free_block() {
    for (int i = 0; i < FS_MAX_BLOCKS; i++) {
        if (!block_bitmap[i]) {
            block_bitmap[i] = 1;
            return i;
        }
    }
    return -1;
}

int fs_create(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (!file_table[i].used) {
            strncpy(file_table[i].name, name, FS_FILENAME_MAX);
            file_table[i].size = 0;
            file_table[i].start_block = find_free_block();
            file_table[i].used = 1;
            return 1;
        }
    }
    return 0;
}

int fs_write(const char* name, const char* data, uint32_t size) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (file_table[i].used && strcmp(file_table[i].name, name) == 0) {
            if (size > FS_BLOCK_SIZE) return 0;
            memcpy(&fs_disk[file_table[i].start_block * FS_BLOCK_SIZE], data, size);
            file_table[i].size = size;
            return 1;
        }
    }
    return 0;
}

const char* fs_read(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (file_table[i].used && strcmp(file_table[i].name, name) == 0) {
            return (const char*)&fs_disk[file_table[i].start_block * FS_BLOCK_SIZE];
        }
    }
    return 0;
}

void fs_list_files() {
    print("Files:\n");
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (file_table[i].used) {
            print("- ");
            print(file_table[i].name);
            print(" (");
            print_int(file_table[i].size);
            print(" bytes)\n");
        }
    }
}

int fs_get_file_count() {
    int count = 0;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (file_table[i].used) count++;
    }
    return count;
}

const char* fs_get_filename(int index) {
    int seen = 0;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (file_table[i].used) {
            if (seen == index) return file_table[i].name;
            seen++;
        }
    }
    return 0;
}

int fs_delete(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (file_table[i].used && strcmp(file_table[i].name, name) == 0) {
            // Mark block as free
            block_bitmap[file_table[i].start_block] = 0;
            // Clear file table entry
            file_table[i].used = 0;
            file_table[i].name[0] = '\0';
            file_table[i].size = 0;
            file_table[i].start_block = 0;
            return 1; // Success
        }
    }
    return 0; // Not found
}
