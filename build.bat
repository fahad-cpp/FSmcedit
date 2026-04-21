@echo off
if not exist build mkdir build
if exist build\Release\FSmcedit.exe del build\Release\FSmcedit.exe
pushd build
if "%1" == "clean" del CMakeCache.txt
if not exist CMakeCache.txt cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
popd
if exist worldData del /f /q "worldData"
if not exist worldData mkdir worldData
build\FSmcedit.exe