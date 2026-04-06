#include <leveldb/db.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include "Parser.h"
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
        std::cout << (it->key().ToString()) << "\n";
        bool chunkKey = true;
        for(int i=0;i<KEYPREFSIZE;i++){
            if(it->key().ToString().contains(keyPrefs[i])){
                chunkKey = false;
            }
        }

        if(chunkKey){
            it->value().ToString();
        }
    }

    delete it;
    delete db;
    return 0;
}