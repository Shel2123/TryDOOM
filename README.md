needs [XQuartz](https://www.xquartz.org/) or smth similar to run on macOS

build for windows on macOS


`cmake -S . -B build-win -G Ninja \                                                                                                                                            ─╯
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_TOOLCHAIN_FILE="Path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" \
-DVCPKG_TARGET_TRIPLET=x64-mingw-static \
-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="$PWD/cmake/toolchains/windows-mingw64.cmake" -DVCPKG_APPLOCAL_DEPS=OFF`

`cmake --build build-win `