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
        //std::cout << (it->key().ToString()) << "\n";
        bool chunkKey = true;
        for(int i=0;i<KEYPREFSIZE;i++){
            if(it->key().ToString().contains(keyPrefs[i])){
                chunkKey = false;
            }
        }

        if(chunkKey){
            std::string chunkdata = it->key().ToString();
            Cursor cursor((uint8_t*)chunkdata.data(),0);
            //std::cout << "KEY:" << it->key().ToString() << "\n";
            std::cout << "X:" << (int)cursor.readu32();
            std::cout << " Z:" << (int)cursor.readu32() << "\n";

             uint32_t dimension = cursor.readu32();
            // std::cout << "Dimension:" << dimension << '\n';

            std::cout << "RecordType:" << (uint32_t)cursor.readu8() << "\n";
        }
    }

    delete it;
    delete db;
    return 0;
}