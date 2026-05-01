#include <iostream>
#include "Parser.h"
#include "Timer.h"
#include "DB.h"

std::vector<std::pair<int,int>> chunks;
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
void parseDB(const std::string& dbPath) {
    Timer timer;
    Parser parser;
	DB db(dbPath);
    leveldb::Iterator* it = db.newIterator();
    it->SeekToFirst();
    if(!it->Valid()){
        std::cerr << "Failed to open database\n";
        std::cerr << it->status().ToString() << "\n";
        delete it;
        return;
    }
    timer.start();
    for (it->SeekToFirst();it->Valid();it->Next()) {
        bool chunkKey = true;
        //Temporary (TODO:Handle all cases)
        for (int i = 0;i < keyPrefs.size();i++) {
            if (it->key().ToString().contains(keyPrefs[i])) {
                chunkKey = false;
            }
        }
        if (it->key().ToString() == "~local_player") {
            parser.parseLocalPlayer((uint8_t*)it->value().data());
        }else if(it->key().ToString().contains("player_")){
            parser.parseRemotePlayer(it->key().ToString(),(uint8_t*)it->value().data());
        }
        else if (it->key().ToString().contains("actorprefix")) {
            parser.parseActorPrefix((uint8_t*)it->key().data(),(uint8_t*)it->value().data());
        }else if (it->key().ToString().contains("digp")) {
            parser.parseDigp((uint8_t*)it->key().data(),(uint8_t*)it->value().data(),it->value().size());
        }
        if (!chunkKey)continue;
        
        parser.parseChunk((uint8_t*)it->key().data(),(uint8_t*)it->value().data(),it->key().size());
    }
    parser.drawChunkImage();
    double diff = timer.getDiff();
    std::cout << "Parsing took:" << diff << "ms.\n";
    delete it;
}
int main() {
    parseDB("tmp/testworld/db");
    Parser::parseDAT("tmp/testworld/level.dat");
}