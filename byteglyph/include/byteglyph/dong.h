#ifndef BYTEGLYPH_DONG_H
#define BYTEGLYPH_DONG_H

#include "common.h"

b8 dong_encode(
    const u8* in_data,
    u8** out_data,
    usize in_size,
    usize* out_size
);

b8 dong_decode(
    const u8* in_data,
    u8** out_data,
    usize in_size,
    usize* out_size
);

#endif // BYTEGLYPH_DONG_H
