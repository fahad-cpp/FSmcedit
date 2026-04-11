set(CMAKE_CXX_STANDARD 23)
cmake_minimum_required(VERSION 3.10)

project(FSmcedit)

file(GLOB SOURCES src/*.cpp)
add_executable(FSmcedit ${SOURCES})

# Mojang's Leveldb
add_subdirectory(vendor/leveldb)

add_subdirectory(vendor/json)

target_link_libraries(FSmcedit PRIVATE leveldb nlohmann_json::nlohmann_json)