cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(chip8)

#project files
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(chip8 ${SRC_FILES})
target_include_directories(chip8 PUBLIC ${CMAKE_CURRENT_DIR}/src)

#SFML
find_package(SFML 2.5.1 REQUIRED system window graphics)
if(SFML_FOUND)
    target_include_directories(chip8 PUBLIC ${SFML_INCLUDE_DIR}) #FIXME
    target_link_libraries(chip8 sfml-system sfml-window sfml-graphics)
    message("[+] SFML LIBS LINKED + HEADERS INCLUDED")
else()
    message("[-] SFML PACKAGE NOT FOUND")
endif()

