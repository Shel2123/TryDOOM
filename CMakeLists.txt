################### WINDOWS BUILD ####################

#cmake_minimum_required(VERSION 4.1)
#project(TryDOOM LANGUAGES CXX)
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
#        src/renderer2D.h
#        src/logger.h
#        src/logger.cpp
#        src/app.h
#        src/app.cpp
#        src/input.h
#        src/services.h
#        src/scene.h
#        src/scene_stack.h
#        src/game_scene.h
#        src/game_scene.cpp
#        src/pause_scene.cpp
#        src/map_select_scene.cpp
#)
#
#find_package(SDL3 CONFIG REQUIRED)
#find_package(glad CONFIG REQUIRED)
#
#target_link_libraries(TryDOOM PRIVATE
#        SDL3::SDL3
#        glad::glad
#)
#
#if (WIN32)
#    target_link_libraries(TryDOOM PRIVATE opengl32)
#
#    target_link_libraries(TryDOOM PRIVATE user32 gdi32 winmm imm32 version setupapi)
#
#    # if(TARGET SDL3::SDL3main)
#    #   target_link_libraries(TryDOOM PRIVATE SDL3::SDL3main)
#    # endif()
#else()
#    find_package(OpenGL REQUIRED)
#    target_link_libraries(TryDOOM PRIVATE OpenGL::GL)
#endif()
#
#if (MINGW)
#    target_link_options(TryDOOM PRIVATE
#            -static
#            -static-libgcc
#            -static-libstdc++
#    )
#endif()


#################### MACOS BUILD ######################
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
        src/app.cpp
        src/input.h
        src/services.h
        src/scene.h
        src/scene_stack.h
        src/game_scene.h
        src/game_scene.cpp
        src/pause_scene.cpp
        src/map_select_scene.cpp)

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

