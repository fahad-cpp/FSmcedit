#include "Parser.h"
#include "NBT.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
// std::vector<std::pair<int,int>> chunks;

void Parser::parseLocalPlayer(uint8_t* value){
    Cursor cursor(value);

    std::stringstream ss;
    NBT::parseNBT(cursor,ss);
    std::ofstream ofs("worldData/player.json");
    json playerJson;
    try{
        playerJson = json::parse(ss);
    }catch(json::exception e){
        std::cerr << e.what() << "\n";
    }
    ofs << playerJson.dump(4);
    ofs.close();
}

void Parser::parseDigp(uint8_t* key,uint8_t* value,uint32_t valueSize) {
    Cursor keyCursor(key, 4);
    Cursor valueCursor(value);

    int x = keyCursor.readu32();
    int y = keyCursor.readu32();
    uint32_t entitySize = valueSize / 8;
    std::vector<int64_t> entityIDs = {};
    entityIDs.reserve(entitySize);
    for(int i=0;i<entitySize;i++){
        entityIDs.emplace_back((int64_t)valueCursor.readu64());
    }
}

void Parser::parseActorPrefix(uint8_t* key,uint8_t* value) {
    Cursor keyCursor(key);
    Cursor valueCursor(value);

    std::stringstream ss;
    int64_t id = (int64_t)keyCursor.readu64();
    NBT::parseNBT(valueCursor,ss);
    json actorJson = json::parse(ss);
}

void Parser::parseChunk(uint8_t* key,uint8_t* value) {
    Cursor keyCursor(key);
    Cursor valueCursor(value);
    
    json chunkJson;
    int x = (int)keyCursor.readu32();
    int z = (int)keyCursor.readu32();
    uint8_t record = keyCursor.readu8();
    
    std::string recordName =
    tagName.find(record) == tagName.end() ?
    "InvalidRecord:" + std::to_string((int)record)
    : tagName.at(record);
    
    std::cout << "x: " << x << " z: " << z << " :" << recordName << "\n";
    if ((recordName == "SubChunkPrefix") && (std::find(chunks.begin(),chunks.end(),std::pair{x,z}) == chunks.end())){
        chunks.push_back(std::pair{x,z});
    }


    if (recordName == "BlockEntity" || recordName == "Entity") {
        // std::stringstream ss;
        // NBT::parseNBT(valueCursor,ss);

        // entityJson = json::parse(ss);
    }
    else if (recordName == "Version") {
        uint8_t version = valueCursor.readu8();
    }
    else if (recordName == "SubChunkPrefix") {
        // std::stringstream ss;
        // uint8_t version = valueCursor.readu8();
        // uint8_t numStorage = 0;
        // if (version == 1) {
        //     numStorage = 1;
        // }
        // else if (version < 8) {
        //     std::cerr << "Unsupported Version\n";
        //     assert(false);
        //     return;
        // }
        // else {
        //     numStorage = valueCursor.readu8();
        // }
        // int8_t subChunkIndex = 0;
        // if (version >= 9) {
        //     subChunkIndex = (int8_t)valueCursor.readu8();
        // }
        // ss << '[';
        // for (uint8_t i = 0;i < numStorage;i++) {
        //     uint8_t flags = valueCursor.readu8();
        //     uint8_t bitsPerBlock = flags >> 1;
        //     uint32_t blocksPerWord = floor(32.f / (float)bitsPerBlock);
        //     uint32_t blockStateCount = ceil(4096.f / (float)blocksPerWord);
        //     valueCursor.skip(blockStateCount * 4);
        //     uint32_t paletteSize = valueCursor.readu32();
        //     ss << "{\"palette\":[";
        //     for (uint32_t j = 0;j < paletteSize;j++) {
        //         NBT::parseNBT(valueCursor,ss);
        //         if(j != (paletteSize-1)){
        //             ss << ",";
        //         }
        //     }
        //     ss << "]}";
        //     if(i != (numStorage - 1)){
        //         ss << ',';
        //     }
        // }
        // ss << ']';
        // std::string key = "subchunk-" + std::to_string(subChunkIndex);
        // try{
        //     chunkJson[key] = json::parse(ss);
        // }catch(json::exception e){
        //     std::cerr << "Json error:" << e.what() << "\n";
        //     return;
        // }
        // std::string filename = "worldData/chunk_" + std::to_string(x) + "_" + std::to_string(z) + ".json";
        // std::ofstream ofs(filename);
        // ofs << chunkJson.dump(4);
        // ofs.close();
    }
}

void Parser::parseRemotePlayer(std::string key,uint8_t* value) {
    Cursor valueCursor(value);
    std::string playerId = key.substr(7,key.length() - 7);
    std::stringstream ss;
    NBT::parseNBT(valueCursor,ss);
    std::string filename = "worldData/player_" + playerId + ".json";
    std::ofstream ofs(filename);
    json playerJson = json::parse(ss);
    ofs << playerJson.dump(4);
    ofs.close();
}

void Parser::parseDAT(const std::string& path) {
    std::stringstream ss;
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    if(!ifs.is_open()){
        std::cerr << "Failed to open level.dat file\n";
        return;
    }
    uint32_t size = ifs.tellg();
    ifs.seekg(0);
    uint8_t* data = (uint8_t*)malloc(size * sizeof(uint8_t));
    ifs.read((char*)data, size);
    Cursor cursor(data);
    uint32_t header = cursor.readu32();
    uint32_t length = cursor.readu32();
    if (header != 10) {
        std::cerr << "Header:" << header << "\n";
        std::cerr << "Length:" << length << "\n";
        std::cerr << "Not a valid level.dat file\n";
        assert(false);
        return;
    }
    NBT::parseNBT(cursor,ss);
    free(data);
    ifs.close();

    std::ofstream ofs("worldData/level.json");
    json levelJson;
    try{
        levelJson = json::parse(ss);
    }catch(json::exception e){
        std::cerr << "Json error" << e.what() << "\n";
        return;
    }
    ofs << levelJson.dump(4);
}

void Parser::drawChunkImage(){
    std::cout << "Drawing image\n";
    if(!chunks.size())return;
    std::vector<int> xcords = {};
    std::vector<int> zcords = {};
    for(const std::pair<int,int>& chunk : chunks){
        xcords.push_back(chunk.first);
        zcords.push_back(chunk.second);
    }
    
    std::sort(xcords.begin(),xcords.end());
    std::sort(zcords.begin(),zcords.end());
    
    //Remove Duplicate
    xcords.erase(std::unique(xcords.begin(),xcords.end()),xcords.end());
    zcords.erase(std::unique(zcords.begin(),zcords.end()),zcords.end());
    
    //offset to make minx=0 minz=0
    int xmin = xcords.at(std::min_element(xcords.begin(),xcords.end()) - xcords.begin());
    int zmin = zcords.at(std::min_element(zcords.begin(),zcords.end()) - zcords.begin());
    for(int i=0;i<xcords.size();i++){
        xcords[i] -= xmin;
    }
    for(int i=0;i<zcords.size();i++){
        zcords[i] -= zmin;
    }

    for(int i=0;i<chunks.size();i++){
        chunks[i] = std::pair{chunks[i].first - xmin,chunks[i].second - zmin};
    }
    
    int xdiff = abs(xcords.at(xcords.size()-1) - xcords.at(0)) + 1;
    int zdiff = abs(zcords.at(zcords.size()-1) - zcords.at(0)) + 1;
    
    int width = xdiff*16;
    int height = zdiff*16;
    //Render chunks on image
    uint32_t* image = (uint32_t*)malloc(width*height*sizeof(uint32_t));
    for(int i=0;i<width*height;i++)image[i] = 0x00000000;
    for(const std::pair<int,int>& chunk : chunks){
        int xstart=chunk.first*16;
        int zstart=chunk.second*16;
        for(int x=xstart;x<(xstart+16);x++){
            for(int z=zstart;z<(zstart+16);z++){
                image[width*z + x] = 0xff0000ff; //red
            }
        }
    }
    stbi_write_png("worldData/chunks.png",width,height,4,image,width*4);
    free(image);

}
