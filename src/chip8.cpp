#include "chip8.hpp"

chip8::chip8() {
    load_fonts();
    pc = START_ADDR;
}

chip8::~chip8() {}

void chip8::load_fonts() {
    constexpr int FONTSET_SIZE         = 80;
    std::uint8_t fontset[FONTSET_SIZE] = {
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

    for (std::size_t i = 0; i < FONTSET_SIZE; i++) {
        memory[i] = fontset[i];
    }
}

void chip8::load_rom(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs || !ifs.is_open()) {
        std::cerr << "[ERROR] ROM " << filename << " not found.\n";
        exit(-1);
    }

    char c;
    for (std::size_t i = START_ADDR; ifs.get(c); i++) {
        if (i >= MEMORY_SIZE) {
            std::cerr << "[ERROR] " << filename << "'s ROM data exceeds " << MEMORY_SIZE << " bytes.\n";
            exit(-1);
        }
        memory[i] = static_cast<std::uint8_t>(c);
    }

    ifs.close();
}

void chip8::run() {
    for (int i = pc; i < pc + 20; i += 2) {
        opcode             = memory[i] << 8 | memory[i + 1];
        std::uint8_t msbit = get_msbit<std::uint16_t>(opcode, 0xF000, 12);
        printf("%02x %02x ; opcode = %04x ; msbit = %01x ", memory[i], memory[i + 1], opcode, msbit);

        switch (msbit) {
            // 0nnn - SYS - Jump to a machine code routine at nnn
            // 00E0 - CLS - Clear the display
            // 00EE - RET - Return from subroutine
        case 0:
            if (opcode == 0x00E0) {
                std::memset(video, 0, sizeof(video));
            } else if (opcode == 0x00EE) {
                pc = stack[--sp];
                pc += 2;
            }
            break;
        default:
            printf("unknown msbit ????");
        }

        printf("\n");
    }
}