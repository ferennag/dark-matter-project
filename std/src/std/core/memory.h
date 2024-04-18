#pragma once

#include "std/defines.h"

void *memory_alloc(u64 size);

void memory_free(void *block);

void *memory_zero(void *block, u64 size);

void *memory_copy(void *dest, const void *src, u64 size);

void *memory_set(void *dest, i32 value, u64 size);