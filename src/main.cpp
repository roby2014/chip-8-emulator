#include <cstring>
#include <iostream>

#include "chip8.hpp"
#include "imgui-SFML.h"
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
    ImGui::SFML::Init(window);

    sf::Clock clk;
    while (window.isOpen()) {
        // TODO: handle keyboard input
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // fetch, decode, execute..
        emu.run();
        ImGui::SFML::Update(window, clk.restart());

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        // draw
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

        // display
        ImGui::SFML::Render(window);
        window.display();
    }

    window.close();
    ImGui::SFML::Shutdown();
    std::cin; // windows things x]
    return 0;
}
