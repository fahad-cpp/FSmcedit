#include <stdint.h>

class Cursor{
    const uint8_t* ptr;
    uint32_t offset;
    public:
    Cursor(const uint8_t* ptr, uint32_t offset = 0){
        this->ptr = ptr;
        this->offset = offset;
    }

    uint8_t readu8();
    uint16_t readu16();
    uint32_t readu32();
};