# A SD card Media Browser
- FreeRTOS (3 tasks for buttons, SD card and display)
- STM32 MCU (L4RG-Nucleo and WeAct-F411-BlackPill)

This media display is my first attempt to apply and learn about RTOS and get use to programing with something other than Arduino/ESP32 and simple "super loops". It uses a SD TFT display and SD card reader which are connected to a SPI bus to the MCU. The two tasks aquire the mutex to write or read from the bus. I also have a set of buttons to type (Nokia style is WIP) and navigate the different files on the sd card. 

## Build Instructions
- I will produce proper build instructions when I get this to work on VSCode. Using the stm32cubeIDE was nice to explore and learn but it seems the VSCode verions is just as capable (maybe prefered method) and I am more accustomed.
- If the IDE and CubeMx are installed it should be easy to run the project after cloning. Import a stm32 project and select the two boards when importing.
- After importing you might get some simple warnings you can fix than by click on the project and running build it should build. (slight issue with setting up paths as I wanted multiple targets. I hope to get that resolved so it is as simple as build -> import -> select -> build -> flash)

## Pictures
### Nucleo testing phase
Need to add
### Porting to F411 board
WIP
### PCB and 3D model
WIP
### Final Project
WIP
## Future Works
- More buttons right now there are 2
- refactor first steps code into something more resuable
- DMA for SPI bus to make it faster and more responsive. CPU can than handle button polling instead of writing SPI bytes
- make into final project with battery, custom PCB and plastic case.
