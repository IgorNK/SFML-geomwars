cmake_minimum_required(VERSION 3.15)
project(sfmlgame
  LANGUAGES CXX
  VERSION 1.0
)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(dependencies/SFML)
set(SFML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/SFML/cmake)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui)
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
add_subdirectory(dependencies/imgui-sfml)
add_subdirectory(src)

