#include <cstring>
#include <fstream>

#include "chip8.hpp"
#include "types.hpp"
#include "utils.hpp"

chip8::chip8() {
    std::srand((unsigned int)time(nullptr));

    load_fonts();
    _pc = START_ADDR;

    // clang-format off
    opcode_table = {{
        {0x00E0, 0xFFFF, &chip8::cls},       // 0x00E0
        {0x00EE, 0xFFFF, &chip8::ret},       // 0x00EE
        {0x0000, 0xF000, &chip8::sys},       // 0x0NNN
        {0x1000, 0xF000, &chip8::jp},        // 0x1NNN
        {0x2000, 0xF000, &chip8::call},      // 0x2NNN
        {0x3000, 0xF000, &chip8::seq_kk},    // 0x3XNN
        {0x4000, 0xF000, &chip8::sne_kk},    // 0x4XNN
        {0x5000, 0xF00F, &chip8::seq},       // 0x5XY0
        {0x6000, 0xF000, &chip8::ld_kk},     // 0x6XNN
        {0x7000, 0xF000, &chip8::add_kk},    // 0x7XNN
        {0x8000, 0xF00F, &chip8::ld},        // 0x8XY0
        {0x8001, 0xF00F, &chip8::logic_or},  // 0x8XY1
        {0x8002, 0xF00F, &chip8::logic_and}, // 0x8XY2
        {0x8003, 0xF00F, &chip8::logic_xor}, // 0x8XY3
        {0x8004, 0xF00F, &chip8::add},       // 0x8XY4
        {0x8005, 0xF00F, &chip8::sub},       // 0x8XY5
        {0x8006, 0xF00F, &chip8::shr},       // 0x8XY6
        {0x8007, 0xF00F, &chip8::subn},      // 0x8XY7
        {0x800E, 0xF00F, &chip8::shl},       // 0x8XYE
        {0x9000, 0xF00F, &chip8::sne},       // 0x9XY0
        {0xA000, 0xF000, &chip8::ld_i},      // 0xANNN
        {0xB000, 0xF000, &chip8::jpo},       // 0xBNNN
        {0xC000, 0xF000, &chip8::rnd},       // 0xCXNN
        {0xD000, 0xF000, &chip8::drw},       // 0xDXYN
        {0xE09E, 0xF0FF, &chip8::skp},       // 0xEX9E
        {0xE0A1, 0xF0FF, &chip8::sknp},      // 0xEXA1
        {0xF007, 0xF0FF, &chip8::ld_vx_dt},  // 0xFX07
        {0xF00A, 0xF0FF, &chip8::ld_k},      // 0xFX0A
        {0xF015, 0xF0FF, &chip8::ld_dt},     // 0xFX15
        {0xF018, 0xF0FF, &chip8::ld_st},     // 0xFX18
        {0xF01E, 0xF0FF, &chip8::add_i},     // 0xFX1E
        {0xF029, 0xF0FF, &chip8::ld_f},      // 0xFX29
        {0xF033, 0xF0FF, &chip8::str_b},     // 0xFX33
        {0xF055, 0xF0FF, &chip8::str_r},     // 0xFX55
        {0xF065, 0xF0FF, &chip8::read_r}}};  // 0xFX65
    // clang-format on
}

chip8::~chip8() {
}

void chip8::reset_chip8() {
    _memory.fill(0);
    _v.fill(0);
    _stack.fill(0);
    _keypad.fill(0);
    _video.fill(0);
    _i = 0;
    _pc = START_ADDR;
    _sp = 0;
    _delay_timer = 0;
    _sound_timer = 0;
    _opcode = 0;
}

void chip8::load_fonts() {
    // clang-format off
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
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    // clang-format on

    for (usize i = 0; i < fontset.size(); i++) {
        _memory[i] = fontset[i];
    }
}

void chip8::load_rom(const std::string& filename) {
    std::ifstream ifs(filename, std::ios_base::binary);
    if (!ifs || !ifs.is_open()) {
        fprintf(stderr, "[-] ROM %s not found \n", filename.c_str());
        exit(-1);
    }

    reset_chip8();

    char c;
    for (usize i = START_ADDR; ifs.get(c); i++) {
        if (i >= MEMORY_SIZE) {
            fprintf(stderr, "[-] %s's ROM data exceeds %d bytes \n", filename.c_str(),
                    MEMORY_SIZE);
            exit(-1);
        }
        _memory[i] = static_cast<u8>(c);
    }

    ifs.close();
}

void chip8::load_rom(const std::vector<u8>& raw_data) {
    for (usize idx = START_ADDR; const auto& b : raw_data) {
        _memory[idx] = b;
        idx++;
    }
}

void chip8::set_key(u8 key_idx, bool state) {
    if (key_idx < 0 || key_idx > 15) {
        return;
    }

    _keypad[key_idx] = (int)state;
}

void chip8::run() {
    _opcode = (_memory[_pc] << 8 | _memory[_pc + 1]);
    _pc += 2;

    bool found = false;
    for (const auto& op : opcode_table) {
        if (op._opcode == (_opcode & op._mask)) {
            found = true;
            (this->*(op._fn))();
            break;
        }
    }

    if (!found) {
        printf("[ERR] instruction/opcode implementation not found :(");
        return;
    }

    if (_delay_timer > 0) {
        _delay_timer--;
    }

    if (_sound_timer > 0) {
        _sound_timer--;
    }
}

void chip8::cls() {
    _video.fill(0);
}

void chip8::ret() {
    _pc = _stack[--_sp];
}

void chip8::sys() {
}

void chip8::jp() {
    _pc = get_nnn(_opcode);
}

void chip8::call() {
    _stack[_sp++] = _pc;
    _pc = get_nnn(_opcode);
}

void chip8::seq_kk() {
    if (_v[get_x(_opcode)] == get_kk(_opcode))
        _pc += 2;
}

void chip8::sne_kk() {
    if (_v[get_x(_opcode)] != get_kk(_opcode))
        _pc += 2;
}

void chip8::seq() {
    if (_v[get_x(_opcode)] == _v[get_y(_opcode)])
        _pc += 2;
}

void chip8::ld_kk() {
    _v[get_x(_opcode)] = get_kk(_opcode);
}

void chip8::add_kk() {
    _v[get_x(_opcode)] += get_kk(_opcode);
}

void chip8::ld() {
    _v[get_x(_opcode)] = _v[get_y(_opcode)];
}

void chip8::logic_or() {
    _v[get_x(_opcode)] |= _v[get_y(_opcode)];
}

void chip8::logic_and() {
    _v[get_x(_opcode)] &= _v[get_y(_opcode)];
}

void chip8::logic_xor() {
    _v[get_x(_opcode)] ^= _v[get_y(_opcode)];
}

void chip8::add() {
    u8 x = get_x(_opcode);
    u8 y = get_y(_opcode);
    u16 sum = _v[x] + _v[y];
    _v[0xF] = (sum > 0xFF ? 1 : 0);
    _v[x] = (sum & 0xFF);
}

void chip8::sub() {
    u8 x = get_x(_opcode);
    u8 y = get_y(_opcode);
    _v[0xF] = (_v[x] < _v[y] ? 1 : 0);
    _v[x] -= _v[y];
}

void chip8::shr() {
    u8 x = get_x(_opcode);
    _v[0xF] = (_v[x] & 1);
    _v[x] >>= 1;
}

void chip8::subn() {
    u8 x = get_x(_opcode);
    u8 y = get_y(_opcode);
    _v[0xF] = (_v[y] > _v[x] ? 1 : 0);
    _v[x] = _v[y] - _v[x];
}

void chip8::shl() {
    u8 x = get_x(_opcode);
    _v[0xF] = (_v[x] >> 7);
    _v[x] <<= 1;
}

void chip8::sne() {
    if (_v[get_x(_opcode)] != _v[get_y(_opcode)])
        _pc += 2;
}

void chip8::ld_i() {
    _i = get_nnn(_opcode);
}

void chip8::jpo() {
    _pc = get_nnn(_opcode) + _v[0];
}

void chip8::rnd() {
    _v[get_x(_opcode)] = (std::rand() % 0xFF) & get_kk(_opcode);
}

void chip8::drw() {
    _v[0xF] = 0;
    auto cords = point_t(_v[get_x(_opcode)] % CHIP8_WIDTH, _v[get_y(_opcode)] % CHIP8_HEIGHT);
    u8 n = get_lowest_nibble(_opcode);

    for (usize row = 0; row < n; row++) {
        u8 sprite = _memory[_i + row];

        for (usize col = 0; col < 8; col++) {
            auto idx = (cords.x + col) + ((cords.y + row) * CHIP8_WIDTH);
            u8 sprite_px = sprite & (0x80 >> col);
            u32* screen_px = &_video[idx];

            if (sprite_px) {
                if (*screen_px) {
                    _v[0xF] = 1;
                }
                *screen_px ^= 1;
            }
        }
    }
}

void chip8::skp() {
    if (_keypad[_v[get_x(_opcode)]])
        _pc += 2;
}

void chip8::sknp() {
    if (!_keypad[_v[get_x(_opcode)]])
        _pc += 2;
}

void chip8::ld_vx_dt() {
    _v[get_x(_opcode)] = _delay_timer;
}

void chip8::ld_k() {
    bool pressed = false;
    for (usize idx = 0; const auto& key : _keypad) {
        if (key == 1) {
            u8 x = get_x(_opcode);
            _v[x] = static_cast<u8>(idx);
            pressed = true;
            return;
        }
        idx++;
    }

    if (!pressed)
        _pc -= 2;
}

void chip8::ld_dt() {
    _delay_timer = get_x(_opcode);
}

void chip8::ld_st() {
    _sound_timer = get_x(_opcode);
}

void chip8::add_i() {
    _i += get_x(_opcode);
}

void chip8::ld_f() {
    _i = _v[get_x(_opcode)] * 5;
}

void chip8::str_b() {
    u8 val = _v[get_x(_opcode)];
    u8 ones = val % 10;
    val = val / 10;
    u8 tens = val % 10;
    u8 hundreds = val / 10;
    _memory[_i] = hundreds;
    _memory[_i + 1] = tens;
    _memory[_i + 2] = ones;
}

void chip8::str_r() {
    u8 x = get_x(_opcode);
    for (usize i = 0; i <= x; i++)
        _memory[_i + i] = _v[i];
}

void chip8::read_r() {
    u8 x = get_x(_opcode);
    for (usize i = 0; i <= x; i++)
        _v[i] = _memory[_i + i];
}
