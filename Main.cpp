#include <leveldb/db.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include "Parser.h"
#include "Cursor.h"
#define KEYPREFSIZE 11
std::string keyPrefs[KEYPREFSIZE]{
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

std::map<uint8_t,std::string> tagName{
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

int main(){
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = false;
    leveldb::Status status = leveldb::DB::Open(options,"tmp/testworld/db",&db);
    if(!status.ok()){
        std::cerr << status.ToString() << "\n";
        return 1;
    }
        
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

    for(it->SeekToFirst();it->Valid();it->Next()){
        bool chunkKey = true;
        //Temporary (TODO:Handle all cases)
        for(int i=0;i<KEYPREFSIZE;i++){
            if(it->key().ToString().contains(keyPrefs[i])){
                chunkKey = false;
            }
        }

        if(chunkKey){

            uint32_t keySize = it->key().size();
            uint8_t *keyData = (uint8_t*)malloc(keySize);
            memcpy(keyData,it->key().data(),keySize);

            uint32_t valueSize = it->value().size();
            uint8_t *valueData = (uint8_t*)malloc(valueSize);
            memcpy(valueData,it->value().data(),valueSize);

            Cursor cursor(keyData);
            std::cout << "Chunk:\n";
            std::cout << "\tX:" << (int)cursor.readu32() << "\n";
            std::cout << "\tZ:" << (int)cursor.readu32() << "\n";
            
            uint8_t record = cursor.readu8();

            std::string recordName = tagName.find(record) == tagName.end()?"InvalidRecord":tagName[record];
            std::cout << "\tRecord :" << recordName << "\n";

            if(recordName == "BlockEntity"){
                parseNBT(valueData);
            }

            free(valueData);
            free(keyData);
        }
    }

    delete it;
    delete db;
    return 0;
}