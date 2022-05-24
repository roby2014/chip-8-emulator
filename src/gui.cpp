#include "gui.hpp"
#include "utils.hpp"
#include "chip8.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#ifdef _WIN32
#include <Windows.h>
#include <commdlg.h>
#endif

gui::gui(bool dbg)
    : _rom_loaded(false), _DEBUG_MODE(dbg),
      _window(screen_res_to_use<sf::VideoMode>(dbg), "CHIP-8 Emulator") {
    _window.setFramerateLimit(MAX_FPS);
    ImGui::SFML::Init(_window);
    _texture.create(CHIP8_WIDTH * SCALE_FACTOR, CHIP8_HEIGHT * SCALE_FACTOR);
}

gui::~gui() {
    ImGui::SFML::Shutdown();
}

void gui::registers_dock() {
    ImGui::Begin("Registers", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::End();
}

void gui::memory_dock() {
    ImGui::Begin("Memory", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::End();
}

void gui::keypad_dock(chip8* emu) {
    ImGui::Begin("Keypad", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    for (usize idx = 0; const auto& k : {0x1, 0x2, 0x3, 0xC, 0x4, 0x5, 0x6, 0xD, 0x7, 0x8, 0x9,
                                         0xE, 0xA, 0x0, 0xB, 0xF}) {
        ImGui::PushID(k);
        if (ImGui::Selectable(to_string(k).c_str(), emu->get_key_state(k) == 1, 0,
                              ImVec2{50, 50})) {
            emu->set_key_state(k, 1);
        } else {
            emu->set_key_state(k, 0);
        }
        ImGui::PopID();
        if (++idx % 4 != 0) {
            ImGui::SameLine();
        }
    }

    ImGui::End();
}

void gui::code_dock() {
    ImGui::Begin("Code", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    static char text[1024 * 16] = "/*\n"
                                  " int main() {\n"
                                  "   return 0;\n"
                                  " }\n"
                                  "label:\n"
                                  "\tlock cmpxchg8b eax\n";

    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", &flags, ImGuiInputTextFlags_ReadOnly);
    ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", &flags,
                         ImGuiInputTextFlags_AllowTabInput);
    ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text),
                              ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);

    ImGui::End();
}

void gui::emulator_dock(chip8* emu) {
    ImGui::Begin("Emulator", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    draw_emulator(emu, 0);
    ImGui::End();
}

void gui::show_main_menu_bar(chip8* emu) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load ROM", "Ctrl+O")) {
                char fname[1024];
#ifdef linux
                FILE* fp = popen("zenity --file-selection", "r");
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
                // TODO: clear everything from previous ROM
                emu->load_rom(fname);
                _rom_loaded = true;
            }
            ImGui::EndMenu();
        } else if (ImGui::MenuItem(_DEBUG_MODE ? "Normal mode" : "Debug Mode")) {
            _DEBUG_MODE = !_DEBUG_MODE;
            _window.setSize(screen_res_to_use<sf::Vector2u>(_DEBUG_MODE));
            _window.setPosition(sf::Vector2i(0, 0));
        } else if (ImGui::MenuItem("Exit")) {
            _window.close();
        }
        ImGui::EndMainMenuBar();
    }
}

void gui::draw_emulator(chip8* emu, const ImGuiWindowFlags& flags) {
    if (!_DEBUG_MODE) // only create a new imgui container if not in debug mode
        ImGui::Begin("Game", nullptr, flags);
    if (_rom_loaded) {
        emu->run();
        _texture.clear();
        for (usize row = 0, idx = 0; idx < DISPLAY_SIZE; idx++) {
            usize col = (idx % CHIP8_WIDTH);
            sf::RectangleShape pixel(sf::Vector2f(SCALE_FACTOR, SCALE_FACTOR));
            pixel.setPosition(col * SCALE_FACTOR, row * SCALE_FACTOR);
            pixel.setFillColor(emu->get_pixel(idx) == 1 ? sf::Color::White : sf::Color::Black);
            _texture.draw(pixel);

            if ((idx + 1) % CHIP8_WIDTH == 0) {
                row++;
            }
        }
        _texture.display();
        ImGui::Image(_texture);
    } else {
        ImGui::Text("No ROM loaded!");
    }
    if (!_DEBUG_MODE)
        ImGui::End();
}

void gui::display(chip8* emu) {
    _window.clear();

    show_main_menu_bar(emu);
    if (!_DEBUG_MODE) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoSavedSettings;
        draw_emulator(emu, flags);

    } else {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        code_dock();
        memory_dock();
        keypad_dock(emu);
        emulator_dock(emu);
        registers_dock();
    }

    ImGui::ShowDemoWindow();
    ImGui::SFML::Render(_window);
    _window.display();
}

void gui::handle_events(chip8* emu) {
    sf::Event event;
    while (_window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(_window, event);
        if (event.type == sf::Event::Closed) {
            _window.close();
        } else if (event.type == sf::Event::KeyPressed || sf::Event::KeyReleased) {
            // Keypad layout:
            // Keypad       Keyboard
            //+-+-+-+-+    +-+-+-+-+
            //|1|2|3|C|    |1|2|3|4|
            //+-+-+-+-+    +-+-+-+-+
            //|4|5|6|D|    |Q|W|E|R|
            //+-+-+-+-+ => +-+-+-+-+
            //|7|8|9|E|    |A|S|D|F|
            //+-+-+-+-+    +-+-+-+-+
            //|A|0|B|F|    |Z|X|C|V|
            //+-+-+-+-+    +-+-+-+-+
            bool state = event.type == sf::Event::KeyPressed; // press = 1, release = 0
            switch (event.key.code) {
            default:
                break;
            case sf::Keyboard::Key::Num1:
                emu->set_key_state(1, state);
                break;
            case sf::Keyboard::Key::Num2:
                emu->set_key_state(2, state);
                break;
            case sf::Keyboard::Key::Num3:
                emu->set_key_state(3, state);
                break;
            case sf::Keyboard::Key::Num4:
                emu->set_key_state(0xC, state);
                break;
            case sf::Keyboard::Key::Q:
                emu->set_key_state(4, state);
                break;
            case sf::Keyboard::Key::W:
                emu->set_key_state(5, state);
                break;
            case sf::Keyboard::Key::E:
                emu->set_key_state(6, state);
                break;
            case sf::Keyboard::Key::R:
                emu->set_key_state(0xD, state);
                break;
            case sf::Keyboard::Key::A:
                emu->set_key_state(7, state);
                break;
            case sf::Keyboard::Key::S:
                emu->set_key_state(8, state);
                break;
            case sf::Keyboard::Key::D:
                emu->set_key_state(9, state);
                break;
            case sf::Keyboard::Key::F:
                emu->set_key_state(0xE, state);
                break;
            case sf::Keyboard::Key::Z:
                emu->set_key_state(0xA, state);
                break;
            case sf::Keyboard::Key::X:
                emu->set_key_state(0, state);
                break;
            case sf::Keyboard::Key::C:
                emu->set_key_state(0xB, state);
                break;
            case sf::Keyboard::Key::V:
                emu->set_key_state(0xF, state);
                break;
            }
        }
    }

    sf::Clock deltaClock;
    auto& io = ImGui::GetIO();
    if (_DEBUG_MODE) {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }
    ImGui::SFML::Update(_window, deltaClock.restart());
}
