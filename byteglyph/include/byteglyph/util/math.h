#ifndef BYTEGLYPH_UTIL_MATH_H
#define BYTEGLYPH_UTIL_MATH_H

#define MAX(a,b) ({                 \
    __typeof__(a) _a = (a);         \
    __typeof__(b) _b = (b);         \
    _a ^ ((_a ^ _b) & -(_a < _b));  \
})

#define MIN(a,b) ({                 \
    __typeof__(a) _a = (a);         \
    __typeof__(b) _b = (b);         \
    _b ^ ((_a ^ _b) & -(_a < _b));  \
})

#endif // BYTEGLYPH_UTIL_MATH_H
