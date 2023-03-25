cd ..
mkdir build_release
cd build_release
conan install .. -s build_type=Release -s compiler="Visual Studio" -s compiler.version=16 -s compiler.runtime=MD --build=missing
cmake.exe -G "Visual Studio 16 2019" -A x64 ../utl_function_tests -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release
cd ..
cd build_scripts
