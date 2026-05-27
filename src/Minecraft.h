#include <nlohmann/json.hpp>
#include <stdint.h>
using json = nlohmann::json;

struct Entity {
    int64_t id;
    int32_t cx, cz;
};
struct BlockEntity {
    int32_t cx, cz;
    json nbt;
};
struct Chunk {
    int32_t cx, cz;
    int8_t subchunkIndex;
    json palette;
};
