# chip-8-emulator

CHIP-8 emulator and debugger written in C++.
> Chip-8 is a simple, interpreted, programming language which was first used on some do-it-yourself computer systems in the late 1970s and early 1980s.

You can find more about CHIP-8 here:
* [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
* [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#00E0)

## Yes, I know!
I know that technically this is more like a interpreter & virtual machine implementation than a real "emulator". But.... emulator sounds cool.

## TODO
The project is not fully implemented, here are some stuff I still need to add:
* Keyboard input handling
* Add timers & sounds
* GUI debugger & visualizer
* Windows + Linux bash scripts to compile

## (Planned) Features
* CHIP-8 virtual machine
* CHIP-8 interpreter
* CHIP-8 GUI debugger

## Screenshots

## Building and running
### Linux

- Install the packages:
    - Arch based distros: `sudo pacman -S git make cmake sfml`
    - Other distros: `Idk, google it`

If you have them correctly installed, it should work out of the box, so run this in your terminal:

`git clone https://github.com/roby2014/chip-8-emulator && cd chip-8-emulator`

`mkdir build && cd build && cmake .. && make`

- Running: `./chip8 <ROM_PATH>` (e.g: `./chip8 ../roms/chip8.ch8`)

### Windows (Visual Studio)
Download git, [CMake latest version](https://cmake.org/download/), [SFML 2.5.1 (Visual C++ 15 (2017) - 32-bit)](https://www.sfml-dev.org/download/sfml/2.5.1/) and Visual Studio 2022.

*make sure you add cmake to PATH*

- Clone the repo and go inside: 

    `git clone https://github.com/roby2014/chip-8-emulator && cd chip-8-emulator`
- Generate VS project: 

    `cmake -A Win32 -B build -DSFML_DIR="C:\CPP_TOOLS\SFML\lib\cmake\SFML"` 
    *(make sure you specify your SFML path)*

- Compile the project:
    - Open the VS solution and build it
    - or just run: `cmake --build build --config Release`

- Running:
    - Since we use dynamic linking, you will need to move all the `.dll` files from `C:\CPP_TOOLS\SFML\bin\` into your .exe folder
    - Now either drag a rom file into the exe or run by command line: 

        `cd C:\Coding\chip-8-emulator\build\Release\`

        `chip8.exe C:\Coding\chip-8-emulator\roms\chip8.ch8`



## More ROMS
I have included some ROMs by default inside `roms/` folder, but in case you wanna try more/other games, you can download from [this repository](https://github.com/kripod/chip8-roms).
## Keybinds

## repository under dev...
