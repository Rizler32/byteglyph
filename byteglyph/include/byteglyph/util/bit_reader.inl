#ifndef BYTEGLYPH_BIT_READER_INL
#define BYTEGLYPH_BIT_READER_INL

#include "byteglyph/common.h"
#include <stdlib.h>

typedef struct {
    const u8* data;   // input buffer
    usize size;       // dimension (bytes)
    usize pos;        // current byte
    u8    bitpos;     // bit position in the current byte (0–7)
} byteglyph_bitreader_t;

static inline void byteglyph_br_init(byteglyph_bitreader_t* br, const u8* data, usize size) {
    br->data = data;
    br->size = size;
    br->pos = 0;
    br->bitpos = 0;
}

static inline u8 byteglyph_br_read_bit(byteglyph_bitreader_t* br) {
    if (br->pos >= br->size)
        return 0; // EOF safe

    u8 byte = br->data[br->pos];
    u8 bit = (byte >> (7 - br->bitpos)) & 1;

    br->bitpos++;
    if (br->bitpos == 8) {
        br->bitpos = 0;
        br->pos++;
    }

    return bit;
}

static inline u64 byteglyph_br_read_bits(byteglyph_bitreader_t* br, u8 count) {
    u64 value = 0;
    for (u8 i = 0; i < count; i++) {
        value = (value << 1) | byteglyph_br_read_bit(br);
    }
    return value;
}

#define byteglyph_br_read_u8(br)  ((u8) byteglyph_br_read_bits(br, 8))
#define byteglyph_br_read_u16(br) ((u16)byteglyph_br_read_bits(br, 16))
#define byteglyph_br_read_u32(br) ((u32)byteglyph_br_read_bits(br, 32))
#define byteglyph_br_read_u64(br) ((u64)byteglyph_br_read_bits(br, 64))

#define byteglyph_br_read_i8(br)  ((i8) byteglyph_br_read_bits(br, 8))
#define byteglyph_br_read_i16(br) ((i16)byteglyph_br_read_bits(br, 16))
#define byteglyph_br_read_i32(br) ((i32)byteglyph_br_read_bits(br, 32))
#define byteglyph_br_read_i64(br) ((i64)byteglyph_br_read_bits(br, 64))

static inline u32 byteglyph_br_read_varint(byteglyph_bitreader_t* br) {
    u32 result = 0;
    u32 shift = 0;

    while (1) {
        u8 byte = byteglyph_br_read_u8(br);
        result |= (u32)(byte & 0x7F) << shift;

        if ((byte & 0x80) == 0)
            break;

        shift += 7;
    }

    return result;
}

static inline void byteglyph_br_align(byteglyph_bitreader_t* br) {
    if (br->bitpos != 0) {
        br->bitpos = 0;
        br->pos++;
    }
}

#endif // BYTEGLYPH_BIT_READER_INL
