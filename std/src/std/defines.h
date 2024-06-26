#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 byte");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 byte");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 byte");

STATIC_ASSERT(sizeof(i8) == 1,  "Expected i8 to be 1 byte");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 byte");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 byte");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 byte");

STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 byte");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 byte");

#ifdef API_EXPORT
#ifdef _MSC_VER
#define API __declspec(dllexport)
#else
#define API
#endif

#elif defined(API_IMPORT)

#ifdef _MSC_VER
#define API __declspec(dllimport)
#else
#define API
#endif

#endif