#include "Cursor.h"
uint8_t Cursor::readu8(){
    offset++;
    return ptr[offset];
}

uint16_t Cursor::readu16(){
    uint32_t res =
        ((uint16_t)ptr[offset]) |
        ((uint16_t)ptr[offset + 1]) << 8;
    offset += 2;
    return res;
}

uint32_t Cursor::readu32(){
    uint32_t res = ((uint32_t)ptr[offset]) |
        ((uint32_t)ptr[offset + 1] ) << 8 |
        ((uint32_t)ptr[offset + 2]) << 16 |
        ((uint32_t)ptr[offset + 3]) << 24;
    offset += 4;
    return res;
}