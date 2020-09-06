# ActivePlant
Automatic plant caring using ble

This repository contains the source code for the project
created during the summer of 2020 as part of the Protopaja
(Prototype Workshop) course at Aalto University, Finland.

## The project

As part of the course our team developed a device to control
the humidity level in a plant's soil, as well a user interface
for controlling the settings of the device.

## Build instructions

In order to build the code for the microcont you need:
* the avr-gcc toolchain
* make
* avrdude (used for flashing the chip)

These are quite easily installed on linux through distribution
packages. For windows you can get make from the mingw32 project,
avr-gcc can be downloaded from https://sourceforge.net/projects/winavr/
and avrdude fromhttps://sourceforge.net/projects/avrdudegui/

To build the code you need to be in the top level directory
and run the command
```
make
```
This create a directory called ./build and place all object files
there.
In order to flash the chip you can give:
```
make flash
make flash_eeprom
```

Some other useful targets are:
* ```make size``` will print the memory used by the code (for text and data sections)
* ```make terminal``` to use avrdude to interactively inspect the chip
* ```make clean``` to clean the build files

A map file is also generated under build should you have any use for it.

## GUI
The GUI is build using pyqt5.
If you have python3 installed you can install pyqt3 through
pip
```
pip3 install PyQt5
```

## Team
* Dima Al-Nasser
* Hyeju  Lee
* Konstantinos Karatzidis

## Acknoledgements
* [Helvar](https://helvar.com/) for sponsoring the project and providing technical support
* Elliot Williams (Makefile in this project is a modified version of his from https://github.com/hexagon5un/AVR-Programming)
