set(CMAKE_CXX_STANDARD 23)
cmake_minimum_required(VERSION 3.10)
add_definitions(-DDLLX=)
project(FSmcedit)

file(GLOB SOURCES src/*.cpp)
add_executable(FSmcedit ${SOURCES})

add_subdirectory(vendor/zlib)
set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/zlib")
set(ZLIB_LIBRARY zlib)

# Mojang's Leveldb
add_subdirectory(vendor/leveldb)

add_subdirectory(vendor/json)

target_link_libraries(FSmcedit PRIVATE leveldb_mcpe nlohmann_json::nlohmann_json)
target_include_directories(FSmcedit PRIVATE vendor/leveldb/include)