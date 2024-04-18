#include "file.h"

#include "std/containers/darray.h"
#include <stdio.h>
#include <std/core/logger.h>
#include <std/core/memory.h>

bool file_read_binary(const char *path, BinaryContents *out) {
    FILE *file = fopen(path, "rb");

    if (!file) {
        LOG_ERROR("Unable to open file for reading: %s", path);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    void *buffer = memory_alloc(size);
    u64 result = fread(buffer, size, 1, file);
    fclose(file);

    if (result != 1) {
        LOG_ERROR("Unable to read from file: %s", path);
        return false;
    }

    out->data = buffer;
    out->size = size;

    return true;
}

bool file_read_text(const char *path, const char **out) {
    FILE *file = fopen(path, "r");

    if (!file) {
        LOG_ERROR("Unable to open file for reading: %s", path);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    void *buffer = memory_alloc(size);
    u64 result = fread(buffer, size, 1, file);
    fclose(file);

    if (result != 1) {
        LOG_ERROR("Unable to read from file: %s", path);
        return false;
    }

    *out = buffer;
    return true;
}