#include <cstring>
#include <iostream>

#include "chip8.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./main <rom_file>" << std::endl;
        return 1;
    }

    chip8 vm;

    if (1) {
        vm.load_rom(argv[1]);
    } else {
        vm.load_rom(std::initializer_list<u8>({0xF1, 0x0A, 0x44, 0x44}));
    }

    vm.run();

    return 0;
}
