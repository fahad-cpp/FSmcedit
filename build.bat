@echo off
if not exist build mkdir build
if exist bin\FSmcedit.exe del bin\FSmcedit.exe
pushd build
if "%1" == "clean" del CMakeCache.txt
if not exist CMakeCache.txt cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build . --config Release --parallel
popd
if not exist worldData mkdir worldData
@echo on
bin\FSmcedit.exe shouldBeIgnored --world tmp\testworld --struct TestStructure.mcstructure
