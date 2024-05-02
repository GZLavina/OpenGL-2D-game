cmake_minimum_required(VERSION 3.27)
project(learnOpenGL)

set(CMAKE_CXX_STANDARD 20)

set(OUTPUT_DIR "${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

list(APPEND CMAKE_PREFIX_PATH Dependencies/Freetype)
include_directories(Dependencies/Include)
link_directories(Dependencies/Lib)

find_package(Freetype REQUIRED)

add_executable(learnOpenGL
        main.cpp
        glad.c
        shader.h
        Sprites/Sprite.h
        stb_image.cpp
        Sprites/Sprite.cpp
        shader.cpp
        Sprites/Player.cpp
        Sprites/Player.h
        GameValues.h
        Sprites/Fish.cpp
        Sprites/Fish.h
        Sprites/Trash.cpp
        Sprites/Trash.h
        Sprites/BackgroundLayer.cpp
        Sprites/BackgroundLayer.h
        TextRenderer.cpp
        TextRenderer.h
)
target_link_libraries(learnOpenGL glfw3 Freetype::Freetype)
