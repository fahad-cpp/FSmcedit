#ifndef PARSER_H
#define PARSER_H
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <fstream>
#include <algorithm>
#include <map>
#include <vector>
#include "Cursor.h"
#include "Minecraft.h"
class Parser{
    std::vector<Entity> entities;
    std::vector<BlockEntity> blockEntities;
    std::vector<Chunk> chunks;
    json playersJson;
    uint32_t playerCount = 0;
    enum class TAG{
        Data3D = 43,
        Version,
        Data2D,
        Data2DLegacy,
        SubChunkPrefix,
        LegacyTerrain,
        BlockEntity,
        Entity,
        PendingTicks,
        LegacyBlockExtraData,
        BiomeState,
        FinalizedState,
        ConversionData,
        BorderBlocks,
        HardcodedSpawners,
        RandomTicks,
        Checksums,
        MetaDataHash = 61,
        GeneratedPreCavesAndCliffsBlending,
        BlendingBiomeHeight,
        BlendingData,
        ActorDigestVersion,
        LegacyVersion = 118
    };
public:
    static void parseDAT(const std::string& path);
    static void parseStructure(const std::string& filepath);
    ~Parser();
    void parseLocalPlayer(uint8_t* value);
    void parseDigp(uint8_t* key,uint8_t* value,uint32_t valueSize);
    void parseActorPrefix(uint8_t* key, uint8_t* value);
    void parseChunk(uint8_t* key,uint8_t* value,uint32_t keySize);
    void parseRemotePlayer(std::string key,uint8_t* value);
    void drawChunkImage();
};
#endif