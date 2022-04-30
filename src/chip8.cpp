// clang-format off
#include <cstring>
#include <fstream>

#include "chip8.hpp"
#include "types.hpp"
#include "utils.hpp"
// clang-format on

chip8::chip8() {
    load_fonts();
    pc = START_ADDR;
}

chip8::~chip8() {}

void chip8::load_fonts() {
    std::array<u8, 80> fontset = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80 // F
    };

    for (usize i = 0; i < fontset.size(); i++) {
        memory[i] = fontset[i];
    }
}

void chip8::load_rom(const std::string& filename) {
    std::ifstream ifs(filename, std::ios_base::binary);
    if (!ifs || !ifs.is_open()) {
        fprintf(stderr, "[-] ROM %s not found \n", filename.c_str());
        exit(-1);
    }

    char c;
    for (std::size_t i = START_ADDR; ifs.get(c); i++) {
        if (i >= MEMORY_SIZE) {
            fprintf(stderr, "[-] %s's ROM data exceeds %d bytes \n", filename.c_str(), MEMORY_SIZE);
            exit(-1);
        }
        memory[i] = static_cast<u8>(c);
    }

    ifs.close();
}

void chip8::run() {
    opcode_table = {{{/* 0x00E0 */ 0x00E0, 0xFFFF, &chip8::cls},
        {/* 0x00EE */ 0x00EE, 0xFFFF, &chip8::ret},
        {/* 0x0NNN */ 0x0000, 0xF000, &chip8::sys},
        {/* 0x1NNN */ 0x1000, 0xF000, &chip8::jp},
        {/* 0x2NNN */ 0x2000, 0xF000, &chip8::call},
        {/* 0x3XNN */ 0x3000, 0xF000, &chip8::seq_kk},
        {/* 0x4XNN */ 0x4000, 0xF000, &chip8::sne_kk},
        {/* 0x5XY0 */ 0x5000, 0xF00F, &chip8::seq},
        {/* 0x6XNN */ 0x6000, 0xF000, &chip8::ld_kk},
        {/* 0x7XNN */ 0x7000, 0xF000, &chip8::add_kk},
        {/* 0x8XY0 */ 0x8000, 0xF00F, &chip8::ld},
        {/* 0x8XY1 */ 0x8001, 0xF00F, &chip8::logic_or},
        {/* 0x8XY2 */ 0x8002, 0xF00F, &chip8::logic_and},
        {/* 0x8XY3 */ 0x8003, 0xF00F, &chip8::logic_xor},
        {/* 0x8XY4 */ 0x8004, 0xF00F, &chip8::add},
        {/* 0x8XY5 */ 0x8005, 0xF00F, &chip8::sub},
        {/* 0x8XY6 */ 0x8006, 0xF00F, &chip8::shr},
        {/* 0x8XY7 */ 0x8007, 0xF00F, &chip8::subn},
        {/* 0x8XYE */ 0x800E, 0xF00F, &chip8::shl},
        {/* 0x9XY0 */ 0x9000, 0xF00F, &chip8::sne},
        {/* 0xANNN */ 0xA000, 0xF000, &chip8::ld_i},
        {/* 0xBNNN */ 0xB000, 0xF000, &chip8::jpo},
        {/* 0xCXNN */ 0xC000, 0xF000, &chip8::rnd},
        {/* 0xDXYN */ 0xD000, 0xF000, &chip8::drw},
        {/* 0xEX9E */ 0xE09E, 0xF0FF, &chip8::skp},
        {/* 0xEXA1 */ 0xE0A1, 0xF0FF, &chip8::sknp},
        {/* 0xFX07 */ 0xF007, 0xF0FF, &chip8::ld_vx_dt},
        {/* 0xFX0A */ 0xF00A, 0xF0FF, &chip8::ld_k},
        {/* 0xFX15 */ 0xF015, 0xF0FF, &chip8::ld_dt},
        {/* 0xFX18 */ 0xF018, 0xF0FF, &chip8::ld_st},
        {/* 0xFX1E */ 0xF01E, 0xF0FF, &chip8::add_i},
        {/* 0xFX29 */ 0xF029, 0xF0FF, &chip8::ld_f},
        {/* 0xFX33 */ 0xF033, 0xF0FF, &chip8::str_b},
        {/* 0xFX55 */ 0xF055, 0xF0FF, &chip8::str_r},
        {/* 0xFX65 */ 0xF065, 0xF0FF, &chip8::read_r}}};

    for (int i = pc; i < pc + 20; i += 2) {
        opcode   = memory[i] << 8 | memory[i + 1];
        u8 msbit = get_msbit(opcode);
        printf("%02x %02x ; opcode = %04x ; msbit = %01x ", memory[i], memory[i + 1], opcode, msbit);


        // lookup on the table what we want to execute
        bool found = false;
        for (const auto& op : opcode_table) {
            if ((opcode & op._mask) == op._opcode) {
                found = true;
                (this->*(op._fn))();
                break;
            }
        }

        if (!found) {
            printf("[ERR] instruction/opcode implementation not found :(");
        }

        printf("\n");
    }
}

void chip8::cls() {
    video.fill(0);
}

void chip8::ret() {
    pc = stack[--sp];
}

void chip8::sys() {}

void chip8::jp() {
    pc = opcode & 0xFFF;
}

void chip8::call() {
    stack[sp++] = pc;
    pc          = opcode & 0xFFF;
}

void chip8::seq_kk() {
    u8 x  = (opcode & 0x0F00) >> 8;
    u8 kk = (opcode & 0xFF);
    pc    = pc + (v[x] == kk ? 2 : 0);
}

void chip8::sne_kk() {
    u8 x  = (opcode & 0xF00) >> 8;
    u8 kk = (opcode & 0xFF);
    pc    = pc + (v[x] != kk ? 2 : 0);
}

void chip8::seq() {
    u8 x = (opcode & 0xF00) >> 8;
    u8 y = (opcode & 0xF0) >> 4;
    pc   = pc + (v[x] == v[y] ? 2 : 0);
}

void chip8::ld_kk() {
    u8 x  = (opcode & 0xF00) >> 8;
    u8 kk = (opcode & 0xFF);
    v[x]  = kk;
}

void chip8::add_kk() {
    u8 x  = (opcode & 0xF00) >> 8;
    u8 kk = (opcode & 0xFF);
    v[x] += kk;
}

void chip8::ld() {
    u8 x = (opcode & 0xF00) >> 8;
    u8 y = (opcode & 0xF0) >> 4;
    v[x] = v[y];
}

void chip8::logic_or() {
    u8 x = (opcode & 0xF00) >> 8;
    u8 y = (opcode & 0xF0) >> 4;
    v[x] |= v[y];
}

void chip8::logic_and() {
    u8 x = (opcode & 0xF00) >> 8;
    u8 y = (opcode & 0xF0) >> 4;
    v[x] &= v[y];
}

void chip8::logic_xor() {
    u8 x = (opcode & 0xF00) >> 8;
    u8 y = (opcode & 0xF0) >> 4;
    v[x] ^= v[y];
}

void chip8::add() {
    u8 x    = (opcode & 0xF00) >> 8;
    u8 y    = (opcode & 0xF0) >> 4;
    u16 sum = v[x] + v[y];
    v[0xF]  = (sum > 0xFF ? 1 : 0);
    v[x] += (sum & 0xFF);
}

void chip8::sub() {
    u8 x   = (opcode & 0xF00) >> 8;
    u8 y   = (opcode & 0xF0) >> 4;
    v[0xF] = (v[x] < v[y] ? 1 : 0);
    v[x] -= v[y];
}

void chip8::shr() {
    u8 x = (opcode & 0xF00) >> 8;
    // u8 y   = (opcode & 0xF0) >> 4;
    v[0xF] = (v[x] & 0x1);
    v[x] >>= 1;
}

void chip8::subn() {
    u8 x   = (opcode & 0xF00) >> 8;
    u8 y   = (opcode & 0xF0) >> 4;
    v[0xF] = (v[y] > v[x] ? 1 : 0);
    v[x]   = v[y] - v[x];
}

void chip8::shl() {
    u8 x = (opcode & 0xF00) >> 8;
    u8 y = (opcode & 0xF0) >> 4;
    // TODO
    v[0xF] = v[x] & 0x0080;
    v[x] <<= 1;
}

void chip8::sne() {
    u8 x = (opcode & 0xF00) >> 8;
    u8 y = (opcode & 0xF0) >> 4;
    pc   = pc + (v[x] != v[y] ? 2 : 0);
}

void chip8::ld_i() {
    i = (opcode & 0xFFF);
}

void chip8::jpo() {
    pc = (opcode & 0xFFF) + v[0];
}

void chip8::rnd() {
    // TODO:
}

void chip8::drw() {
}

void chip8::skp() {
}
void chip8::sknp() {
}
void chip8::ld_vx_dt() {
}
void chip8::ld_k() {
}
void chip8::ld_dt() {
}
void chip8::ld_st() {
}
void chip8::add_i() {
}
void chip8::ld_f() {
}
void chip8::str_b() {
}
void chip8::str_r() {
}
void chip8::read_r() {
}