#include <iostream>
#include "imgui.h"
#include "imgui-SFML.h"
#include "chip8.hpp"

#include <SFML/Graphics.hpp>

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 320
#define SCALE_FACTOR  10 // 64*10 = 640 ; 32*10 = 340 ..

void code_dock() {
    ImGui::Begin("Code");

    static char text[1024 * 16] =
        "/*\n"
        " int main() {\n"
        "   return 0;\n"
        " }\n"
        "label:\n"
        "\tlock cmpxchg8b eax\n";

    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", &flags, ImGuiInputTextFlags_ReadOnly);
    ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", &flags, ImGuiInputTextFlags_AllowTabInput);
    ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);

    ImGui::End();
}

bool DEBUG_MODE = false;

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./%s <-d 0/1 (debugger)> \n", argv[0]);
        return 1;
    }

    chip8 emu;

    DEBUG_MODE = argv[1][0] == '0' ? false : true;
    std::cout << "DEBUG MODE IS SET TO " << DEBUG_MODE;

    sf::RenderWindow window(sf::VideoMode(1200, 600), "CHIP-8 Emulator");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (!DEBUG_MODE) {
            ImGui::SFML::Update(window, deltaClock.restart());

            ImGui::ShowDemoWindow();

            static bool rom_loaded = false;
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Load ROM", "Ctrl+O")) {
                        // if LINUX
                        char fname[1024];
                        FILE* fp = popen("zenity --file-selection", "r");
                        fgets(fname, sizeof(fname), fp);
                        if (fname[strlen(fname) - 1] == '\n') {
                            fname[strlen(fname) - 1] = 0;
                        }
                        // TODO: if WINDOWS
                        // TODO: if macOS

                        // check if rom file is already loaded
                        // load it again
                        emu.load_rom(fname);
                        rom_loaded = true;
                    }
                    ImGui::EndMenu();
                } else if (ImGui::MenuItem("Exit")) {
                    window.close();
                }
                ImGui::EndMainMenuBar();
            }

            if (rom_loaded) {
                emu.run();
                auto row = 0;
                for (usize idx = 0; idx < DISPLAY_SIZE; idx++) {
                    auto col = (idx % CHIP8_WIDTH);

                    sf::RectangleShape pixel(sf::Vector2f(SCALE_FACTOR, SCALE_FACTOR));
                    pixel.setPosition(col * SCALE_FACTOR, row * SCALE_FACTOR);
                    pixel.setFillColor(emu.get_pixel(idx) == 1 ? sf::Color::White : sf::Color::Black);
                    window.draw(pixel);

                    if ((idx + 1) % CHIP8_WIDTH == 0) {
                        row++;
                    }
                }
            }

        } else {
            auto& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            ImGui::SFML::Update(window, deltaClock.restart());

            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

            code_dock();

            ImGui::Begin("Registers");
            ImGui::End();

            ImGui::Begin("Memory");
            ImGui::End();
        }


        // window.clear();
        //  draw stuff
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}