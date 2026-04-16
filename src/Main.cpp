#include <leveldb/db.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include "Parser.h"
#include "Cursor.h"

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

int main() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = false;
    leveldb::Status status = leveldb::DB::Open(options, "tmp/testworld/db", &db);
    if (!status.ok()) {
        std::cerr << status.ToString() << "\n";
        return 1;
    }

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

    for (it->SeekToFirst();it->Valid();it->Next()) {
        bool chunkKey = true;
        //Temporary (TODO:Handle all cases)
        for (int i = 0;i < keyPrefs.size();i++) {
            if (it->key().ToString().contains(keyPrefs[i])) {
                chunkKey = false;
            }
        }

        if(!chunkKey)continue;

        uint32_t keySize = it->key().size();
        uint8_t* keyData = (uint8_t*)malloc(keySize);
        memcpy(keyData, it->key().data(), keySize);

        uint32_t valueSize = it->value().size();
        uint8_t* valueData = (uint8_t*)malloc(valueSize);
        memcpy(valueData, it->value().data(), valueSize);

        Cursor keyCursor(keyData);
        std::cout << "Chunk:\n";
        std::cout << "\tX:" << (int)keyCursor.readu32() << "\n";
        std::cout << "\tZ:" << (int)keyCursor.readu32() << "\n";

        uint8_t record = keyCursor.readu8();

        std::string recordName =
            tagName.find(record) == tagName.end() ?
            "InvalidRecord:" + std::to_string((int)record)
            : tagName.at(record);
        std::cout << "\tRecord :" << recordName << "\n";

        Cursor valueCursor(valueData);
        if (recordName == "BlockEntity" || recordName == "Entity" || recordName == "RandomTicks") {
            //parseNBT(valueData);
        }
        else if (recordName == "Version") {
            uint8_t version = valueCursor.readu8();
            std::cout << "(Byte)" << (int)version << "\n";
        }else if(recordName == "SubChunkPrefix"){
            std::cout << "Size:" << valueSize << "\n";
            uint8_t version = valueCursor.readu8();
            std::cout << "Version:" << (int)version << "\n";
            uint8_t numStorage = 0;
            if(version == 1){
                numStorage = 1;
            }else if(version < 8){
                std::cout << "Unsupported Version\n";
                continue;
            }else{
                numStorage = valueCursor.readu8();
            }

            if(version >= 9){
                valueCursor.skip(1);
            }
            for(uint8_t i=0;i<numStorage;i++){
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
                for(uint32_t j=0;j<paletteSize;j++){
                    uint32_t offset = parseNBT(valueCursor.getPtr());
                    valueCursor.skip(offset);
                }
            }
        }

        free(valueData);
        free(keyData);
    }

    delete it;
    delete db;
    return 0;
}