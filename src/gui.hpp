#ifndef GUI_HPP
#define GUI_HPP
#include "chip8.hpp"
#include "imgui.h"
#include <SFML/Graphics.hpp>

#define MAX_FPS 60

class gui : public chip8 {
  private:
    bool _rom_loaded;
    bool _DEBUG_MODE;
    sf::RenderWindow _window;
    sf::RenderTexture _texture;

  public:
    gui(bool dbg);
    ~gui();

    /// Draws registers dock
    void registers_dock();

    /// Draws memory dock
    void memory_dock();

    /// Draws keypad dock
    void keypad_dock();

    /// Draws code dock
    void code_dock();

    /// Draws the emulator window
    void emulator_dock();

    /// Draws main menu bar
    void show_main_menu_bar();

    /// Displays everything
    void display();

    /// Draws the emulator window
    void draw_emulator(const ImGuiWindowFlags& flags);

    /// Handles events
    void handle_events();

    /// Returns true if the window is open
    bool running() const {
        return _window.isOpen();
    }
};
#endif