#ifndef UTILS_HPP
#define UTILS_HPP

#include <types.hpp>

/// Returns the most significant bit of __bits (32)
inline u8 get_msbit(const u32& bits) {
    return (bits & 0xF0000000) >> 28;
}

/// Returns the most significant bit of __bits (16)
inline u8 get_msbit(const u16& bits) {
    return (bits & 0xF000) >> 12;
}

/// Returns the most significant bit of __bits (8)
inline u8 get_msbit(const u8& bits) {
    return (bits & 0xF0) >> 4;
}

#endif