cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(chip8)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(chip8 ${SRC_FILES})
target_include_directories(chip8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# link imgui-sfml (it also links sfml automatically)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
add_subdirectory(external/imgui-sfml)
target_link_libraries(chip8 PRIVATE ImGui-SFML::ImGui-SFML)
