#ifndef PARSER_H
#define PARSER_H
#include "Cursor.h"
#include <fstream>
#include <algorithm>
#include <map>
#include <vector>
class Parser{
    const std::map<const uint8_t, const std::string> tagName{
    //Terrain & Biome Data
    {43,"Data3D"},
    {44,"Version"},
    {45,"Data2D"},
    {46,"Data2DLegacy"},
    {47,"SubChunkPrefix"},
    {48,"LegacyTerrain"},
    //Entity & Block Entity Data
    {49,"BlockEntity"},
    {50,"Entity"},
    {51,"PendingTicks"},
    {58,"RandomTicks"},
    //World State & Features
    {52,"LegacyBlockExtraData"},
    {53,"BiomeState"},
    {54,"FinalizedState"},
    {56,"BorderBlocks"},
    {57,"HardcodedSpawners"},
    {59,"Checksums"},
    {61,"MetaDataHash"},
    {62,"GeneratedPreCavesAndCliffsBlending"},
    {63,"BlendingBiomeHeight"},
    {64,"BlendingData"},
    {65,"ActorDigestVersion"},
    //Legacy & Deprecated Records
    {55,"ConversionData"},
    {118,"LegacyVersion"},
};
public:
    std::vector<std::pair<int,int>> chunks;
    void parseLocalPlayer(uint8_t* value);
    void parseDigp(uint8_t* key,uint8_t* value,uint32_t valueSize);
    void parseActorPrefix(uint8_t* key, uint8_t* value);
    void parseChunk(uint8_t* key,uint8_t* value);
    void parseRemotePlayer(std::string key,uint8_t* value);
    void parseDAT(const std::string& path);
    void drawChunkImage();
};
#endif