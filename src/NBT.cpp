#include "NBT.h"
/*Parse NBT with the following structure:
    -1 byte : tagID
    -2 byte : length
    -length byte string : Name of the NBT
    -payload depending on the tagID
*/
void NBT::parseNBT(Cursor& cursor,std::stringstream& ss) {
    uint8_t tagTypeID = cursor.readu8();
    if (tagTypeID < 0 || tagTypeID > 12) {
        std::cerr << "Invalid type : " << (int)tagTypeID << "\n";
        return;
    }
    uint16_t nameLength = cursor.readu16();
    const std::string& name = cursor.readString(nameLength);
    
    if(nameLength != 0){
        ss << "\"" << name << "\"" << ":";
    }

    parseTag(tagTypeID, cursor,ss);
}
//Parse only payload of the NBT
void NBT::parseTag(uint8_t tagID, Cursor& cursor,std::stringstream& ss) {
    //ss << "(" << types[tagID] << "):";
    switch (tagID) {
        case 1: {
            ss << (int)parseByte(cursor);
        }break;
        case 2: {
            ss << (int)parseShort(cursor);
        }break;
        case 3: {
            ss << parseInt(cursor);
        }break;
        case 4: {
            ss << parseLong(cursor);
        }break;
        case 5: {
            ss << std::fixed << parseFloat(cursor);
        }break;
        case 6: {
            ss << std::fixed << parseDouble(cursor);
        }break;
        case 7: {
            std::vector<char> byteArray = parseByteArray(cursor);
        }break;
        case 8: {
            ss << "\"" << parseString(cursor) << "\"";
        }break;
        case 9: {
            parseList(cursor,ss);
        }break;
        case 10: {
            parseCompound(cursor,ss);
        }break;
        case 11: {
            //Unhandled
            uint32_t size = (int)cursor.readu32();
            ss << "\"UnhandledIntArray\"";
            std::cerr << "IntArray\n";
            //ss << "size:" << size << "\n";
            cursor.skip(size * 4);
        }break;
        case 12: {
            //Unhandled
            uint32_t size = (int)cursor.readu32();
            ss << "\"UnhandledLongArray\"";
            std::cerr << "LongArray\n";
            //ss << "size:" << size << "\n";
            cursor.skip(size * 8);
        }break;
        default: {
            std::cerr << "Invalid TAG\n";
        }break;
    }
}
//Parse a compund of NBTs
void NBT::parseCompound(Cursor& cursor,std::stringstream& ss) {
    ss << "{";
    while (cursor.peeku8() != 0) {
        parseNBT(cursor,ss);
        if(cursor.peeku8() != 0){
            ss << ",";
        }
    }
    ss << "}";
    cursor.skip(1);
}
//Parse a list of NBTs
void NBT::parseList(Cursor& cursor,std::stringstream& ss) {
    uint8_t listTagID = cursor.readu8();
    uint32_t size = cursor.readu32();
    if(listTagID < 1 || listTagID > 12){
        ss << "[]";
        return;
    };
    ss << '[';
    for (int i = 0;i < size;i++) {
        parseTag(listTagID, cursor,ss);
        if(i != (size-1))ss <<',';
    }
    ss << ']';
}

uint8_t NBT::parseByte(Cursor& cursor) {
    return (int)cursor.readu8();
}

uint16_t NBT::parseShort(Cursor& cursor) {
    return (int)cursor.readu16();
}

int NBT::parseInt(Cursor& cursor) {
    return (int)cursor.readu32();
}

uint64_t NBT::parseLong(Cursor& cursor) {
    return (long long)cursor.readu64();
}

float NBT::parseFloat(Cursor& cursor) {
    float res = 0.f;
    uint32_t readValue = cursor.readu32();
    std::memcpy(&res, &readValue, sizeof(float));
    return res;
}

double NBT::parseDouble(Cursor& cursor) {
    double res = 0.0;
    uint64_t readValue = cursor.readu64();
    std::memcpy(&res, &readValue, sizeof(double));
    return res;
}

std::vector<char> NBT::parseByteArray(Cursor& cursor) {
    std::vector<char> res;
    uint32_t size = (int)cursor.readu32();
    res.reserve(size);
    //ss << "size:" << size << "\n";
    for (int i = 0;i < size;i++) {
        res.emplace_back(cursor.readu8());
    }

    return res;
}

std::string NBT::parseString(Cursor& cursor) {
    uint16_t size = cursor.readu16();
    return cursor.readString(size);
}