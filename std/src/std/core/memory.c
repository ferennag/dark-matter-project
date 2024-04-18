#include "memory.h"

#include <stdlib.h>
#include <string.h>

void *memory_alloc(u64 size) {
    return calloc(1, size);
}

void memory_free(void *block) {
    free(block);
}

void *memory_zero(void *block, u64 size) {
    return memset(block, 0, size);
}

void *memory_copy(void *dest, const void *src, u64 size) {
    return memcpy(dest, src, size);
}

void *memory_set(void *dest, i32 value, u64 size) {
    return memset(dest, value, size);
}