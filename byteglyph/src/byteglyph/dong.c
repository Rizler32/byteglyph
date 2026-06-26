#include "byteglyph/dong.h"
#include "byteglyph/util/bit_writer.inl"
#include "byteglyph/util/bit_reader.inl"
#include "byteglyph/util/math.h"

#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE_BITS 5
#define MIN_BLOCK_SIZE 2
#define MAX_BLOCK_SIZE (MIN_BLOCK_SIZE + (1u << BLOCK_SIZE_BITS) - 1)

#define BIT_WIDTH_BITS 3
#define MIN_BIT_WIDTH 1
#define MAX_BIT_WIDTH (MIN_BIT_WIDTH + (1u << BIT_WIDTH_BITS) - 1)

static inline u8 calc_bitwidth(u8 v) {
    return v ? (u8)(32u - __builtin_clz((u32)v)) : 1u;
}

static inline u8 next_block_bitwidth(const u8* in, usize in_size, uindex offset) {
    u8 b = 1;
    for (uindex i = offset; i < MIN(in_size, offset + MIN_BLOCK_SIZE); ++i) {
        u8 c = calc_bitwidth(in[i]);
        if (c > b) b = c;
    }
    return b;
}

static inline void emit_block(
    byteglyph_bitwriter_t* w,
    const u8* in,
    uindex offset,
    usize block_size,
    u8 bitwidth)
{
    byteglyph_bw_write_bits(w, bitwidth - MIN_BIT_WIDTH, BIT_WIDTH_BITS);
    byteglyph_bw_write_bits(w, block_size - MIN_BLOCK_SIZE, BLOCK_SIZE_BITS);

    for (uindex i = 0; i < block_size; ++i) {
        byteglyph_bw_write_bits(w, in[offset + i], bitwidth);
    }
}

b8 dong_encode(
    const u8* in,
    u8** out,
    usize in_size,
    usize* out_size)
{
    if (in_size == 0) {
        *out = NULL;
        *out_size = 0;
        return TRUE;
    }

    byteglyph_bitwriter_t w;
    byteglyph_bw_init(&w);

    byteglyph_bw_write_bits(&w, 0, 3); // reserve space to write significant bits of the last byte

    uindex offset = 0;
    u8 b_bitwidth = calc_bitwidth(in[0]);

    for (usize i = 1; i < in_size; ++i) {
        usize block_size = i - offset + 1;
        u8 c_bitwidth = calc_bitwidth(in[i]);

        if (block_size <= MIN_BLOCK_SIZE) {
            if (c_bitwidth > b_bitwidth)
                b_bitwidth = c_bitwidth;
            continue;
        }

        if (block_size >= MAX_BLOCK_SIZE) {
            emit_block(&w, in, offset, MAX_BLOCK_SIZE, b_bitwidth);
            offset += MAX_BLOCK_SIZE;

            if (offset < in_size)
                b_bitwidth = calc_bitwidth(in[offset]);

            i = offset;
            continue;
        }

        if (c_bitwidth != b_bitwidth) {
            u8 va = MAX(b_bitwidth, c_bitwidth);
            u8 vb = next_block_bitwidth(in, in_size, i);

            i32 wa = va * block_size - b_bitwidth * (block_size - 1);
            i32 wb = BIT_WIDTH_BITS + BLOCK_SIZE_BITS + vb;

            if (wa > wb) {
                block_size--;
                emit_block(&w, in, offset, block_size, b_bitwidth);

                offset = i;
                b_bitwidth = vb;
            } else {
                b_bitwidth = va;
            }
        }
    }

    usize final_size = in_size - offset;
    emit_block(&w, in, offset, final_size, b_bitwidth);

    // sb stored as 3 bits
    u8 sb = (w.cursor_bit == 0) ? 8 : w.cursor_bit;
    byteglyph_bw_seek(&w, 0, 0);
    byteglyph_bw_write_bits(&w, sb, 3);

    byteglyph_bw_flush(&w);

    *out = w.data;
    *out_size = w.size;

    return TRUE;
}

b8 dong_decode(
    const u8* in,
    u8** out,
    usize in_size,
    usize* out_size)
{
    if (in_size == 0) {
        *out = NULL;
        *out_size = 0;
        return TRUE;
    }

    byteglyph_bitreader_t r;
    byteglyph_br_init(&r, in, in_size);

    usize capacity = in_size * 8;
    u8* buf = malloc(capacity);
    if (!buf) return FALSE;

    usize count = 0;

    if (in_size == 0) {
        free(buf);
        return FALSE;
    }

    u8 sb = byteglyph_br_read_bits(&r, 3);
    if (sb == 0) sb = 8;

    while (byteglyph_br_bits_left(&r) > 0) {

        usize bits_left = byteglyph_br_bits_left(&r);
        if (bits_left < BIT_WIDTH_BITS + BLOCK_SIZE_BITS)
            break;

        u8 bitwidth = MIN_BIT_WIDTH +
            byteglyph_br_read_bits(&r, BIT_WIDTH_BITS);

        u32 block_size = MIN_BLOCK_SIZE +
            byteglyph_br_read_bits(&r, BLOCK_SIZE_BITS);

        usize needed = (usize)bitwidth * block_size;

        if (byteglyph_br_bits_left(&r) < needed)
            break;

        for (u32 i = 0; i < block_size; ++i) {
            u32 v = byteglyph_br_read_bits(&r, bitwidth);

            if (count >= capacity) {
                capacity *= 2;
                u8* tmp = realloc(buf, capacity);
                if (!tmp) {
                    free(buf);
                    return FALSE;
                }
                buf = tmp;
            }

            buf[count++] = (u8)v;
        }

        if (r.pos >= in_size - 1 &&
            r.bitpos >= sb)
            break;
    }

    *out = buf;
    *out_size = count;

    return TRUE;
}
