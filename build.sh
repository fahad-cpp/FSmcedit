mkdir -p build
cd build
if [ "$1" == "clean" ]; then
    rm -f CMakeCache.txt
fi
[ ! -f CMakeCache.txt ] && cmake -S .. -DCMAKE_BUILD_TYPE="Release"
cmake --build . --config Release --parallel
cd ..
mkdir -p "worldData"
./build/FSmcedit > worldData/plain.txt