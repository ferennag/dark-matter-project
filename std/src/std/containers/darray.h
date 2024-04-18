#pragma once

#include "std/defines.h"

typedef struct darray_header {
    u64 capacity;
    u64 length;
    u64 stride;
} DarrayHeader;

void *_darray_create(u64 length, u64 stride);

void *_darray_reserve(u64 length, u64 stride);

void _darray_destroy(void *array);

void *_darray_resize(void *array);

void *_darray_push(void *array, const void *value_ptr);

void _darray_pop(void *array, void *dest);

void *_darray_pop_at(void *array, u64 index, void *dest);

void *_darray_insert_at(void *array, u64 index, void *value_ptr);

#define DARRAY_DEFAULT_CAPACITY 1
#define DARRAY_RESIZE_FACTOR 2

#define darray_create(type)                                 \
    _darray_create(DARRAY_DEFAULT_CAPACITY, sizeof(type))

#define darray_reserve(type, capacity)      \
    _darray_reserve(capacity, sizeof(type))

#define darray_destroy(array) _darray_destroy(array);

#define darray_push(array, value)            \
    {                                        \
        typeof(value) temp = value;            \
        array = _darray_push(array, &temp);  \
    }

#define darray_pop(array, value_ptr)    \
    _darray_pop(array, value_ptr);

#define darray_insert_at(array, index, value)              \
    {                                               \
        __auto_type temp = value;                   \
        array = _darray_insert_at(array, index, &temp);    \
    }

#define darray_pop_at(array, index, value_ptr)  \
    _darray_pop_at(array, index, value_ptr);

#define darray_clear(array)  \
    (darray_header(array)->length = 0);

#define darray_header(array) \
    ((DarrayHeader *) ((char *)(array) - sizeof(DarrayHeader)))

#define darray_capacity(array) \
    (darray_header(array)->capacity)

#define darray_length(array)  \
    (darray_header(array)->length)

#define darray_stride(array)  \
    (darray_header(array)->stride)

#define darray_length_set(array, len) \
    (darray_header(array)->length = (len))

