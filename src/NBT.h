#ifndef NBT_H
#define NBT_H
#include <iostream>
#include <vector>
#include <cstring>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
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
    //sequences
    void parseTag(uint8_t tagID, Cursor& cursor,std::stringstream&);
    void parseCompound(Cursor&,std::stringstream&);
    void parseList(Cursor&,std::stringstream& ss);
    //types
    uint8_t parseByte(Cursor& cursor);
    uint16_t parseShort(Cursor& cursor);
    int parseInt(Cursor& cursor);
    uint64_t parseLong(Cursor& cursor);
    float parseFloat(Cursor& cursor);
    double parseDouble(Cursor& cursor);
    std::vector<char> parseByteArray(Cursor& cursor);
    std::string parseString(Cursor& cursor);
}
#endif