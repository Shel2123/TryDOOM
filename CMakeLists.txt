#cmake_minimum_required(VERSION 4.1)
#project(TryDOOM LANGUAGES C CXX)
#
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
#add_executable(TryDOOM
#        src/main.cpp
#        src/common.h
#        src/graphics.cpp
#        src/map.cpp
#        src/map.h
#        src/player.cpp
#        src/player.h
#        src/controls.cpp
#        src/controls.h
#        src/graphics.h
#        src/helpers.cpp
#        src/helpers.h
#)
#
## --- OpenGL ---
#find_package(OpenGL REQUIRED)
#target_link_libraries(TryDOOM PRIVATE OpenGL::GL)
#
## --- FreeGLUT: build it from source, statically ---
#include(FetchContent)
#
#FetchContent_Declare(freeglut
#        GIT_REPOSITORY https://github.com/freeglut/freeglut.git
#        GIT_TAG v3.8.0
#)
#
#set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
#set(FREEGLUT_BUILD_STATIC_LIBS ON  CACHE BOOL "" FORCE)
#set(FREEGLUT_BUILD_DEMOS OFF       CACHE BOOL "" FORCE)
#
#FetchContent_MakeAvailable(freeglut)
#
#target_include_directories(TryDOOM PRIVATE
#        ${freeglut_SOURCE_DIR}/include
#)
#
#if(TARGET freeglut_static)
#    target_link_libraries(TryDOOM PRIVATE freeglut_static)
#    target_compile_definitions(TryDOOM PRIVATE FREEGLUT_STATIC)
#elseif(TARGET freeglut)
#    target_link_libraries(TryDOOM PRIVATE freeglut)
#else()
#    message(FATAL_ERROR "freeglut target not found after FetchContent")
#endif()
#
#
## --- Make the EXE more self-contained when using MinGW ---
#if(MINGW)
#    target_compile_options(TryDOOM PRIVATE -ffunction-sections -fdata-sections)
#    target_link_options(TryDOOM PRIVATE
#            -Wl,--gc-sections
#            -static-libgcc
#            -static-libstdc++
#    )
#    # Optional: strip symbols in Release for smaller exe
#    target_link_options(TryDOOM PRIVATE $<$<CONFIG:Release>:-s>)
#endif()



cmake_minimum_required(VERSION 4.1)
project(TryDOOM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TryDOOM
        src/main.cpp
        src/common.h
        src/graphics.cpp
        src/map.cpp
        src/map.h
        src/player.cpp
        src/player.h
        src/controls.cpp
        src/controls.h
        src/graphics.h
        src/helpers.cpp
        src/helpers.h
        src/renderer2D.h
        src/logger.h
        src/logger.cpp
        src/app.h
        src/app.cpp)

find_package(OpenGL REQUIRED)
target_link_libraries(TryDOOM PRIVATE OpenGL::GL)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(TryDOOM PRIVATE SDL3::SDL3)

find_package(glad CONFIG REQUIRED)
target_link_libraries(TryDOOM PRIVATE glad::glad)

include(FetchContent)

find_package(FreeGLUT CONFIG REQUIRED)
target_link_libraries(TryDOOM PRIVATE
        $<IF:$<TARGET_EXISTS:FreeGLUT::freeglut>,FreeGLUT::freeglut,FreeGLUT::freeglut_static>
)

