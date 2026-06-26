#ifndef BYTEGLYPH_BIT_WRITER_INL
#define BYTEGLYPH_BIT_WRITER_INL

#include "byteglyph/common.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    u8* data;
    usize capacity;

    usize size;

    usize cursor_byte;
    u8 cursor_bit;
} byteglyph_bitwriter_t;

static inline void byteglyph_bw_grow(byteglyph_bitwriter_t* bw)
{
    usize newcap =
        (bw->capacity == 0)
            ? 64
            : bw->capacity * 2;

    u8* newdata =
        (u8*)realloc(
            bw->data,
            newcap);

    if (!newdata)
        return;

    bw->data = newdata;
    bw->capacity = newcap;
}

static inline void byteglyph_bw_reserve_byte(
    byteglyph_bitwriter_t* bw,
    usize byte_index)
{
    while (byte_index >= bw->capacity)
        byteglyph_bw_grow(bw);

    if (byte_index >= bw->size) {
        memset(
            bw->data + bw->size,
            0,
            byte_index - bw->size + 1);

        bw->size =
            byte_index + 1;
    }
}

static inline void byteglyph_bw_init(
    byteglyph_bitwriter_t* bw)
{
    bw->data = NULL;
    bw->capacity = 0;

    bw->size = 0;

    bw->cursor_byte = 0;
    bw->cursor_bit = 0;
}

static inline void byteglyph_bw_seek(
    byteglyph_bitwriter_t* bw,
    usize byte_pos,
    u8 bit_pos)
{
    if (bit_pos > 7)
        bit_pos = 7;

    byteglyph_bw_reserve_byte(
        bw,
        byte_pos);

    bw->cursor_byte = byte_pos;
    bw->cursor_bit = bit_pos;
}

static inline void byteglyph_bw_tell(
    const byteglyph_bitwriter_t* bw,
    usize* byte_pos,
    u8* bit_pos)
{
    if (byte_pos)
        *byte_pos = bw->cursor_byte;

    if (bit_pos)
        *bit_pos = bw->cursor_bit;
}

static inline void byteglyph_bw_write_bit(
    byteglyph_bitwriter_t* bw,
    u8 bit)
{
    byteglyph_bw_reserve_byte(
        bw,
        bw->cursor_byte);

    if (bit & 1) {
        bw->data[bw->cursor_byte] |=
            (1u << (7 - bw->cursor_bit));
    }
    else {
        bw->data[bw->cursor_byte] &=
            ~(1u << (7 - bw->cursor_bit));
    }

    bw->cursor_bit++;

    if (bw->cursor_bit == 8) {
        bw->cursor_bit = 0;
        bw->cursor_byte++;
    }

    if (bw->cursor_byte >= bw->size) {
        bw->size =
            bw->cursor_byte + 1;
    }
}

static inline void byteglyph_bw_write_bits(
    byteglyph_bitwriter_t* bw,
    u64 value,
    u8 count)
{
    for (i32 i = (i32)count - 1;
         i >= 0;
         --i)
    {
        byteglyph_bw_write_bit(
            bw,
            (u8)((value >> i) & 1));
    }
}

#define byteglyph_bw_write_u8(bw, value) \
    byteglyph_bw_write_bits(bw, (u8)(value), 8)

#define byteglyph_bw_write_u16(bw, value) \
    byteglyph_bw_write_bits(bw, (u16)(value), 16)

#define byteglyph_bw_write_u32(bw, value) \
    byteglyph_bw_write_bits(bw, (u32)(value), 32)

#define byteglyph_bw_write_u64(bw, value) \
    byteglyph_bw_write_bits(bw, (u64)(value), 64)

#define byteglyph_bw_write_i8(bw, value) \
    byteglyph_bw_write_bits(bw, (u8)(value), 8)

#define byteglyph_bw_write_i16(bw, value) \
    byteglyph_bw_write_bits(bw, (u16)(value), 16)

#define byteglyph_bw_write_i32(bw, value) \
    byteglyph_bw_write_bits(bw, (u32)(value), 32)

#define byteglyph_bw_write_i64(bw, value) \
    byteglyph_bw_write_bits(bw, (u64)(value), 64)

static inline void byteglyph_bw_write_varint(
    byteglyph_bitwriter_t* bw,
    u32 value)
{
    while (value >= 0x80) {
        u8 byte =
            (value & 0x7F) | 0x80;

        byteglyph_bw_write_u8(
            bw,
            byte);

        value >>= 7;
    }

    byteglyph_bw_write_u8(
        bw,
        (u8)value);
}

static inline void byteglyph_bw_flush(
    byteglyph_bitwriter_t* bw)
{
    if (bw->cursor_bit != 0) {
        bw->cursor_byte++;
        bw->cursor_bit = 0;
    }

    if (bw->cursor_byte > bw->size)
        bw->size = bw->cursor_byte;
}

static inline void byteglyph_bw_free(
    byteglyph_bitwriter_t* bw)
{
    free(bw->data);

    bw->data = NULL;
    bw->capacity = 0;

    bw->size = 0;

    bw->cursor_byte = 0;
    bw->cursor_bit = 0;
}

#endif
