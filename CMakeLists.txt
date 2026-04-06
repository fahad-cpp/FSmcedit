set(CMAKE_CXX_STANDARD 23)
cmake_minimum_required(VERSION 3.10)

project(FSmcedit)

add_executable(FSmcedit Main.cpp)

add_subdirectory(vendor/leveldb)
add_subdirectory(vendor/json)

target_link_libraries(FSmcedit PRIVATE leveldb nlohmann_json::nlohmann_json)