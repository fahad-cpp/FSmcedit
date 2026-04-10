#include "Parser.h"
#include "Cursor.h"
void parseNBT(const uint8_t* data){
    std::cout << "Parsing NBT...\n";
    Cursor cursor(data,0);
    uint8_t tagTypeID = cursor.readu8();
    uint16_t nameLength = cursor.readBEu16();
    
    std::cout << "ID: " << (int)tagTypeID << "\n";
    std::cout << "nameLength: " << nameLength << "\n";
}