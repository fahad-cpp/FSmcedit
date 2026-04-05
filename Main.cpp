#include <leveldb/db.h>
#include <iostream>
#include "NBT.h"

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
    }

    delete it;
    delete db;
    return 0;
}