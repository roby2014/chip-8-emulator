/*#include <cstring>
#include <iostream>

#include "chip8.hpp"
#include "imgui.h"
#include <SFML/Graphics.hpp>

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 320
#define SCALE_FACTOR  10 // 64*10 = 640 ; 32*10 = 340 ..

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./%s <rom_file> \n", argv[0]);
        return 1;
    }

    chip8 emu;

#ifndef DEBUG
    emu.load_rom(argv[1]);
#else
    emu.load_rom(std::initializer_list<u8>({0xF1, 0x0A, 0x44, 0x44}));
#endif

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "CHIP-8 Emulator");

    sf::Clock clk;
    while (window.isOpen()) {
        // TODO: handle keyboard input
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // fetch, decode, execute..
        emu.run();

        // draw
        auto row = 0;
        for (usize idx = 0; idx < DISPLAY_SIZE; idx++) {
            auto col = (idx % CHIP8_WIDTH);

            sf::RectangleShape pixel(sf::Vector2f(SCALE_FACTOR, SCALE_FACTOR));
            pixel.setPosition(col * SCALE_FACTOR, row * SCALE_FACTOR);
            pixel.setFillColor(emu.get_pixel(idx) == 1 ? sf::Color::White :
sf::Color::Black); window.draw(pixel);

            if ((idx + 1) % CHIP8_WIDTH == 0) {
                row++;
            }
        }

        // display
        window.display();
    }

    window.close();
    std::cin; // windows things x]
    return 0;
}*/
#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

void code_dock() {
    ImGui::Begin("Code");

    // Note: we are using a fixed-sized buffer for simplicity here. See ImGuiInputTextFlags_CallbackResize
        // and the code in misc/cpp/imgui_stdlib.h for how to setup InputText() for dynamically resizing strings.
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
    ImGui::TreePop();

    ImGui::End();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 600), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        
        ImGui::ShowDemoWindow();

        code_dock();

        ImGui::Begin("Registers");
        ImGui::End();

        ImGui::Begin("Memory");
        ImGui::End();

        window.clear();
        window.draw(shape);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}