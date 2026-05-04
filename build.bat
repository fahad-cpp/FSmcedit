@echo off
if not exist build mkdir build
if exist build\FSmcedit.exe del build\FSmcedit.exe
pushd build
if "%1" == "clean" del CMakeCache.txt
if not exist CMakeCache.txt cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
popd
if not exist worldData mkdir worldData
REM build\FSmcedit.exe
build\FSmcedit.exe > worldData/plain.txt