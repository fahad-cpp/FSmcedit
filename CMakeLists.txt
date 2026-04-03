cmake_minimum_required(VERSION 3.10)

project(FSmcedit)

add_executable(FSmcedit Main.cpp)

add_subdirectory(vendor/leveldb)

target_link_libraries(FSmcedit PRIVATE leveldb)