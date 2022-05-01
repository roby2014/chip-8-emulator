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

## (Planned) Features
* CHIP-8 virtual machine
* CHIP-8 interpreter
* CHIP-8 GUI debugger

## Screenshots

## Building
To build this project, you will need SFML libraries. If you have them correctly installed, just run:

`make`

*(TODO: proper explanation on building on different OSes, lib linking, etc...)*

## Running
I have included some ROMs by default inside `roms/` folder, but in case you wanna try more/other games, you can download from [this repository](https://github.com/kripod/chip8-roms).

After building, you can just run:

`bin/chip8 <ROM_PATH>`

e.g: `bin/chip8 roms/PONG`

## Keybinds

## repository under dev...
