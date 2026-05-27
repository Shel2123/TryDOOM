cmake_minimum_required(VERSION 3.20)
project(TryDOOM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(TryDOOM
        src/main.cpp
        src/app.cpp
        src/renderer.cpp
        src/raycaster.cpp
)

target_compile_options(TryDOOM PRIVATE
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(TryDOOM PRIVATE SDL3::SDL3)

find_package(glad CONFIG REQUIRED)
target_link_libraries(TryDOOM PRIVATE glad::glad)

if (WIN32)
    target_link_libraries(TryDOOM PRIVATE opengl32 user32 gdi32 winmm imm32 version setupapi)
    if (MINGW)
        target_link_options(TryDOOM PRIVATE -static -static-libgcc -static-libstdc++)
    endif ()
else ()
    find_package(OpenGL REQUIRED)
    target_link_libraries(TryDOOM PRIVATE OpenGL::GL)
endif ()

option(ENABLE_SANITIZERS "Enable ASan + UBSan" OFF)
if (ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(TryDOOM PRIVATE -fsanitize=address,undefined)
    target_link_options(TryDOOM PRIVATE -fsanitize=address,undefined)
endif ()
