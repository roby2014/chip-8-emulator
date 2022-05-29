#include "gui.hpp"
#include "chip8.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include "imgui_memory_editor.h"
#include "utils.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <commdlg.h>
#endif

gui::gui(bool dbg)
    : _rom_loaded(false), _DEBUG_MODE(dbg),
      _window(screen_res_to_use<sf::VideoMode>(dbg), "CHIP-8 Emulator") {
    _window.setFramerateLimit(MAX_FPS);
    _window.setPosition(sf::Vector2i(0, 0));
    ImGui::SFML::Init(_window);
    _texture.create(CHIP8_WIDTH * SCALE_FACTOR, CHIP8_HEIGHT * SCALE_FACTOR);
}

gui::~gui() {
    ImGui::SFML::Shutdown();
}

void gui::registers_dock() {
    // make some copies (used to determine if value changed so we change color)
    static auto copy_of_v = _v;
    static auto copy_of_i = _i;
    static auto copy_of_pc = _pc;
    static auto copy_of_sp = _sp;
    static auto copy_of_stack = _stack;
    static auto copy_of_delay_timer = _delay_timer;
    static auto copy_of_sound_timer = _sound_timer;

    auto white = ImVec4(255.0f, 255.0f, 255.0f, 1.0f);
    auto red = ImVec4(255.0f, 0.0f, 0.0f, 1.0f);
    auto flags = ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly;

    ImGui::Begin("Registers", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Indent(10.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::PushItemWidth(30.0f);

    for (int i = 0; i < _v.size(); i++) {
        if (i && !(i % 4)) {
            ImGui::NewLine();
        }

        char label[6];
        snprintf(label, sizeof(label), "##v%X", i);
        ImGui::Text(&label[2]);
        ImGui::PushStyleColor(ImGuiCol_Text, copy_of_v[i] == _v[i] ? white : red);
        ImGui::SameLine();
        ImGui::InputScalar(label, ImGuiDataType_U8, &_v[i], NULL, NULL, "%02X", flags);
        ImGui::SameLine();
        ImGui::PopStyleColor();
    }

    ImGui::NewLine();

    // delay timer
    ImGui::Text("DT");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, copy_of_delay_timer == _delay_timer ? white : red);
    ImGui::InputScalar("##DT", ImGuiDataType_U8, &_delay_timer, NULL, NULL, "%02X", flags);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // sound timer
    ImGui::Text("ST");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, copy_of_sound_timer == _sound_timer ? white : red);
    ImGui::InputScalar("##ST", ImGuiDataType_U8, &_sound_timer, NULL, NULL, "%02X", flags);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // stack pointer
    ImGui::Text("SP");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, copy_of_sp == _sp ? white : red);
    ImGui::InputScalar("##SP", ImGuiDataType_U8, &_sp, NULL, NULL, "%02X", flags);
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // index register
    ImGui::TextColored(white, " I");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, copy_of_i == _i ? white : red);
    ImGui::InputScalar("##I", ImGuiDataType_U16, &_i, NULL, NULL, "%02X", flags);
    ImGui::PopStyleColor();

    // program counter
    ImGui::TextColored(white, "PC");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, copy_of_pc == _pc ? white : red);
    ImGui::InputScalar("##PC", ImGuiDataType_U16, &_pc, NULL, NULL, "%X", flags);
    ImGui::PopStyleColor();

    ImGui::PopItemWidth();
    ImGui::End();

    // update copies
    copy_of_v = _v;
    copy_of_i = _i;
    copy_of_pc = _pc;
    copy_of_sp = _sp;
    copy_of_stack = _stack;
    copy_of_delay_timer = _delay_timer;
    copy_of_sound_timer = _sound_timer;
}

void gui::memory_dock() {
    static MemoryEditor mem_edit;
    mem_edit.DrawWindow("Memory", &_memory, _memory.size());
}

void gui::keypad_dock() {
    ImGui::Begin("Keypad", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    for (usize idx = 0; const auto& k : {0x1, 0x2, 0x3, 0xC, 0x4, 0x5, 0x6, 0xD, 0x7, 0x8, 0x9,
                                         0xE, 0xA, 0x0, 0xB, 0xF}) {
        ImGui::PushID(k);
        if (ImGui::Selectable(to_string(k).c_str(), _keypad[k] == 1, 0, ImVec2{50, 50})) {
            _keypad[k] = 1;
        } else {
            _keypad[k] = 0;
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

void gui::emulator_dock() {
    ImGui::Begin("Emulator", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    draw_emulator(0);
    ImGui::End();
}

void gui::show_main_menu_bar() {
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
                load_rom(fname);
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

void gui::draw_emulator(const ImGuiWindowFlags& flags) {
    if (!_DEBUG_MODE) // only create a new imgui container if not in debug mode
        ImGui::Begin("Game", nullptr, flags);
    if (_rom_loaded) {
        run();
        _texture.clear();
        for (usize row = 0, idx = 0; idx < DISPLAY_SIZE; idx++) {
            usize col = (idx % CHIP8_WIDTH);
            sf::RectangleShape pixel(sf::Vector2f(SCALE_FACTOR, SCALE_FACTOR));
            pixel.setPosition(col * SCALE_FACTOR, row * SCALE_FACTOR);
            pixel.setFillColor(_video[idx] == 1 ? sf::Color::White : sf::Color::Black);
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

void gui::display() {
    _window.clear();

    show_main_menu_bar();
    if (!_DEBUG_MODE) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoSavedSettings;
        draw_emulator(flags);

    } else {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        code_dock();
        memory_dock();
        keypad_dock();
        emulator_dock();
        registers_dock();
    }

    // ImGui::ShowDemoWindow();
    ImGui::SFML::Render(_window);
    _window.display();
}

void gui::handle_events() {
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
                _keypad[1] = state;
                break;
            case sf::Keyboard::Key::Num2:
                _keypad[2] = state;
                break;
            case sf::Keyboard::Key::Num3:
                _keypad[3] = state;
                break;
            case sf::Keyboard::Key::Num4:
                _keypad[0xC] = state;
                break;
            case sf::Keyboard::Key::Q:
                _keypad[4] = state;
                break;
            case sf::Keyboard::Key::W:
                _keypad[5] = state;
                break;
            case sf::Keyboard::Key::E:
                _keypad[6] = state;
                break;
            case sf::Keyboard::Key::R:
                _keypad[0xD] = state;
                break;
            case sf::Keyboard::Key::A:
                _keypad[7] = state;
                break;
            case sf::Keyboard::Key::S:
                _keypad[8] = state;
                break;
            case sf::Keyboard::Key::D:
                _keypad[9] = state;
                break;
            case sf::Keyboard::Key::F:
                _keypad[0xE] = state;
                break;
            case sf::Keyboard::Key::Z:
                _keypad[0xA] = state;
                break;
            case sf::Keyboard::Key::X:
                _keypad[0] = state;
                break;
            case sf::Keyboard::Key::C:
                _keypad[0xB] = state;
                break;
            case sf::Keyboard::Key::V:
                _keypad[0xF] = state;
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
