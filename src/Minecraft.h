#include <stdint.h>
#include "NBT.h"

struct Entity{
    int64_t id;
    int32_t cx,cz;
};
struct BlockEntity{
    std::string id;
    json nbt;
};
struct Chunk{
    int32_t cx,cz;
    json palette;
    std::vector<Entity> entities;
};