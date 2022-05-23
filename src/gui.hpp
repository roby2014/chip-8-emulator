#ifndef GUI_HPP
#define GUI_HPP
#include "chip8.hpp"
#include <SFML/Graphics.hpp>

#define MAX_FPS 60
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 720
#define SCALE_FACTOR 10

class gui {
  private:
    bool _rom_loaded;
    bool _DEBUG_MODE;
    sf::RenderWindow _window;

  public:
    gui(bool dbg);
    ~gui();

    /// Draws registers dock
    void registers_dock();

    /// Draws memory dock
    void memory_dock();

    /// Draws code dock
    void code_dock();

    /// Draws main menu bar
    void show_main_menu_bar(chip8 *emu);

    /// Displays everything
    void display(chip8 *emu);

    /// Handles events
    void handle_events();

    /// Returns true if the window is open
    bool running() const { return _window.isOpen(); }
};
#endif