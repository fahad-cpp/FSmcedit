#include <leveldb/db.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
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
            uint64_t id = cursor.readu64();
            std::cout << "id:" << id << "\n";
            Cursor valueCursor((uint8_t*)it->value().data());
            parseNBT(valueCursor);
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
int main() {
    parseDB("tmp/testworld/db");
    //parseDAT("tmp/testworld/level.dat");
    return 0;
}