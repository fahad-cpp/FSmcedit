#ifndef NBT_H
#define NBT_H
#include <vector>
#include <cstring>
#include <sstream>
#include "Cursor.h"
namespace NBT{
    const std::string types[13] = {
        "END",
        "Byte",
        "Short",
        "Int",
        "Long",
        "Float",
        "Double",
        "ByteArray",
        "String",
        "List",
        "Compound",
        "IntArray",
        "LongArray"
    };
    void parseNBT(Cursor& cursor,std::stringstream&);
    void parseTag(uint8_t tagID, Cursor& cursor,std::stringstream&);
    //sequences
    void parseCompound(Cursor&,std::stringstream&);
    void parseList(Cursor&,std::stringstream& ss);
    //types
    uint8_t parseByte(Cursor& cursor);
    uint16_t parseShort(Cursor& cursor);
    int parseInt(Cursor& cursor);
    uint64_t parseLong(Cursor& cursor);
    float parseFloat(Cursor& cursor);
    double parseDouble(Cursor& cursor);
    std::string parseString(Cursor& cursor);
    //arrays
    std::vector<uint8_t> parseByteArray(Cursor& cursor);
    std::vector<int32_t> parseIntArray(Cursor& cursor);
    std::vector<int64_t> parseLongArray(Cursor& cursor);
}
#endif
