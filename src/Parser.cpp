#include <stdexcept>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <fstream>
#include <iostream>

#include "Log.h"
#include "NBT.h"
#include "Parser.h"
#include "Timer.h"

void Parser::parseLocalPlayer(uint8_t *value) {
    Cursor cursor(value);

    std::stringstream ss;
    NBT::parseNBT(cursor, ss);
    std::string id = "local_player";
    try {
        playersJson[id] = json::parse(ss);
    } catch (json::exception e) {
        std::cerr << "Error parsing ~local_player:\n";
        std::cerr << e.what() << '\n';
    }
    playerCount++;
}

void Parser::parseDigp(uint8_t *key, uint8_t *value, uint32_t valueSize) {
    Cursor keyCursor(key, 4);
    Cursor valueCursor(value);
    int x = keyCursor.readu32();
    int z = keyCursor.readu32();
    chunks.emplace_back(x, z, 0, json{});
    uint32_t entitySize = valueSize / 8;
    std::stringstream ss;
    for (int i = 0; i < entitySize; i++) {
        int64_t id = (int64_t)valueCursor.readu64();
        entities.emplace_back(id, x, z);
    }
}

void Parser::parseActorPrefix(uint8_t *key, uint8_t *value) {
    Cursor keyCursor(key);
    Cursor valueCursor(value);

    std::stringstream ss;
    int64_t id = (int64_t)keyCursor.readu64();
    // NBT::parseNBT(valueCursor,ss);
    json actorJson;
    try {
        // actorJson = json::parse(ss);
    } catch (json::exception e) {
        std::cerr << ss.str() << '\n';
        std::cerr << "Error parsing actorprefix:\n";
        std::cerr << e.what() << '\n';
    }
}

void Parser::parseChunk(uint8_t *key, uint8_t *value, uint32_t keySize) {
    Cursor keyCursor(key);
    Cursor valueCursor(value);

    json chunkJson;
    int x = (int)keyCursor.readu32();
    int z = (int)keyCursor.readu32();
    TAG record = (TAG)keyCursor.readu8();
    if ((x < 1875000) && (x > -1875000) && (z < 1875000) && (z > -1875000) && (record != TAG::SubChunkPrefix)) {
        chunks.emplace_back(x, z, 0, json{});
    } else if (record != TAG::SubChunkPrefix) {
        return;
    }

    if (record == TAG::BlockEntity || record == TAG::Entity) {
        std::stringstream ss;
        NBT::parseNBT(valueCursor, ss);
        json nbt;
        try {
            nbt = json::parse(ss);
            if (nbt.contains("id")) {
                nbt = {
                    { nbt["id"].get<std::string>(), nbt }
                };
            }
            blockEntities.emplace_back(x, z, nbt);
        } catch (json::exception e) {
            std::cerr << ss.str() << '\n';
            std::cerr << "Error parsing BlockEntity:\n";
            std::cerr << e.what() << '\n';
            std::cin.get();
        }
    } else if (record == TAG::Version) {
        uint8_t version = valueCursor.readu8();
    } else if (record == TAG::SubChunkPrefix) {
        std::stringstream ss;
        uint8_t version = valueCursor.readu8();
        uint8_t numStorage = 0;
        if (version == 1) {
            numStorage = 1;
        } else if (version < 8) {
            std::cerr << "Unsupported Version\n";
            assert(false);
            return;
        } else {
            numStorage = valueCursor.readu8();
        }
        int8_t subChunkIndex = 0;
        if (version >= 9) {
            subChunkIndex = (int8_t)valueCursor.readu8();
        }
        ss << '[';
        for (uint8_t i = 0; i < numStorage; i++) {
            // 1 byte flag
            uint8_t flags = valueCursor.readu8();
            uint8_t bitsPerBlock = flags >> 1;
            uint32_t blocksPerWord = floor(32.f / (float)bitsPerBlock);
            uint32_t blockStateCount = ceil(4096.f / (float)blocksPerWord);
            if (blockStateCount > 4096) {
                throw std::runtime_error("Invalid blockStateCount\n");
                return;
            }
            // Read Block Data
            std::vector<uint32_t> blockStateData(blockStateCount);
            for (uint32_t j = 0; j < blockStateCount; j++) {
                blockStateData.push_back(valueCursor.readu32());
            }
            ss << "{\"palette\":[";
            if (bitsPerBlock != 0) {
                uint32_t paletteSize = valueCursor.readu32();
                for (uint32_t j = 0; j < paletteSize; j++) {
                    NBT::parseNBT(valueCursor, ss);
                    if (j != (paletteSize - 1)) {
                        ss << ',';
                    }
                }
            }
            ss << "]";
            // Commented to save filesize of chunks.json
            // ss << ",\"blockData\":[";
            // for (uint32_t j = 0; j < blockStateCount; j++) {
            //     ss << blockStateData.at(j);
            //     if (j != (blockStateCount - 1)) {
            //         ss << ',';
            //     }
            // }
            // ss << ']';
            ss << '}';
            if (i != (numStorage - 1)) {
                ss << ',';
            }
        }
        ss << ']';
        json chunkJson;
        try {
            chunkJson = json::parse(ss);
            chunks.emplace_back(x, z, subChunkIndex, chunkJson);
        } catch (json::exception e) {
            std::cerr << "Error parsing subchunk:\n";
            std::cerr << e.what() << '\n';
            return;
        }
    }
}

void Parser::parseRemotePlayer(std::string key, uint8_t *value) {
    Cursor valueCursor(value);
    std::stringstream ss;
    NBT::parseNBT(valueCursor, ss);
    std::string filename = "worldData/" + key + ".json";
    try {
        playersJson[key] = json::parse(ss);
    } catch (json::exception e) {
        std::cerr << "Error parsing remote player:\n";
        std::cerr << e.what() << '\n';
    }
    playerCount++;
}

void Parser::parseDAT(const std::string &path) {
    liveLog("Parsing level.dat...");
    std::stringstream ss;
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open level.dat file\n";
        return;
    }
    uint32_t size = ifs.tellg();
    ifs.seekg(0);
    uint8_t *data = (uint8_t *)malloc(size * sizeof(uint8_t));
    ifs.read((char *)data, size);
    Cursor cursor(data);
    uint32_t header = cursor.readu32();
    uint32_t length = cursor.readu32();
    if (header != 10) {
        std::cerr << "Header:" << header << '\n';
        std::cerr << "Length:" << length << '\n';
        std::cerr << "Not a valid level.dat file\n";
        assert(false);
        return;
    }
    NBT::parseNBT(cursor, ss);
    free(data);
    ifs.close();

    std::ofstream ofs("worldData/level.json");
    json levelJson;
    try {
        levelJson = json::parse(ss);
    } catch (json::exception e) {
        std::cerr << "Error parsing level.dat:\n";
        std::cerr << e.what() << '\n';
        return;
    }
    ofs << levelJson.dump(4);
    ofs.close();
}

void Parser::drawChunkImage() {
    if (!chunks.size())
        return;
    liveLog("Drawing Chunk image...");
    std::vector<int> xcords = {};
    std::vector<int> zcords = {};
    for (const Chunk &chunk : chunks) {
        xcords.emplace_back(chunk.cx);
        zcords.emplace_back(chunk.cz);
    }

    std::sort(xcords.begin(), xcords.end());
    std::sort(zcords.begin(), zcords.end());

    // Remove Duplicate
    xcords.erase(std::unique(xcords.begin(), xcords.end()), xcords.end());
    zcords.erase(std::unique(zcords.begin(), zcords.end()), zcords.end());

    // offset to make minx=0 minz=0
    int xmin = xcords.at(0);
    int zmin = zcords.at(0);
    for (int i = 0; i < xcords.size(); i++) {
        xcords[i] -= xmin;
    }
    for (int i = 0; i < zcords.size(); i++) {
        zcords[i] -= zmin;
    }
    std::vector<std::pair<int, int>> offsetchunks = {};
    for (int i = 0; i < chunks.size(); i++) {
        offsetchunks.emplace_back(chunks[i].cx - xmin, chunks[i].cz - zmin);
    }

    int xdiff = abs(xcords.at(xcords.size() - 1) - xcords.at(0)) + 1;
    int zdiff = abs(zcords.at(zcords.size() - 1) - zcords.at(0)) + 1;

    int width = xdiff * 16;
    int height = zdiff * 16;
    // Render chunks on image
    uint32_t *image = (uint32_t *)malloc(width * height * sizeof(uint32_t));
    for (int i = 0; i < width * height; i++)
        image[i] = 0x00000000;
    for (const std::pair<int, int> &chunk : offsetchunks) {
        int xstart = chunk.first * 16;
        int zstart = chunk.second * 16;
        for (int x = xstart; x < (xstart + 16); x++) {
            for (int z = zstart; z < (zstart + 16); z++) {
                image[width * z + x] = 0xffff00ff; // pink
            }
        }
    }
    stbi_write_png("worldData/chunks.png", width, height, 4, image, width * 4);
    free(image);
}
void Parser::parseStructure(const std::string &filepath) {
    std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open structure file\n";
        return;
    }
    uint32_t size = ifs.tellg();
    ifs.seekg(0);
    uint8_t *data = (uint8_t *)malloc(size * sizeof(uint8_t));
    ifs.read((char *)data, size);

    Cursor cursor(data);
    std::stringstream ss;
    NBT::parseNBT(cursor, ss);
    ifs.close();
    try {
        json strucJson = json::parse(ss);
        // Convert to material list
        json::json_pointer ptr("/structure/block_indices");
        if (!strucJson.contains(ptr)) {
            std::cerr << "Json does not have \'structure\' or \'block_indices\'\n";
            return;
        }

        ptr = json::json_pointer("/structure/palette/default/block_palette");
        if (!strucJson.contains(ptr)) {
            std::cerr << "Json does not have a block palette\n";
            return;
        }
        std::vector<int> indices = {};
        for (const json &layer : strucJson["structure"]["block_indices"]) {
            for (const int &index : layer) {
                indices.emplace_back(index);
            }
        }
        uint32_t size = strucJson["structure"]["palette"]["default"]["block_palette"].size();
        std::vector<std::string> blocks = {};
        blocks.reserve(size);
        for (const json &block : strucJson["structure"]["palette"]["default"]["block_palette"]) {
            if (!block.contains("name")) {
                std::cerr << "Block does not have a name lol\n";
                return;
            }
            std::string name = block["name"].get<std::string>();
            if (name.starts_with("minecraft:")) {
                name = name.substr(10);
            }
            blocks.push_back(name);
        }

        std::map<std::string, uint32_t> materialList = {};

        for (uint32_t i = 0; i < size; i++) {
            if (blocks[i] == "air")
                continue;
            uint32_t blockCount = std::count(indices.begin(), indices.end(), i);
            materialList[blocks[i]] = blockCount;
        }

        std::cout << "Material List:\n";
        for (auto pair : materialList) {
            std::cout << pair.first << " : " << pair.second << "\n";
        }

    } catch (json::exception &e) {
        std::cerr << "Error parsing strucJson:\n";
        std::cerr << e.what() << "\n";
    }
}
void exportEntities(const std::vector<Entity> &entities, const std::string &path) {
    std::ofstream ofs(path);
    json entitiesJson;
    try {
        for (const Entity &entity : entities) {
            std::string key = std::to_string(entity.cx) + "_" + std::to_string(entity.cz);
            entitiesJson[key].push_back(entity.id);
        }
    } catch (json::exception e) {
        std::cerr << "Error parsing entityJson:\n";
        std::cerr << e.what() << '\n';
    }
    ofs << entitiesJson.dump(4);
    ofs.close();
}
void exportBlockEntities(const std::vector<BlockEntity> &blockEntities, const std::string &path) {
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open worldData/blockEntities.json\n";
        return;
    }
    json blockEntitiesJson;
    try {
        for (const BlockEntity &blockEntity : blockEntities) {
            std::string key = std::to_string(blockEntity.cx) + "_" + std::to_string(blockEntity.cz);
            blockEntitiesJson[key].push_back(blockEntity.nbt);
        }
    } catch (json::exception e) {
        std::cerr << "Error parsing blockEntitiesJson:\n";
        std::cerr << e.what() << '\n';
    }

    ofs << blockEntitiesJson.dump(4);
    ofs.close();
}
void exportChunks(const std::vector<Chunk> &chunks, const std::string &path) {
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open worldData/chunks.json\n";
        return;
    }
    json chunksJson;
    try {
        for (const Chunk &chunk : chunks) {
            std::string key = std::to_string(chunk.cx) + '_' + std::to_string(chunk.cz);
            if (!chunk.palette.empty()) {
                chunksJson[key][std::to_string(chunk.subchunkIndex)].push_back(std::move(chunk.palette));
            }
        }
    } catch (json::exception e) {
        std::cerr << "Error parsing chunksJson:\n";
        std::cerr << e.what() << '\n';
    }
    ofs << chunksJson.dump();
    ofs.close();
}
void exportPlayers(const json &playersJson, const std::string &path) {
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open worldData/players.json\n";
        return;
    }
    ofs << playersJson.dump(4);
    ofs.close();
}
Parser::~Parser() {
    Timer timer;
    timer.start();
    if (!chunks.size())
        return;
    // Export chunks
    liveLog("Exporting " + std::to_string(chunks.size()) + " chunks...");
    exportChunks(chunks, "worldData/chunks.json");

    // Export BlockEntities
    liveLog("Exporting " + std::to_string(blockEntities.size()) + " block entities...");
    exportBlockEntities(blockEntities, "worldData/blockEntities.json");

    // Export entities
    liveLog("Exporting " + std::to_string(entities.size()) + " entities...");
    exportEntities(entities, "worldData/entities.json");

    // Export Players
    liveLog("Exporting " + std::to_string(playerCount) + " players...");
    exportPlayers(playersJson, "worldData/players.json");

    liveLog("Exported.");
    double diff = timer.getDiff();
}
