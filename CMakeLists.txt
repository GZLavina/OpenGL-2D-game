cmake_minimum_required(VERSION 3.27)
project(learnOpenGL)

set(CMAKE_CXX_STANDARD 20)
set(USER_HOME_DIRECTORY $ENV{USERPROFILE})

include_directories(Dependencies/Include)
link_directories(Dependencies/Lib)

add_executable(learnOpenGL main.cpp
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
)
target_link_libraries(learnOpenGL glfw3)
