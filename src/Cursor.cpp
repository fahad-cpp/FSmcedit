#include "Cursor.h"
//Peek little endian unsigned byte
uint8_t Cursor::peeku8() {
    return ptr[offset];
}
//Read little endian unsigned byte
uint8_t Cursor::readu8() {
    uint8_t res = ptr[offset];
    offset++;
    return res;
}
//Read little endian 16 bit unsigned integer
uint16_t Cursor::readu16() {
    uint32_t res =
        ((uint16_t)ptr[offset]) |
        ((uint16_t)ptr[offset + 1]) << 8;
    offset += 2;
    return res;
}
//Read little endian 32 bit unsigned integer
uint32_t Cursor::readu32() {
    uint32_t res = ((uint32_t)ptr[offset]) |
        ((uint32_t)ptr[offset + 1]) << 8 |
        ((uint32_t)ptr[offset + 2]) << 16 |
        ((uint32_t)ptr[offset + 3]) << 24;
    offset += 4;
    return res;
}
//Read little endian 64 bit unsigned integer
uint64_t Cursor::readu64() {
    uint64_t res = ((uint64_t)ptr[offset]) |
        ((uint64_t)ptr[offset + 1]) << 8 |
        ((uint64_t)ptr[offset + 2]) << 16 |
        ((uint64_t)ptr[offset + 3]) << 24 |
        ((uint64_t)ptr[offset + 4]) << 32 |
        ((uint64_t)ptr[offset + 5]) << 40 |
        ((uint64_t)ptr[offset + 6]) << 48 |
        ((uint64_t)ptr[offset + 7]) << 56;
    offset += 8;
    return res;
}

//Read big endian 16 bit unsigned integer
uint16_t Cursor::readBEu16() {
    uint32_t res =
        ((uint16_t)ptr[offset]) << 8 |
        ((uint16_t)ptr[offset + 1]);
    offset += 2;
    return res;
}
//Read big endian 32 bit unsigned integer
uint32_t Cursor::readBEu32() {
    uint32_t res = ((uint32_t)ptr[offset]) << 24 |
        ((uint32_t)ptr[offset + 1]) << 16 |
        ((uint32_t)ptr[offset + 2]) << 8 |
        ((uint32_t)ptr[offset + 3]);
    offset += 4;
    return res;
}
//Read big endian 64 bit unsigned integer
uint64_t Cursor::readBEu64() {
    uint64_t res = ((uint64_t)ptr[offset]) << 56 |
        ((uint64_t)ptr[offset + 1]) << 48 |
        ((uint64_t)ptr[offset + 2]) << 40 |
        ((uint64_t)ptr[offset + 3]) << 32 |
        ((uint64_t)ptr[offset + 4]) << 24 |
        ((uint64_t)ptr[offset + 5]) << 16 |
        ((uint64_t)ptr[offset + 6]) << 8 |
        ((uint64_t)ptr[offset + 7]);
    offset += 8;
    return res;
}

//Read string
std::string Cursor::readString(uint32_t length) {
    std::string str = "";
    for (int i = 0;i < length;i++) {
        str += (char)ptr[offset];
        offset++;
    }

    return str;
}

uint32_t Cursor::getOffset() {
    return offset;
}

const uint8_t* Cursor::getPtr() {
    return (ptr + offset);
}
//skip `count` bytes in the cursor
void Cursor::skip(uint32_t count) {
    offset += count;
}