#ifndef TYPES_HPP
#define TYPES_HPP

#include <SFML/Graphics.hpp>

// :D

using usize = size_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// evil? maybe
#define MONITOR_WIDTH sf::VideoMode::getDesktopMode().width
#define MONITOR_HEIGHT sf::VideoMode::getDesktopMode().height

/// Represents a "point" by (x,y)
typedef struct point { u32 x, y; } point_t;

#endif