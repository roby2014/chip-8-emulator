#include "gui.hpp"
#include "chip8.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#ifdef _WIN32
#include <Windows.h>
#include <commdlg.h>
#endif

gui::gui(bool dbg)
    : _rom_loaded(false), _DEBUG_MODE(dbg),
      _window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "CHIP-8 Emulator") {
    _window.setFramerateLimit(MAX_FPS);
    ImGui::SFML::Init(_window);
}

gui::~gui() {
    ImGui::SFML::Shutdown();
}

void gui::registers_dock() {
    ImGui::Begin("Registers");
    ImGui::End();
}

void gui::memory_dock() {
    ImGui::Begin("Memory");
    ImGui::End();
}

void gui::code_dock() {
    ImGui::Begin("Code");

    static char text[1024 * 16] = "/*\n"
                                  " int main() {\n"
                                  "   return 0;\n"
                                  " }\n"
                                  "label:\n"
                                  "\tlock cmpxchg8b eax\n";

    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", &flags,
                         ImGuiInputTextFlags_ReadOnly);
    ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", &flags,
                         ImGuiInputTextFlags_AllowTabInput);
    ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text),
                              ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16),
                              flags);

    ImGui::End();
}

void gui::show_main_menu_bar(chip8 *emu) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load ROM", "Ctrl+O")) {
                char fname[1024];
#ifdef linux
                FILE *fp = popen("zenity --file-selection", "r");
                fgets(fname, sizeof(fname), fp);
                if (fname[strlen(fname) - 1] == '\n') {
                    fname[strlen(fname) - 1] = 0;
                }
#elif _WIN32
                // FIXME
                OPENFILENAMEA f;
                f.lStructSize = sizeof(f);
                f.hwndOwner = GetActiveWindow();
                f.lpstrFile = fname;
                f.nMaxFile = sizeof(fname);
                if (!GetOpenFileNameA(&f)) {
                    return;
                }
#endif
                emu->load_rom(fname);
                _rom_loaded = true;
            }
            ImGui::EndMenu();
        } else if (ImGui::MenuItem(_DEBUG_MODE ? "Normal mode"
                                               : "Debug Mode")) {
            _DEBUG_MODE = !_DEBUG_MODE;
        } else if (ImGui::MenuItem("Exit")) {
            _window.close();
        }
        ImGui::EndMainMenuBar();
    }
}

void gui::display(chip8 *emu) {
    _window.clear();

    show_main_menu_bar(emu);
    if (!_DEBUG_MODE) {
        // ImGui::ShowDemoWindow();
        if (_rom_loaded) {
            ImGui::Text("Rom is loaded");
            emu->run();
            auto row = 0;
            sf::Sprite sprite;
            for (usize idx = 0; idx < DISPLAY_SIZE; idx++) {
                auto col = (idx % CHIP8_WIDTH);

                sf::RectangleShape pixel(
                    sf::Vector2f(SCALE_FACTOR, SCALE_FACTOR));
                pixel.setPosition(col * SCALE_FACTOR, row * SCALE_FACTOR);
                pixel.setFillColor(emu->get_pixel(idx) == 1 ? sf::Color::White
                                                            : sf::Color::Black);
                // window.draw(pixel);

                if ((idx + 1) % CHIP8_WIDTH == 0) {
                    row++;
                }
            }
        }
    } else {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        code_dock();
        memory_dock();
        registers_dock();
    }

    ImGui::SFML::Render(_window);
    _window.display();
}

void gui::handle_events() {
    sf::Event event;
    while (_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(_window, event);
        if (event.type == sf::Event::Closed) {
            _window.close();
        }
    }

    sf::Clock deltaClock;
    auto &io = ImGui::GetIO();
    if (_DEBUG_MODE) {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }
    ImGui::SFML::Update(_window, deltaClock.restart());
}
