#ifndef CURSOR_H
#define CURSOR_H
#include <stdint.h>
#include <string>

class Cursor{
    const uint8_t* ptr;
    uint32_t offset;
    public:
    Cursor(const uint8_t* ptr, uint32_t offset = 0){
        this->ptr = ptr;
        this->offset = offset;
    }
    
    //Byte
    uint8_t peeku8();
    uint8_t readu8();
    //LE
    uint16_t readu16();
    uint32_t readu32();
    uint64_t readu64();
    //BE
    uint16_t readBEu16();
    uint32_t readBEu32();
    uint64_t readBEu64();

    std::string readString(uint32_t length);

    uint32_t getOffset();
    const uint8_t* getPtr();
    void skip(uint32_t count);
};
#endif