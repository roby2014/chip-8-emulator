#include "chip8.hpp"
#include "gui.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    bool dbg_mode = false;
    for (int i = 1; i < argc; ++i) {
        const std::string& arg = argv[i];
        if ((arg == "-d") || (arg == "--debug")) {
            dbg_mode = true;
        } else {
            std::cerr << "Usage: " << argv[0] << " <option(s)>"
                      << "Options:\n"
                      << "\t-h,--help\t\tShow this help message\n"
                      << "\t-d,--debug\tSpecify if program starts in debug mode" << std::endl;
            return 0;
        }
    }

    gui emu_gui{dbg_mode};
    while (emu_gui.running()) {
        emu_gui.handle_events();
        emu_gui.display();
    }
}