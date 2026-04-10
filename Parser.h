#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <vector>
#include "Cursor.h"
const static std::string types[13] = {
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
uint32_t parseNBT(const uint8_t* data);
uint32_t parseCompound(const uint8_t* data);
uint32_t getItemSize(uint8_t tag);
void parseList(Cursor&);
void parseTag(uint8_t tagID,Cursor& cursor);
uint8_t parseByte(Cursor& cursor);
uint16_t parseShort(Cursor& cursor);
uint32_t parseInt(Cursor& cursor);
uint64_t parseLong(Cursor& cursor);
float parseFloat(Cursor& cursor);
double parseDouble(Cursor& cursor);
std::vector<char> parseByteArray(Cursor& cursor);
std::string parseString(Cursor& cursor);
#endif