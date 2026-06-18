#ifndef BYTEGLYPH_ZIGZAG_INL
#define BYTEGLYPH_ZIGZAG_INL

#include "byteglyph/common.h"

static inline u8 zigzag8(i8 value) {
    return (u8)((((u8)value) << 1) ^ (u8)(value >> 7));
}

static inline i8 unzigzag8(u8 value) {
    return (i8)((value >> 1) ^ (u8)-(i8)(value & 1));
}

static inline u16 zigzag16(i16 value) {
    return (u16)((((u16)value) << 1) ^ (u16)(value >> 15));
}

static inline i16 unzigzag16(u16 value) {
    return (i16)((value >> 1) ^ (u16)-(i16)(value & 1));
}

static inline u32 zigzag32(i32 value) {
    return (u32)((((u32)value) << 1) ^ (u32)(value >> 31));
}

static inline i32 unzigzag32(u32 value) {
    return (i32)((value >> 1) ^ (u32)-(i32)(value & 1));
}

static inline u64 zigzag64(i64 value) {
    return (u64)((((u64)value) << 1) ^ (u64)(value >> 63));
}

static inline i64 unzigzag64(u64 value) {
    return (i64)((value >> 1) ^ (u64)-(i64)(value & 1));
}

#define zigzag(v) _Generic((v), \
    i8  : zigzag8,              \
    i16 : zigzag16,             \
    i32 : zigzag32,             \
    i64 : zigzag64              \
)(v)

#define unzigzag(v) _Generic((v), \
    u8  : unzigzag8,              \
    u16 : unzigzag16,             \
    u32 : unzigzag32,             \
    u64 : unzigzag64              \
)(v)

#endif // BYTEGLYPH_ZIGZAG_INL
