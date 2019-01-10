# machXOprog
program Lattice MachXO2/3 from CircuitPython

## Overview
This is a CircuitPython library that allows updating the internal flash in a MachXO2/3 device.

## Dependencies
This library depends on:
* [Adafruit CircuitPython](https://github.com/adafruit/circuitpython)
* [Adafruit_CircuitPython_BusDevice](https://github.com/adafruit/Adafruit_CircuitPython_BusDevice|)

## Examples
An example is provided for programming a MachXO2000 from a .jed file

## Status
This can currently program the configuration flash through I2C and has a variety of helper functions for erasing and status.
To be done:
* Add support for EBR initialization
* Add support for UFM programming
* Add more documentation about feature row (before allowing a program function that could brick boards)
* Add equivalent library for SPI programming

## Credits 
This was based loosely on [Adafruit_CircuitPython_AVRprog](https://github.com/adafruit/Adafruit_CircuitPython_AVRprog), but the code was all written from scratch