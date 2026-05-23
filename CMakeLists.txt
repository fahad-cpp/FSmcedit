set(CMAKE_CXX_STANDARD 23)
cmake_minimum_required(VERSION 3.10)

project(FSmcedit)

file(GLOB SOURCES src/*.cpp)
add_executable(FSmcedit ${SOURCES})

add_subdirectory(vendor/zlib)
set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/zlib")
set(ZLIB_LIBRARY zlib)

# Mojang's Leveldb
add_subdirectory(vendor/leveldb)
add_subdirectory(vendor/json)

set_target_properties(FSmcedit PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
target_compile_options(FSmcedit PRIVATE -O3 -ffast-math -static -static-libgcc -static-libstdc++ -flto -fuse-ld=lld)
target_link_libraries(FSmcedit PRIVATE leveldb_mcpe nlohmann_json::nlohmann_json)
target_include_directories(FSmcedit PRIVATE vendor/leveldb/include)
