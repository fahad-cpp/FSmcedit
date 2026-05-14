if [ "$1" == "clean" ]; then
    rm -rf build
fi
mkdir -p build
cd build
[ ! -f CMakeCache.txt ] && cmake -S .. -G "Ninja" -DCMAKE_CXX_COMPILER="clang++" -DCMAKE_BUILD_TYPE="Release"
cmake --build . --config Release --parallel
cd ..
mkdir -p "worldData"
./build/FSmcedit > worldData/plain.txt
