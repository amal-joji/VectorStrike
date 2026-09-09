cmake_minimum_required(VERSION 3.20)

project(VectorStrike LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(VectorStrike
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Bullet.cpp
)

target_include_directories(VectorStrike PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

target_link_libraries(VectorStrike PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)