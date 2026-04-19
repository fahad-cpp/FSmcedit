#include "Parser.h"
/*Parse NBT with the following structure:
    -1 byte : tagID
    -2 byte : length
    -length byte string : Name of the NBT
    -payload depending on the tagID
*/
void parseNBT(Cursor& cursor,std::stringstream& ss,uint32_t depth) {
    uint8_t tagTypeID = cursor.readu8();
    uint16_t nameLength = tagTypeID == 0 ? 0 : cursor.readu16();
    if (tagTypeID < 0 || tagTypeID > 12) {
        ss << "Invalid type : " << (int)tagTypeID << "\n";
        assert(false);
    }
    const std::string& name = cursor.readString(nameLength);
    for(int i=0;i<depth;i++)ss << '\t';
    
    if(nameLength != 0){
        ss << "\"" << name << "\"" << ":";
    }

    parseTag(tagTypeID, cursor,ss,depth);
}
//Parse only payload of the NBT
void parseTag(uint8_t tagID, Cursor& cursor,std::stringstream& ss,uint32_t depth) {
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
            parseList(cursor,ss,depth+1);
        }break;
        case 10: {
            parseCompound(cursor,ss,depth+1);
        }break;
        case 11: {
            //Unhandled
            uint32_t size = (int)cursor.readu32();
            ss << "size:" << size << "\n";
            cursor.skip(size * 4);
        }break;
        case 12: {
            //Unhandled
            uint32_t size = (int)cursor.readu32();
            ss << "size:" << size << "\n";
            cursor.skip(size * 8);
        }break;
        default: {
            ss << "Invalid TAG\n";
            assert(false);
        }
    }
}
//Parse a compund of NBTs
void parseCompound(Cursor& cursor,std::stringstream& ss,uint32_t depth) {
    if(cursor.peeku8() == 0){
        ss << "{}";
        cursor.skip(1);
        return;
    }
    ss << "{\n";
    while (cursor.peeku8() != 0) {
        parseNBT(cursor,ss,depth);
        if(cursor.peeku8() != 0){
            ss << ",\n";
        }else{
            ss << "\n";
        }
    }
    for(int i=0;i<depth-1;i++)ss << '\t';
    ss << "}";
    cursor.skip(1);
}
//Parse a list of NBTs
void parseList(Cursor& cursor,std::stringstream& ss,uint32_t depth) {
    uint8_t listTagID = cursor.readu8();
    uint32_t size = cursor.readu32();
    if(size == 0){
        ss << "[]";
        return;
    }
    ss << "[\n";
    for (int i = 0;i < size;i++) {
        for(int j=0;j<depth;j++)ss << '\t';
        parseTag(listTagID, cursor,ss,depth);
        if(i != (size-1))ss <<",\n";
        else ss << "\n";
    }
    for(int i=0;i<depth-1;i++)ss << '\t';
    ss << ']';
}

uint8_t parseByte(Cursor& cursor) {
    return (int)cursor.readu8();
}

uint16_t parseShort(Cursor& cursor) {
    return (int)cursor.readu16();
}

int parseInt(Cursor& cursor) {
    return (int)cursor.readu32();
}

uint64_t parseLong(Cursor& cursor) {
    return (long long)cursor.readu64();
}

float parseFloat(Cursor& cursor) {
    float res = 0.f;
    uint32_t readValue = cursor.readu32();
    std::memcpy(&res, &readValue, sizeof(float));
    return res;
}

double parseDouble(Cursor& cursor) {
    double res = 0.0;
    uint64_t readValue = cursor.readu64();
    std::memcpy(&res, &readValue, sizeof(double));
    return res;
}

std::vector<char> parseByteArray(Cursor& cursor) {
    std::vector<char> res;
    uint32_t size = (int)cursor.readu32();
    res.reserve(size);
    //ss << "size:" << size << "\n";
    for (int i = 0;i < size;i++) {
        res.emplace_back(cursor.readu8());
    }

    return res;
}

std::string parseString(Cursor& cursor) {
    uint16_t size = cursor.readu16();
    return cursor.readString(size);
}