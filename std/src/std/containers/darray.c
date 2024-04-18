#include "darray.h"
#include "std/core/memory.h"
#include "std/core/logger.h"

void *_darray_create(u64 length, u64 stride) {
    u64 header_size = sizeof(DarrayHeader);
    u64 array_size = length * stride;
    DarrayHeader *array = memory_alloc(header_size + array_size);
    memory_set(array, 0, header_size + array_size);
    array->capacity = length;
    array->length = 0;
    array->stride = stride;
    return (void *) (array + 1);
}

void *_darray_reserve(u64 length, u64 stride) {
    void *array = _darray_create(length, stride);
    DarrayHeader *header = darray_header(array);
    header->length = header->capacity;
    return array;
}

void _darray_destroy(void *array) {
    DarrayHeader *header = darray_header(array);
    memory_free(header);
}

void *_darray_resize(void *array) {
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);
    void *temp = _darray_create((DARRAY_RESIZE_FACTOR * darray_capacity(array)), stride);
    memory_copy(temp, array, length * stride);

    darray_length_set(array, length);
    _darray_destroy(array);
    return temp;
}

void *_darray_push(void *array, const void *value_ptr) {
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);

    if (length >= darray_capacity(array)) {
        array = _darray_resize(array);
    }

    u8 *addr = (u8 *) array;
    addr += length * stride;
    memory_copy((void *) addr, value_ptr, stride);
    darray_length_set(array, length + 1);
    return array;
}

void _darray_pop(void *array, void *dest) {
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);

    u64 addr = (u64) array;
    addr += (length - 1) * stride;
    memory_copy(dest, (void *) addr, stride);
    darray_length_set(array, length - 1);
}

void *_darray_pop_at(void *array, u64 index, void *dest) {
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);

    if (index >= length) {
        LOG_ERROR("Index outside of bounds of array! Length: %i, index: %i", length, index);
        return array;
    }

    u64 addr = (u64) array;
    memory_copy(dest, (void *) (addr + (index * stride)), stride);

    if (index != length - 1) {
        memory_copy(
                (void *) (addr + (index * stride)),
                (void *) (addr + ((index + 1) * stride)),
                stride * (length - index));
    }

    darray_length_set(array, length - 1);
    return array;
}

void *_darray_insert_at(void *array, u64 index, void *value_ptr) {
    u64 length = darray_length(array);
    u64 stride = darray_stride(array);

    if (index > length) {
        LOG_ERROR("Index outside of bounds of array! Length: %i, index: %i", length, index);
        return array;
    }

    if (length >= darray_capacity(array)) {
        array = _darray_resize(array);
    }

    u64 addr = (u64) array;

    if (index != length) {
        memory_copy(
                (void *) (addr + ((index + 1) * stride)),
                (void *) (addr + (index * stride)),
                stride * (length - index));
    }

    memory_copy((void *) (addr + (index * stride)), value_ptr, stride);

    darray_length_set(array, length + 1);
    return array;
}