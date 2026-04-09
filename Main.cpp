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
    leveldb::Status status = leveldb::DB::Open(options,"tmp/testdb",&db);
    if(!status.ok()){
        std::cerr << status.ToString() << "\n";
        return 1;
    }
        
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

    for(it->SeekToFirst();it->Valid();it->Next()){
        bool chunkKey = true;
        for(int i=0;i<KEYPREFSIZE;i++){
            if(it->key().ToString().contains(keyPrefs[i])){
                chunkKey = false;
            }
        }

        if(chunkKey){

            uint32_t keySize = it->key().size();
            uint8_t *data = (uint8_t*)malloc(keySize);
            memcpy(data,it->key().data(),keySize);

            Cursor cursor(data,0);
            std::cout << "Chunk:\n";
            std::cout << "\tX:" << (int)cursor.readu32() << "\n";
            std::cout << "\tZ:" << (int)cursor.readu32() << "\n";
            
            uint8_t record = cursor.readu8();
            std::string recordName = tagName[record];
            std::cout << "\tRecord :" << recordName << "\n";

            free(data);
        }
    }

    delete it;
    delete db;
    return 0;
}