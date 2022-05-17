cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(chip8)

# project
set(IMGUI_SFML ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml/imgui-SFML.cpp)
file(GLOB_RECURSE SRC_FILES src/*.cpp ${IMGUI_SFML})
add_executable(chip8 ${SRC_FILES} )
target_include_directories(chip8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

# include imgui-sfml
target_include_directories(chip8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml)

# include + static lib imgui
add_library(imgui STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml)
target_include_directories(chip8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)

# sfml
find_package(SFML 2.5.1 REQUIRED system window graphics)
target_include_directories(chip8 PUBLIC ${SFML_INCLUDE_DIR}) #FIXME

# opengl (fixes some linking error from imgui-sfml lol)
find_package(OpenGL REQUIRED)

# link everything
target_link_libraries(chip8 sfml-system sfml-window sfml-graphics imgui ${OPENGL_LIBRARIES})
