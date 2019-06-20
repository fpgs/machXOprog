# machXOprog
program Lattice MachXO2/3 from CircuitPython or Arduino

## CircuitPython
### Overview
This is a CircuitPython library that allows updating the internal flash in a MachXO2/3 device through I2C with a CircuitPython board.

### Dependencies
This library depends on:
* [Adafruit CircuitPython](https://github.com/adafruit/circuitpython)
* [Adafruit_CircuitPython_BusDevice](https://github.com/adafruit/Adafruit_CircuitPython_BusDevice|)

### Examples
* load_jed.py is an example for programming a MachXO2/3 from a .jed file
* load_hex.py is an example for programming a MachXO2/3 from a .hex file

### Status
This can currently program the configuration flash through I2C and has a variety of helper functions for erasing and status.
To be done:
* Add support for EBR initialization
* Add support for UFM programming
* Add more documentation about feature row (before allowing a program function that could brick boards)
* Add equivalent library for SPI programming

### Credits 
This was based loosely on [Adafruit_CircuitPython_AVRprog](https://github.com/adafruit/Adafruit_CircuitPython_AVRprog), but the code was all written from scratch

## Arduino 
machxo_spi.ino

### Overview
This is an Arduino Sketch that allows updating the internal flash in a MachXO2/3 device through SPI with an Arduino board.

### Dependencies
This example sketch relies on the SD library to store the image to be loaded onto the device.

