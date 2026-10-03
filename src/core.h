#ifndef CORE_H
#define CORE_H

#include <stdint.h>

#define PRINT(...) { printf(__VA_ARGS__); printf("\n"); }
#define EXIT(...) { PRINT(__VA_ARGS__); exit(1); }

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef float    f32;
typedef double   f64;
typedef char     c8;

#endif
