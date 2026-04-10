#include "Parser.h"
uint32_t parseNBT(const uint8_t* data) {
    std::cout << "Parsing NBT...\n";
    Cursor cursor(data, 0);
    uint8_t tagTypeID = cursor.readu8();
    uint16_t nameLength = tagTypeID == 0 ? 0 : cursor.readu16();
    if (tagTypeID < 0 || tagTypeID > 12) {
        std::cout << "Invalid type : " << (int)tagTypeID << "\n";
        return cursor.getOffset();
    }
    std::cout << "Type: " << types[(int)tagTypeID] << "\n";
    if (nameLength > 0)std::cout << "Name:" << cursor.readString(nameLength) << "\n";

    if(tagTypeID == 1){
        std::cout << (int)parseByte(cursor) << "\n";
    }else if(tagTypeID == 2){
        std::cout << (int)parseShort(cursor) << "\n";
    }else if(tagTypeID == 3){
        std::cout << parseInt(cursor) << "\n";
    }else if(tagTypeID == 4){
        std::cout << parseLong(cursor) << "\n";
    }else if(tagTypeID == 5){
        std::cout << parseFloat(cursor) << "\n";
    }else if(tagTypeID == 6){
        std::cout << parseDouble(cursor) << "\n";
    }else if(tagTypeID == 7){
        std::vector<char> byteArray = parseByteArray(cursor);
    }else if(tagTypeID == 8){
        std::cout << parseString(cursor) << "\n";
    }else if(tagTypeID == 9){
        parseList(cursor);
    }else if(tagTypeID == 10){
        uint32_t offset = parseCompound(cursor.getPtr());
        cursor.skip(offset);
    }else if(tagTypeID == 11){
        uint32_t size = (int)cursor.readBEu32();
        std::cout << "size:" << size << "\n";
        cursor.skip(size*4);
    }else if(tagTypeID == 12){
        uint32_t size = (int)cursor.readBEu32();
        std::cout << "size:" << size << "\n";
        cursor.skip(size*8);
    }

    return cursor.getOffset();
}
uint32_t parseCompound(const uint8_t* data){
    Cursor cursor(data,0);
    while(cursor.readu8() != 0){
        uint32_t offset = parseNBT(data);
        cursor.skip(offset);
    }
    std::cout << "Compound END\n";
    return cursor.getOffset();
}
uint32_t getItemSize(uint8_t tag){
    if(tag == 1)return 1;
    else if(tag == 2)return 2;
    else if(tag == 3)return 4;
    else if(tag == 4)return 8;
    else if(tag == 5)return 4;
    else if(tag == 6)return 8;
    else return 0;
}
void parseList(Cursor& cursor){
    uint8_t listTagID = cursor.readu8();
    uint32_t size = cursor.readBEu32();
    
    for(int i=0;i<size;i++){
        std::cout << types[listTagID] << ":";
        if(listTagID == 1){
            std::cout << (int)parseByte(cursor) << "\n";
        }else if(listTagID == 2){
            std::cout << (int)parseShort(cursor) << "\n";
        }else if(listTagID == 3){
            std::cout << parseInt(cursor) << "\n";
        }else if(listTagID == 4){
            std::cout << parseLong(cursor) << "\n";
        }else if(listTagID == 5){
            std::cout << parseFloat(cursor) << "\n";
        }else if(listTagID == 6){
            std::cout << parseDouble(cursor) << "\n";
        }else if(listTagID == 7){
            std::vector<char> byteArray = parseByteArray(cursor);
        }else if(listTagID == 8){
            std::cout << parseString(cursor) << "\n";
        }else if(listTagID == 9){
            parseList(cursor);
        }else if(listTagID == 10){
            uint32_t offset = parseCompound(cursor.getPtr());
            cursor.skip(offset);
        }else{
            std::cout << "Unhandled list\n";
            return;
        }
    }

    std::cout << "List END\n";
}

uint8_t parseByte(Cursor& cursor){
    return (int)cursor.readu8();
}

uint16_t parseShort(Cursor& cursor){
    return (int)cursor.readBEu16();
}

uint32_t parseInt(Cursor& cursor){
    return (int)cursor.readBEu32();
}

uint64_t parseLong(Cursor& cursor){
    return (long long)cursor.readBEu64();
}

float parseFloat(Cursor& cursor){
    return (float)cursor.readBEu32();
}

double parseDouble(Cursor& cursor){
    return (double)cursor.readBEu64();
}

std::vector<char> parseByteArray(Cursor& cursor){
    std::vector<char> res;
    uint32_t size = (int)cursor.readu32();
    res.reserve(size);
    std::cout << "size:" << size << "\n";
    for(int i=0;i<size;i++){
        res.emplace_back(cursor.readu8());
    }

    return res;
}

std::string parseString(Cursor& cursor){
    uint16_t size = cursor.readBEu16();
    return cursor.readString(size);
}