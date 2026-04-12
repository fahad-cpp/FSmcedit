#include "Parser.h"
/*Parse NBT with the following structure:
    -1 byte : tagID
    -2 byte : length
    -length byte string : Name of the NBT
    -payload depending on the tagID
*/
uint32_t parseNBT(const uint8_t* data) {
    Cursor cursor(data, 0);
    uint8_t tagTypeID = cursor.readu8();
    uint16_t nameLength = tagTypeID == 0 ? 0 : cursor.readu16();
    if (tagTypeID < 0 || tagTypeID > 12) {
        std::cout << "Invalid type : " << (int)tagTypeID << "\n";
        return cursor.getOffset();
    }
    std::cout << "(" << types[(int)tagTypeID] << ")";
    std::cout << cursor.readString(nameLength) << ":";

    parseTag(tagTypeID, cursor);

    return cursor.getOffset();
}
//Parse only payload of the NBT
void parseTag(uint8_t tagID, Cursor& cursor) {
    if (tagID == 1) {
        std::cout << (int)parseByte(cursor) << "\n";
    }
    else if (tagID == 2) {
        std::cout << (int)parseShort(cursor) << "\n";
    }
    else if (tagID == 3) {
        std::cout << parseInt(cursor) << "\n";
    }
    else if (tagID == 4) {
        std::cout << parseLong(cursor) << "\n";
    }
    else if (tagID == 5) {
        std::cout << std::fixed << parseFloat(cursor) << "\n";
    }
    else if (tagID == 6) {
        std::cout << std::fixed << parseDouble(cursor) << "\n";
    }
    else if (tagID == 7) {
        std::vector<char> byteArray = parseByteArray(cursor);
    }
    else if (tagID == 8) {
        std::cout << parseString(cursor) << "\n";
    }
    else if (tagID == 9) {
        parseList(cursor);
    }
    else if (tagID == 10) {
        parseCompound(cursor);
    }
    else if (tagID == 11) {
        uint32_t size = (int)cursor.readu32();
        std::cout << "size:" << size << "\n";
        cursor.skip(size * 4);
    }
    else if (tagID == 12) {
        uint32_t size = (int)cursor.readu32();
        std::cout << "size:" << size << "\n";
        cursor.skip(size * 8);
    }
    else {
        std::cout << "Invalid TAG\n";
    }
}
//Parse a compund of NBTs
void parseCompound(Cursor& cursor) {
    while (cursor.peeku8() != 0) {
        uint32_t offset = parseNBT(cursor.getPtr());
        cursor.skip(offset);
    }
}
//Parse a list of NBTs
void parseList(Cursor& cursor) {
    uint8_t listTagID = cursor.readu8();
    uint32_t size = cursor.readu32();
    std::cout << "List size:" << size << "\n";

    for (int i = 0;i < size;i++) {
        std::cout << types[listTagID] << ":";
        parseTag(listTagID, cursor);
    }
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
    std::cout << "size:" << size << "\n";
    for (int i = 0;i < size;i++) {
        res.emplace_back(cursor.readu8());
    }

    return res;
}

std::string parseString(Cursor& cursor) {
    uint16_t size = cursor.readu16();
    return cursor.readString(size);
}