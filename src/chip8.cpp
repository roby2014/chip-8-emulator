#include <cstring>
#include <unistd.h>
#include <fstream>

#include "chip8.hpp"
#include "types.hpp"
#include "utils.hpp"

chip8::chip8() {
    std::srand((unsigned int) time(nullptr));

    load_fonts();
    pc = START_ADDR;

    opcode_table = {{{0x00E0, 0xFFFF, &chip8::cls}, // 0x00E0
        {0x00EE, 0xFFFF, &chip8::ret}, // 0x00EE
        {0x0000, 0xF000, &chip8::sys}, // 0x0NNN
        {0x1000, 0xF000, &chip8::jp}, // 0x1NNN
        {0x2000, 0xF000, &chip8::call}, // 0x2NNN
        {0x3000, 0xF000, &chip8::seq_kk}, // 0x3XNN
        {0x4000, 0xF000, &chip8::sne_kk}, // 0x4XNN
        {0x5000, 0xF00F, &chip8::seq}, // 0x5XY0
        {0x6000, 0xF000, &chip8::ld_kk}, // 0x6XNN
        {0x7000, 0xF000, &chip8::add_kk}, // 0x7XNN
        {0x8000, 0xF00F, &chip8::ld}, // 0x8XY0
        {0x8001, 0xF00F, &chip8::logic_or}, // 0x8XY1
        {0x8002, 0xF00F, &chip8::logic_and}, // 0x8XY2
        {0x8003, 0xF00F, &chip8::logic_xor}, // 0x8XY3
        {0x8004, 0xF00F, &chip8::add}, // 0x8XY4
        {0x8005, 0xF00F, &chip8::sub}, // 0x8XY5
        {0x8006, 0xF00F, &chip8::shr}, // 0x8XY6
        {0x8007, 0xF00F, &chip8::subn}, // 0x8XY7
        {0x800E, 0xF00F, &chip8::shl}, // 0x8XYE
        {0x9000, 0xF00F, &chip8::sne}, // 0x9XY0
        {0xA000, 0xF000, &chip8::ld_i}, // 0xANNN
        {0xB000, 0xF000, &chip8::jpo}, // 0xBNNN
        {0xC000, 0xF000, &chip8::rnd}, // 0xCXNN
        {0xD000, 0xF000, &chip8::drw}, // 0xDXYN
        {0xE09E, 0xF0FF, &chip8::skp}, // 0xEX9E
        {0xE0A1, 0xF0FF, &chip8::sknp}, // 0xEXA1
        {0xF007, 0xF0FF, &chip8::ld_vx_dt}, // 0xFX07
        {0xF00A, 0xF0FF, &chip8::ld_k}, // 0xFX0A
        {0xF015, 0xF0FF, &chip8::ld_dt}, // 0xFX15
        {0xF018, 0xF0FF, &chip8::ld_st}, // 0xFX18
        {0xF01E, 0xF0FF, &chip8::add_i}, // 0xFX1E
        {0xF029, 0xF0FF, &chip8::ld_f}, // 0xFX29
        {0xF033, 0xF0FF, &chip8::str_b}, // 0xFX33
        {0xF055, 0xF0FF, &chip8::str_r}, // 0xFX55
        {0xF065, 0xF0FF, &chip8::read_r}}}; // 0xFX65
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
    for (usize i = START_ADDR; ifs.get(c); i++) {
        if (i >= MEMORY_SIZE) {
            fprintf(stderr, "[-] %s's ROM data exceeds %d bytes \n", filename.c_str(), MEMORY_SIZE);
            exit(-1);
        }
        memory[i] = static_cast<u8>(c);
    }

    ifs.close();
}

void chip8::load_rom(const std::vector<u8>& raw_data) {
    for (usize idx = START_ADDR; const auto& b : raw_data) {
        memory[idx] = b;
        idx++;
    }
}

void chip8::run() {
    opcode = (memory[pc] << 8 | memory[pc + 1]);

    u8 hnibble = get_highest_nibble(opcode);
    // printf("%02x %02x ; opcode = %04x ; nibble = %01x ", memory[pc], memory[pc + 1], opcode, hnibble);

    pc += 2;

    bool found = false;
    for (const auto& op : opcode_table) {
        if (op._opcode == (opcode & op._mask)) {
            found = true;
            (this->*(op._fn))();
            break;
        }
    }

    if (!found) {
        printf("[ERR] instruction/opcode implementation not found :(");
    }

    // printf("\n");
}

void chip8::cls() {
    video.fill(0);
}

void chip8::ret() {
    pc = stack[--sp];
}

void chip8::sys() {}

void chip8::jp() {
    pc = get_nnn(opcode);
}

void chip8::call() {
    stack[sp++] = pc;
    pc          = get_nnn(opcode);
}

void chip8::seq_kk() {
    u8 x  = get_x(opcode);
    u8 kk = get_kk(opcode);
    if (v[x] == kk)
        pc += 2;
}

void chip8::sne_kk() {
    u8 x  = get_x(opcode);
    u8 kk = get_kk(opcode);
    if (v[x] != kk)
        pc += 2;
}

void chip8::seq() {
    u8 x = get_x(opcode);
    u8 y = get_y(opcode);
    if (v[x] == v[y])
        pc += 2;
}

void chip8::ld_kk() {
    v[get_x(opcode)] = get_kk(opcode);
}

void chip8::add_kk() {
    v[get_x(opcode)] += get_kk(opcode);
}

void chip8::ld() {
    v[get_x(opcode)] = v[get_y(opcode)];
}

void chip8::logic_or() {
    v[get_x(opcode)] |= v[get_y(opcode)];
}

void chip8::logic_and() {
    v[get_x(opcode)] &= v[get_y(opcode)];
}

void chip8::logic_xor() {
    v[get_x(opcode)] ^= v[get_y(opcode)];
}

void chip8::add() {
    u8 x    = get_x(opcode);
    u8 y    = get_y(opcode);
    u16 sum = v[x] + v[y];
    v[0xF]  = (sum > 0xFF ? 1 : 0);
    v[x]    = (sum & 0xFF);
}

void chip8::sub() {
    u8 x   = get_x(opcode);
    u8 y   = get_y(opcode);
    v[0xF] = (v[x] < v[y] ? 1 : 0);
    v[x] -= v[y];
}

void chip8::shr() {
    u8 x   = get_x(opcode);
    v[0xF] = (v[x] & 1);
    v[x] >>= 1;
}

void chip8::subn() {
    u8 x   = get_x(opcode);
    u8 y   = get_y(opcode);
    v[0xF] = (v[y] > v[x] ? 1 : 0);
    v[x]   = v[y] - v[x];
}

void chip8::shl() {
    u8 x   = get_x(opcode);
    v[0xF] = (v[x] >> 7);
    v[x] <<= 1;
}

void chip8::sne() {
    u8 x = get_x(opcode);
    u8 y = get_y(opcode);
    if (v[x] != v[y])
        pc += 2;
}

void chip8::ld_i() {
    i = get_nnn(opcode);
}

void chip8::jpo() {
    pc = get_nnn(opcode) + v[0];
}

void chip8::rnd() {
    u8 x  = get_x(opcode);
    u8 kk = get_kk(opcode);
    v[x]  = (std::rand() % 0xFF) & kk;
}

void chip8::drw() {
    v[0xF]     = 0;
    auto cords = point_t(v[get_x(opcode)] % CHIP8_WIDTH, v[get_y(opcode)] % CHIP8_HEIGHT);
    u8 n       = get_lowest_nibble(opcode);

    for (usize row = 0; row < n; row++) {
        u8 sprite = memory[i + row];

        for (usize col = 0; col < 8; col++) {
            auto idx       = (cords.x + col) + ((cords.y + row) * CHIP8_WIDTH);
            u8 sprite_px   = sprite & (0x80 >> col);
            u32* screen_px = &video[idx];

            if (sprite_px) {
                if (*screen_px) {
                    v[0xF] = 1;
                }
                *screen_px ^= 1;
            }
        }
    }
}

void chip8::skp() {
    u8 x = get_x(opcode);
    if (keypad[v[x]])
        pc += 2;
}

void chip8::sknp() {
    u8 x = get_x(opcode);
    if (!keypad[v[x]])
        pc += 2;
}

void chip8::ld_vx_dt() {
    u8 x = get_x(opcode);
    v[x] = delay_timer;
}

void chip8::ld_k() {
    bool pressed = false;
    for (usize idx = 0; const auto& key : keypad) {
        if (key == 1) {
            u8 x    = get_x(opcode);
            v[x]    = idx;
            pressed = true;
            return;
        }
        idx++;
    }

    if (!pressed)
        pc -= 2;
}

void chip8::ld_dt() {
    delay_timer = get_x(opcode);
}

void chip8::ld_st() {
    sound_timer = get_x(opcode);
}

void chip8::add_i() {
    i += get_x(opcode);
}

void chip8::ld_f() {
    i = v[get_x(opcode)] * 5;
}

void chip8::str_b() {
    u8 val        = v[get_x(opcode)];
    u8 ones       = val % 10;
    val           = val / 10;
    u8 tens       = val % 10;
    u8 hundreds   = val / 10;
    memory[i]     = hundreds;
    memory[i + 1] = tens;
    memory[i + 2] = ones;
}

void chip8::str_r() {
    u8 x = get_x(opcode);
    for (usize idx = 0; idx <= x; idx++)
        memory[i + idx] = v[idx];
}

void chip8::read_r() {
    u8 x = get_x(opcode);
    for (usize idx = 0; idx <= x; idx++)
        v[idx] = memory[i + idx];
}