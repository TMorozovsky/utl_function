cd ..
mkdir build
cd build
conan install .. --build=missing
cmake ../utl_function_tests
cd ..
cd build_scripts
