#include "DB.h"
#include <iostream>

DB::DB(const std::string &dbPath) {
    mOptions.info_log = new NullLogger();
    mOptions.create_if_missing = false;
    mOptions.compressors[0] = new leveldb::ZlibCompressorRaw(-1);
    mOptions.compressors[1] = new leveldb::ZlibCompressor();
    mReadOptions.decompress_allocator = new leveldb::DecompressAllocator();
    leveldb::Status status = leveldb::DB::Open(mOptions, dbPath, &mDb);
    if (!status.ok()) {
        std::cerr << status.ToString() << "\n";
        assert(false);
    }
}

DB::~DB() {
    delete mDb;
    delete mReadOptions.decompress_allocator;
    delete mOptions.compressors[1];
    delete mOptions.compressors[0];
    delete mOptions.info_log;
}

leveldb::Iterator *DB::newIterator() {
    return mDb->NewIterator(mReadOptions);
}
