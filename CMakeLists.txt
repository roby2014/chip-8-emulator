cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(chip8)

# project sources
set(IMGUI_SFML ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml/imgui-SFML.cpp)
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(chip8 ${SRC_FILES} ${IMGUI_SFML})

# imgui setup
add_library(imgui STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
)
target_include_directories(imgui 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml
)
target_link_libraries(imgui 
    PUBLIC 
    sfml-system 
    sfml-window 
    sfml-graphics
)

# sfml
find_package(SFML 2.5.1 COMPONENTS system window graphics REQUIRED)

# opengl (fixes some linking error from imgui-sfml lol)
find_package(OpenGL REQUIRED)

# include headers
target_include_directories(chip8 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
)

# link everything
target_link_libraries(chip8 
    sfml-system 
    sfml-window 
    sfml-graphics 
    imgui 
    ${OPENGL_LIBRARIES}
)
