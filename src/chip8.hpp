#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

#include "utils.hpp"

#define MEMORY_SIZE     4096
#define STACK_SIZE      16
#define TOTAL_REGISTERS 16
#define START_ADDR      512
#define DISPLAY_SIZE    64 * 32

// chip-8 virtual machine implementation
class chip8 {
public:
    std::uint8_t memory[MEMORY_SIZE]{};
    std::uint32_t video[DISPLAY_SIZE]{};
    std::uint8_t v[TOTAL_REGISTERS]{};
    std::uint16_t i{};
    std::uint16_t pc{};
    std::uint8_t sp{};
    std::uint16_t stack[STACK_SIZE]{};
    std::uint8_t delay_timer{}, sound_timer{};
    std::uint16_t opcode{};

    chip8();
    ~chip8();

    /// Loads fonts into memory
    void load_fonts();

    /// Loads ROM data into memory
    /// @param filename ROM file name/path
    /// Function exits the program if an error occurred
    void load_rom(const std::string& filename);

    /// Reads opcodes and executes them
    void run();
};

#endif