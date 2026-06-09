#include "rift/bridge.h"
#include <stdio.h>
#include <string.h>

/*
 * Hex encoding with RB-AVL tree (from R_open/ropen.c)
 * Conjugate operation: 0xF ^ value
 */

uint8_t rift_conjugate(uint8_t value) {
    return 0x0F ^ value;
}

int rift_hex_encode(const void *data, size_t size, char *out, size_t out_size) {
    if (!data || !out || size == 0) return -1;

    const unsigned char *bytes = (const unsigned char *)data;
    size_t required = size * 3 + 1;
    if (out_size < required) return -1;

    size_t pos = 0;
    for (size_t i = 0; i < size; i++) {
        pos += snprintf(out + pos, out_size - pos, "%02x ", bytes[i]);
    }

    if (pos > 0) out[pos - 1] = '\0'; /* trim trailing space */
    return 0;
}
