#include "Parser.h"
#include "Timer.h"
#include <iostream>
#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>
#include <leveldb/env.h>
#include <leveldb/zlib_compressor.h>
#include <leveldb/decompress_allocator.h>

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
class NullLogger : public leveldb::Logger {
public:
    void Logv(const char*, va_list) override {
    }
};
leveldb::DB* initLevelDB(const std::string& path){
    leveldb::Options options;
	options.info_log = new NullLogger();
    options.create_if_missing = false;
	options.compressors[0] = new leveldb::ZlibCompressorRaw(-1);
	options.compressors[1] = new leveldb::ZlibCompressor();
	leveldb::ReadOptions readOptions;
	readOptions.decompress_allocator = new leveldb::DecompressAllocator();
    leveldb::DB* db;
    leveldb::Status status = leveldb::DB::Open(options, path, &db);
    if (!status.ok()) {
        std::cerr << status.ToString() << "\n";
        assert(false);
        return nullptr;
    }

    return db;
}
void parseDB(const std::string& dbPath) {
    Timer timer;
    Parser parser;
	leveldb::DB* db = initLevelDB(dbPath);
    
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    it->SeekToFirst();
    if(!it->Valid()){
        std::cerr << "Failed to open database\n";
        std::cerr << it->status().ToString() << "\n";
        delete it;
        delete db;
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
    delete db;
}
int main() {
    parseDB("tmp/testworld/db");
    Parser::parseDAT("tmp/testworld/level.dat");
}