#ifndef BYTEGLYPH_COMMON_H
#define BYTEGLYPH_COMMON_H

#include <atil/common/log.h>

#define BG_TRACE(...) ATIL_TRACE(__VA_ARGS__)
#define BG_DEBUG(...) ATIL_DEBUG(__VA_ARGS__)
#define BG_INFO(...) ATIL_INFO(__VA_ARGS__)
#define BG_WARN(...) ATIL_WARN(__VA_ARGS__)
#define BG_ERROR(...) ATIL_ERROR(__VA_ARGS__)
#define BG_FATAL(...) ATIL_FATAL(__VA_ARGS__)

#define TRUE ATIL_TRUE
#define FALSE ATIL_FALSE

#define BG_FAILURE 1
#define BG_SUCCESS 0

#define BG_IS_SUCCESS(x) ((x) == BG_SUCCESS)
#define BG_IS_FAILURE(x) ((x) != BG_SUCCESS)

#endif // BYTEGLYPH_COMMON_H
