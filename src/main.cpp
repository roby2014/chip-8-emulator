#include <iostream>
#include "chip8.hpp"
#include "gui.hpp"

int main(int argc, char **argv) {
    bool dbg_mode = true;
    if (argc == 2) {
        // TODO: -d arg
        dbg_mode = argv[1][0] == '0' ? false : true;
    }

    gui emu_gui(dbg_mode);
    chip8 emu;

    while (emu_gui.running()) {
        emu_gui.handle_events();
        emu_gui.display(&emu);
    }
}