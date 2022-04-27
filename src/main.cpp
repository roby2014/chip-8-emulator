#include <cstring>
#include <iostream>

#include "chip8.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./main <rom_file>" << std::endl;
        return 1;
    }

    chip8 vm;
    vm.load_rom(argv[1]);
    vm.run();

    return 0;
}
