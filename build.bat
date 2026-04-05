@echo off
if not exist build mkdir build
pushd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
popd

build\FSmcedit.exe > world_data.txt