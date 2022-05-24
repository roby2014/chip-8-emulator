#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "types.hpp"

#define MEMORY_SIZE 4096
#define STACK_SIZE 16
#define TOTAL_REGISTERS 16
#define START_ADDR 512
#define DISPLAY_SIZE 64 * 32
#define MAX_INSTRUCTIONS 35
#define MAX_KEYS 16
#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32
#define SCALE_FACTOR 10

// CHIP-8 virtual machine implementation
class chip8 {
  private:
    std::array<u8, MEMORY_SIZE> _memory{};
    std::array<u8, TOTAL_REGISTERS> _v{};
    u16 _i{};
    u16 _pc{};
    u8 _sp{};
    std::array<u16, STACK_SIZE> _stack{};
    u8 _delay_timer{};
    u8 _sound_timer{};
    std::array<u32, DISPLAY_SIZE> _video{};
    std::array<u8, MAX_KEYS> _keypad{};
    u16 _opcode{};

    // opcode table
    struct opcode_member {
        u16 _opcode;
        u16 _mask;
        void (chip8::*_fn)();
    };
    std::array<struct opcode_member, MAX_INSTRUCTIONS> opcode_table{};

  public:
    chip8();
    ~chip8();

    /// Resets the "virtual machine" aKa all data (registers, video, etc)
    void reset_chip8();

    /// Loads fonts into memory
    void load_fonts();

    /// Loads ROM data into memory
    /// @param filename ROM file name/path
    /// Function exits the program if an error occurred
    void load_rom(const std::string& filename);

    /// Loads ROM data into memory
    /// @param raw_data ROM data, as raw bytes
    /// This function can be used to debug/test custom ROMs
    void load_rom(const std::vector<u8>& raw_data);

    /// Sets the keypad state
    /// @param key_idx key index
    /// @param state key state
    void set_key(u8 key_idx, bool state);

    /// Returns __video's pixel at address __idx
    /// This can be used on the draw function to check if we should draw
    u32 get_pixel(usize idx) const {
        return _video[idx];
    };

    /// Fetch, decode, execute...
    void run();

  public:
    /*********************
        CPU INSTRUCTIONS
    **********************/

    /// 00E0 - CLS
    /// Clear the display
    void cls();

    /// 00EE - RET
    /// Return from subroutine
    void ret();

    /// Not used by modern interpreters
    void sys();

    /// 1nnn - JP addr
    /// Jump to location nnn
    void jp();

    /// 2nnn - CALL addr
    /// Call subroutine at nnn
    void call();

    /// 3xkk - SE Vx, byte
    /// Skip next instruction if Vx = kk (if equal)
    void seq_kk();

    /// 4xkk - SNE Vx, byte
    /// Skip next instruction if Vx != kk
    void sne_kk();

    /// 5xy0 - SE Vx, Vy
    /// Skip next instruction if Vx = Vy (if registers equal)
    void seq();

    /// 6xkk - LD Vx, byte
    /// Set Vx = kk
    void ld_kk();

    /// 7xkk - ADD Vx, byte
    /// Set Vx = Vx + kk
    void add_kk();

    /// 8xy0 - LD Vx, Vy
    /// Set Vx = Vy
    void ld();

    /// 8xy1 - OR Vx, Vy
    /// Set Vx = Vx OR Vy
    void logic_or();

    /// 8xy2 - AND Vx, Vy
    /// Set Vx = Vx AND Vy
    void logic_and();

    /// 8xy3 - XOR Vx, Vy
    /// Set Vx = Vx XOR Vy
    void logic_xor();

    /// 8xy4 - ADD Vx, Vy
    /// Set Vx = Vx + Vy, set VF = carry
    void add();

    /// 8xy5 - SUB Vx, Vy
    /// Set Vx = Vx - Vy, set VF = NOT borrow
    void sub();

    /// 8xy6 - SHR Vx {, Vy}
    /// Set Vx = Vx SHR 1
    void shr();

    /// 8xy7 - SUBN Vx, Vy
    /// Set Vx = Vy - Vx, set VF = NOT borrow
    void subn();

    /// 8xyE - SHL Vx {, Vy}
    /// Set Vx = Vx SHL 1
    void shl();

    /// 9xy0 - SNE Vx, Vy
    /// Skip next instruction if Vx != Vy
    void sne();

    /// Annn - LD I, addr
    /// Set I = nnn
    void ld_i();

    /// Bnnn - JP V0, addr
    /// Jump to location nnn + V0 (jmp by offset)
    void jpo();

    /// Cxkk - RND Vx, byte
    /// Set Vx = random byte AND kk
    void rnd();

    /// Dxyn - DRW Vx, Vy, nibble
    /// Display n-byte sprite starting at mem location I at (Vx, Vy)
    /// Set VF = collision
    void drw();

    /// Ex9E - SKP Vx
    /// Skip next instruction if key with the value of Vx is pressed
    void skp();

    /// ExA1 - SKNP Vx
    /// Skip next instruction if key with the value of Vx is not pressed
    void sknp();

    /// Fx07 - LD Vx, DT
    /// Set Vx = delay timer value
    void ld_vx_dt();

    /// Fx0A - LD Vx, K
    /// Wait for a key press, store the value of the key in Vx
    void ld_k();

    /// Fx15 - LD DT, Vx
    /// Set delay timer = Vx
    void ld_dt();

    /// Fx18 - LD ST, Vx
    /// Set sound timer = Vx
    void ld_st();

    /// Fx1E - ADD I, Vx
    /// Set I = I + Vx
    void add_i();

    /// Fx29 - LD F, Vx
    /// Set I = location of sprite for digit Vx
    void ld_f();

    /// Fx33 - LD B, Vx
    /// Store BCD representation of Vx in memory locations I, I+1, and I+2
    void str_b();

    /// Fx55 - LD [I], Vx
    /// Store registers V0 through Vx in memory starting at location I
    void str_r();

    /// Fx65 - LD Vx, [I]
    /// Read registers V0 through Vx from memory starting at location I
    void read_r();
};

#endif
