# avrdude-gpio-autoreset

## Introduction
This is a little command line tool, that helps you programming an Arduino chip attached directly to the RPi GPIO pins.
Only the RPi hardware serial port /dev/ttyAMA0 (2 pins) and one GPIO pin for resetting the Arduino chip are used. So, no USB port is taken.
This tool was inspired by the work of Dean Mao. Thank you.
There are some advantages though:
- Standalone (Does not require python or any other dependencies)
- Fast startup (completely written in C/C++)
- Has a working timeout built in, so the program does not get locked under certain conditions
- Reset pin gets released again after execution

Instead of using SPI programming this solution talks to the Arduino Bootloader over the serial interface. So you can program your Arduino over serial and then use the serial port for communication between Raspberry Pi and Arduino.
 
## History
- 2016-01-25 Initial release
- 2018-01-17 Enhanced documentation for Raspbian Stretch including RPi 3 and RPi Zero W. GPIOClass reimplementaion. Stretch compatibility. 

## Setup overview
Download the respository as ZIP file and extract its content. In short you have to do the following steps
- Disable serial output on the Raspberry Pi
- Install avrdude including all dependencies
- Make a backup copy of avrdude
- Create a wrapper script (avrdude)
- Move avrdude-gpio-autoreset binary to /usr/bin
If you are interested in how the whole process works you can go to Dean Mao's page http://www.deanmao.com/2012/08/12/fixing-the-dtr-pin/

## Requirements
- Raspberry Pi A/A+, B/B+, 2 or Zero (W)
- Arduino board or clone
- 3 jumper wires
- 1 1uF capacitor (or 100nF if connected directly to the microcontroller)
- 2 Resistors (10k and 15k) for level shifting 5V -> 3.3V (Keep in mind that RPi pins are not 5V tolerant!)

## Setup instructions
Disable all internal serial output on the Raspberry Pi by typing
```
# sudo raspi-config
```
Go to "Advanced Options" -> "Serial" -> "No"
If you are using newer Raspbian distros, ensure to disable "login shell" and enable "serial port hardware".

Install avrdude and make a backup copy
```
# sudo apt-get install avrdude
# sudo mv /usr/bin/avrdude /usr/bin/avrdude-original
```
Create a wrapper script and make it executable
```
# sudo nano /usr/bin/avrdude
```
paste this code in
```
#!/bin/sh
strace -o "|avrdude-gpio-autoreset" -eioctl /usr/bin/avrdude-original $@
```
save it and make it executable
```
# sudo chmod +x /usr/bin/avrdude
```
Download [avrdude-gpio-autoreset](https://github.com/TOLDOTECHNIK/avrdude-gpio-autoreset/blob/master/bin/avrdude-gpio-autoreset) and move binary to the /usr/bin folder. You can download it manually or make a clone of this repository (Ensure you have git installed).
```
# git clone https://github.com/TOLDOTECHNIK/avrdude-gpio-autoreset.git
# sudo mv avrdude-gpio-autoreset/bin/avrdude-gpio-autoreset /usr/bin/
```

## Wire up
![RPi Arduino Wire Up](/diagrams/RPi%20Arduino%20Wire%20Up.png)

## Upload sketches to Arduino
- After compiling your own sketch in the Arduino IDE you have to find the generated HEX file and copy it to the Raspberry Pi. On Windows it is located under the temp folder (Press Windows-R, type in %temp% and press Enter). There you will find a build folder containing the HEX file).
- On the Raspberry Pi open up a terminal window and type:
```
# sudo avrdude -D -V -F -c arduino -p m328p -P /dev/ttyAMA0 -U flash:w:PATH_TO_YOUR_ARDUINO_FIRMWARE.hex:i
```

**Please note:** On Raspberry Pi 3 or Raspberry Pi Zero W the ttyAMA0 is used for Bluetooth communication by default. If so, use ttyS0 for uploading instead.

## Custom compilation
You can compile your own version of avrdude-gpio-autoreset(required when you use another reset pin than GPIO4). You will find the RESET_PIN definition in the avrdude-gpio-autoreset.cpp file.

```
# cd avrdude-gpio-autoreset
# make
# sudo mv avrdude-gpio-autoreset/bin/avrdude-gpio-autoreset /usr/bin/
``` 