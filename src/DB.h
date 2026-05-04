#ifndef FSDB
#define FSDB
#include <iostream>
#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>
#include <leveldb/env.h>
#include <leveldb/zlib_compressor.h>
#include <leveldb/decompress_allocator.h>
class NullLogger : public leveldb::Logger {
public:
    void Logv(const char*, va_list) override {
    }
};
class DB{
    private:
    leveldb::DB* mDb = nullptr;
	leveldb::ReadOptions mReadOptions;
    leveldb::Options mOptions;
    public:
    DB(const std::string& dbPath);
    ~DB();
    //You need to delete the iterator
    leveldb::Iterator* newIterator();
};
#endif