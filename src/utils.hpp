#ifndef UTILS_HPP
#define UTILS_HPP

#include <types.hpp>

/// Returns the highest nibble (4 bits) of __data
template <typename T>
inline u8 get_highest_nibble(const T& data) {
    return (data & (0xF0 << (sizeof(T) * 8 - 8))) >> (sizeof(T) * 8 - 4);
}

/// Returns the lowest nibble (4 bits) of __data
template <typename T>
inline u8 get_lowest_nibble(const T& data) {
    return (data & 0xF);
}

/// nnn or addr - Returns a 12-bit value, the lowest 12 bits of the instruction
inline u16 get_nnn(const u16& opcode) {
    return (opcode & 0xFFF);
}

/// x - A 4-bit value, the lower 4 bits of the high byte of the instruction
inline u8 get_x(const u16& opcode) {
    return (opcode & 0xF00) >> 8;
}

/// y - A 4-bit value, the upper 4 bits of the low byte of the instruction
inline u8 get_y(const u16& opcode) {
    return (opcode & 0xF0) >> 4;
}

/// kk or byte - An 8-bit value, the lowest 8 bits of the instruction
inline u8 get_kk(const u16& opcode) {
    return (opcode & 0xFF);
}

#endif