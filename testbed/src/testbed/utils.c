#include "testbed/utils.h"
#include <stdio.h>

void print_bytes_ud(const char* name, u8* data, usize size) {
    printf("%s (%u bytes):\n  %u", name, size, data[0]);
    for (int i = 1; i < size; ++i) printf(", %u", data[i]);
    printf("\n");
}
