#include <map>
#include <math.h>
#include <iostream>
#include <fstream>

#include <leveldb/db.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Parser.h"
std::vector<std::pair<int,int>> chunks = {};
const std::vector<std::string> keyPrefs = {
    "~local_player",
    "player_",
    "VILLAGE_",
    "AutonomousEntities",
    "BiomeData",
    "Overworld",
    "WorldClocks",
    "actorprefix",
    "digp",
    "mobevents",
    "scoreboard"
};

const std::map<uint8_t, const std::string> tagName{
    //Terrain & Biome Data
    {43,"Data3D"},
    {44,"Version"},
    {45,"Data2D"},
    {46,"Data2DLegacy"},
    {47,"SubChunkPrefix"},
    {48,"LegacyTerrain"},
    //Entity & Block Entity Data
    {49,"BlockEntity"},
    {50,"Entity"},
    {51,"PendingTicks"},
    {58,"RandomTicks"},
    //World State & Features
    {52,"LegacyBlockExtraData"},
    {53,"BiomeState"},
    {54,"FinalizedState"},
    {56,"BorderBlocks"},
    {57,"HardcodedSpawners"},
    {59,"Checksums"},
    {61,"MetaDataHash"},
    {62,"GeneratedPreCavesAndCliffsBlending"},
    {63,"BlendingBiomeHeight"},
    {64,"BlendingData"},
    {65,"ActorDigestVersion"},
    //Legacy & Deprecated Records
    {55,"ConversionData"},
    {118,"LegacyVersion"},
};
void parseDB(const std::string& dbPath) {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = false;
    leveldb::Status status = leveldb::DB::Open(options, dbPath, &db);
    if (!status.ok()) {
        std::cerr << status.ToString() << "\n";
        assert(false);
        return;
    }

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    // it->SeekToFirst();
    // if(!it->Valid()){
    //     std::cerr << "Failed to open database\n";
    //     std::cerr << it->status().ToString() << "\n";
    //     return;
    // }
    for (it->SeekToFirst();it->Valid();it->Next()) {
        bool chunkKey = true;
        //Temporary (TODO:Handle all cases)
        for (int i = 0;i < keyPrefs.size();i++) {
            if (it->key().ToString().contains(keyPrefs[i])) {
                std::cout << "KEY:" << it->key().ToString() << "\n";
                chunkKey = false;
            }
        }
        if (it->key().ToString() == "~local_player") {
            Cursor cursor((uint8_t*)it->value().data());
            parseNBT(cursor);
        }
        else if (it->key().ToString().contains("actorprefix")) {
            Cursor cursor((uint8_t*)it->key().data(), 11);
            int64_t id = (int64_t)cursor.readu64();
            std::cout << "id:" << id << "\n";
            Cursor valueCursor((uint8_t*)it->value().data());
            parseNBT(valueCursor);
        }else if (it->key().ToString().contains("digp")) {
            Cursor cursor((uint8_t*)it->key().data(), 4);
            int x = cursor.readu32();
            int y = cursor.readu32();
            std::cout << "x:" << x << " y:" << y << "\n";
            Cursor valueCursor((uint8_t*)it->value().data());
            uint32_t entitySize = it->value().size() / 8;
            std::vector<int64_t> entityIDs = {};
            entityIDs.reserve(entitySize);
            for(int i=0;i<entitySize;i++){
                entityIDs.emplace_back((int64_t)valueCursor.readu64());
                std::cout << "ID " << i << ": " << entityIDs.at(i) << "\n";
            }
        }
        if (!chunkKey)continue;

        uint32_t keySize = it->key().size();
        uint8_t* keyData = (uint8_t*)malloc(keySize);
        memcpy(keyData, it->key().data(), keySize);

        uint32_t valueSize = it->value().size();
        uint8_t* valueData = (uint8_t*)malloc(valueSize);
        memcpy(valueData, it->value().data(), valueSize);

        Cursor keyCursor(keyData);
        std::cout << "Chunk:";
        //std::cout << "keySize:" << keySize << "\n";
        int x = (int)keyCursor.readu32();
        int z = (int)keyCursor.readu32();
        if (std::find(chunks.begin(),chunks.end(),std::pair{x,z}) == chunks.end()){
            chunks.push_back(std::pair{x,z});
        }
        std::cout << "\tX:" << x;
        std::cout << "\tZ:" << z << "\n";
        // std::cout << "\tWX:" << x * 16;
        // std::cout << "\tWZ:" << z * 16 << "\n";

        uint8_t record = keyCursor.readu8();

        std::string recordName =
            tagName.find(record) == tagName.end() ?
            "InvalidRecord:" + std::to_string((int)record)
            : tagName.at(record);
        std::cout << "\tRecord :" << recordName << "\n";

        Cursor valueCursor(valueData);
        if (recordName == "BlockEntity" || recordName == "Entity" || recordName == "RandomTicks") {
            parseNBT(valueCursor);
        }
        else if (recordName == "Version") {
            uint8_t version = valueCursor.readu8();
            std::cout << "(Byte)" << (int)version << "\n";
        }
        else if (recordName == "SubChunkPrefix") {
            std::cout << "Size:" << valueSize << "\n";
            uint8_t version = valueCursor.readu8();
            std::cout << "Version:" << (int)version << "\n";
            uint8_t numStorage = 0;
            if (version == 1) {
                numStorage = 1;
            }
            else if (version < 8) {
                std::cout << "Unsupported Version\n";
                assert(false);
                continue;
            }
            else {
                numStorage = valueCursor.readu8();
            }

            if (version >= 9) {
                uint8_t subChunkIndex = valueCursor.readu8();
                std::cout << "subChunkIndex:" << (int)subChunkIndex << "\n";
            }
            for (uint8_t i = 0;i < numStorage;i++) {
                uint8_t flags = valueCursor.readu8();
                uint8_t bitsPerBlock = flags >> 1;
                uint32_t blocksPerWord = floor(32.f / (float)bitsPerBlock);
                uint32_t blockStateCount = ceil(4096.f / (float)blocksPerWord);
                valueCursor.skip(blockStateCount * 4);
                uint32_t paletteSize = valueCursor.readu32();
                std::cout << "numStorage:" << (int)numStorage << "\n";
                std::cout << "bitsPerBlock:" << (int)bitsPerBlock << "\n";
                std::cout << "blocksPerWord:" << (int)blocksPerWord << "\n";
                std::cout << "blockStateCount:" << (int)blockStateCount << "\n";
                std::cout << "pallette size:" << paletteSize << "\n";
                for (uint32_t j = 0;j < paletteSize;j++) {
                    parseNBT(valueCursor);
                }
            }
        }

        free(valueData);
        free(keyData);
    }

    delete it;
    delete db;
}
void parseDAT(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    uint32_t size = ifs.tellg();
    ifs.seekg(0);
    uint8_t* data = (uint8_t*)malloc(size * sizeof(uint8_t));
    ifs.read((char*)data, size);
    Cursor cursor(data);
    uint32_t header = cursor.readu32();
    uint32_t length = cursor.readu32();
    if (header != 10) {
        std::cout << "Header:" << header << "\n";
        std::cout << "Length:" << length << "\n";
        std::cerr << "Not a valid level.dat file\n";
        assert(false);
        return;
    }
    parseNBT(cursor);
    free(data);
    ifs.close();
}
void drawChunkImage(std::vector<std::pair<int,int>> chunks){
    std::vector<int> xcords = {};
    std::vector<int> zcords = {};
    std::cout << "All chunks\n";
    for(const std::pair<int,int>& chunk : chunks){
        xcords.push_back(chunk.first);
        zcords.push_back(chunk.second);
    }
    std::sort(zcords.begin(),zcords.end());

    xcords.erase(std::unique(xcords.begin(),xcords.end()),xcords.end());
    zcords.erase(std::unique(zcords.begin(),zcords.end()),zcords.end());

    int xmin = xcords.at(std::min_element(xcords.begin(),xcords.end()) - xcords.begin());
    int zmin = zcords.at(std::min_element(zcords.begin(),zcords.end()) - zcords.begin());
    for(int i=0;i<xcords.size();i++){
        xcords[i] -= xmin;
    }
    for(int i=0;i<zcords.size();i++){
        zcords[i] -= zmin;
    }
    for(int i=0;i<chunks.size();i++){
        chunks[i] = {chunks[i].first - xmin,chunks[i].second - zmin};
        std::cout << "X:"<<chunks[i].first<<" Z:" << chunks[i].second << "\n";
    }

    int xdiff = (xcords.at(xcords.size()-1) - xcords.at(0)) + 1;
    int zdiff = (zcords.at(xcords.size()-1) - zcords.at(0)) + 1;

    int width = xdiff*16;
    int height = zdiff*16;

    uint32_t* image = (uint32_t*)malloc(width*height*sizeof(uint32_t));
    for(int i=0;i<width*height;i++)image[i] = 0x00000000;
    for(const std::pair<int,int>& chunk : chunks){
        int xstart=chunk.first*16;
        int zstart=chunk.second*16;
        for(int x=xstart;x<(xstart+16);x++){
            for(int z=zstart;z<(zstart+16);z++){
                image[width*z + x] = 0xff0000ff;
            }
        }
    }
    stbi_write_png("chunks.png",width,height,4,image,width*4);
    free(image);

}
int main() {
    parseDB("tmp/testworld/db");
    drawChunkImage(chunks);
    //parseDAT("tmp/testworld/level.dat");
    return 0;
}