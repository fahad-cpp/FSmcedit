@echo off
if not exist build mkdir build
pushd build
if "%1" == "clean" del CMakeCache.txt
if not exist CMakeCache.txt cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
popd

build\FSmcedit.exe > world_data.txt