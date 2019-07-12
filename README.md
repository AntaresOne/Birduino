# FlappyBirdy for Arduino
# Index
- <a href="#what-it-is">What it is</a>
- <a href="#wiring">Wiring</a>
- <a href="#display-setup">Display setup</a>
- <a href="#debugging">Debugging</a>

## [What it is](#what-it-is)
Yet another Flappy Bird clone, this time playable on an Arduino with a momentary button and an OLED display.

## [Wiring](#wiring)
Very simple, all you need is:
- SSD1306 OLED display
- momentary button
- 4.7kΩ resistor
- obviously, an Arduino :)

![Alt text](wiring.jpg?raw=true "Wiring")

## [Display setup](#display-setup)
This minigame has been developed for 0.96" 128x64 OLED display. Although all the resolution variables and constants have been exposed to be modified on the fly, it is untested on different resolution displays.

In case you want to try it on a different OLED display, here's the data to be modified:
- SSD1306_LCDHEIGHT from Adafruit_SSD1306.h
- PLAYER_SIZE_X // Player bitmap's X size
- PLAYER_SIZE_Y // Player bitmap's Y size
- PIPE_SIZE_X // Pipe obstacle type bitmap's X size
- PIPE_SIZE_Y // Pipe obstacle type bitmap's Y size
- axisOffset // Axis offset to define a safe screen area
- playerPositionY // Initial player position on Y axis (display.height() / 2)
- obstacleInitialPositionX // Initial obstacle position on X axis (display.width() - axisOffset)

## [Debugging](#debugging)
Debugging via the serial port is turned off by default. To enable it, just uncomment "#define DEBUGGABLE" symbol.
