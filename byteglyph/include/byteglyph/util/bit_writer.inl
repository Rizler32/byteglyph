#ifndef BYTEGLYPH_BIT_WRITER_INL
#define BYTEGLYPH_BIT_WRITER_INL

#include "byteglyph/common.h"
#include <stdlib.h>

typedef struct {
    u8*   data;       // buffer
    usize capacity;   // allocated capacity
    usize size;       // used bytes
    u8    bitpos;     // bit position in the current byte (0–7)
} byteglyph_bitwriter_t;

static inline void byteglyph_bw_grow(byteglyph_bitwriter_t* bw) {
    usize newcap = (bw->capacity == 0) ? 64 : bw->capacity * 2;
    bw->data = (u8*)realloc(bw->data, newcap);
    bw->capacity = newcap;
}

static inline void byteglyph_bw_init(byteglyph_bitwriter_t* bw) {
    bw->data = NULL;
    bw->capacity = 0;
    bw->size = 0;
    bw->bitpos = 0;
}

static inline void byteglyph_bw_write_bit(byteglyph_bitwriter_t* bw, u8 bit) {
    if (bw->size == bw->capacity)
        byteglyph_bw_grow(bw);

    if (bw->bitpos == 0) {
        bw->data[bw->size] = 0;
    }

    if (bit & 1)
        bw->data[bw->size] |= (1u << (7 - bw->bitpos));

    bw->bitpos++;

    if (bw->bitpos == 8) {
        bw->bitpos = 0;
        bw->size++;
    }
}

static inline void byteglyph_bw_write_bits(byteglyph_bitwriter_t* bw, u32 value, u8 count) {
    for (int i = count - 1; i >= 0; --i) {
        byteglyph_bw_write_bit(bw, (value >> i) & 1);
    }
}

#define byteglyph_bw_write_u8(bw, value)  byteglyph_bw_write_bits(bw, (u32)(value), 8)
#define byteglyph_bw_write_u16(bw, value) byteglyph_bw_write_bits(bw, (u32)(value), 16)
#define byteglyph_bw_write_u32(bw, value) byteglyph_bw_write_bits(bw, (u32)(value), 32)
#define byteglyph_bw_write_u64(bw, value) byteglyph_bw_write_bits(bw, (u64)(value), 64)

#define byteglyph_bw_write_i8(bw, value)  byteglyph_bw_write_bits(bw, (u32)(value), 8)
#define byteglyph_bw_write_i16(bw, value) byteglyph_bw_write_bits(bw, (u32)(value), 16)
#define byteglyph_bw_write_i32(bw, value) byteglyph_bw_write_bits(bw, (u32)(value), 32)
#define byteglyph_bw_write_i64(bw, value) byteglyph_bw_write_bits(bw, (u64)(value), 64)

static inline void byteglyph_bw_write_varint(byteglyph_bitwriter_t* bw, u32 value) {
    while (value >= 0x80) {
        u8 byte = (value & 0x7F) | 0x80; // 7 bit + continuation bit
        byteglyph_bw_write_u8(bw, byte);
        value >>= 7;
    }
    byteglyph_bw_write_u8(bw, (u8)value); // ultimo byte, MSB=0
}

static inline void byteglyph_bw_flush(byteglyph_bitwriter_t* bw) {
    if (bw->bitpos != 0) {
        bw->bitpos = 0;
        bw->size++;
    }
}

static inline void byteglyph_bw_free(byteglyph_bitwriter_t* bw) {
    free(bw->data);
    bw->data = NULL;
    bw->capacity = 0;
    bw->size = 0;
    bw->bitpos = 0;
}

#endif // BYTEGLYPH_BIT_WRITER_INL
