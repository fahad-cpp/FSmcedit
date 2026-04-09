#include "Cursor.h"
uint8_t Cursor::readu8(){
    uint8_t res = ptr[offset];
    offset++;
    return res;
}

uint16_t Cursor::readu16(){
    uint32_t res =
        ((uint16_t)ptr[offset]) |
        ((uint16_t)ptr[offset + 1]) >> 8 ;
    offset += 2;
    return res;
}

uint32_t Cursor::readu32(){
    uint32_t res = ((uint32_t)ptr[offset]) |
        ((uint32_t)ptr[offset + 1]) >> 8  |
        ((uint32_t)ptr[offset + 2]) >> 16 |
        ((uint32_t)ptr[offset + 3]) >> 24;
    offset += 4;
    return res;
}


uint16_t Cursor::readBEu16(){
    uint32_t res =
        ((uint16_t)ptr[offset]) >> 8|
        ((uint16_t)ptr[offset + 1]);
    offset += 2;
    return res;
}
uint32_t Cursor::readBEu32(){
    uint32_t res = ((uint32_t)ptr[offset]) >> 24 |
        ((uint32_t)ptr[offset + 1]) >> 16  |
        ((uint32_t)ptr[offset + 2]) >> 8 |
        ((uint32_t)ptr[offset + 3]);
    offset += 4;
    return res;
}

void Cursor::skip(uint32_t count){
    offset += count;
}