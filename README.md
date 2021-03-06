# machXOprog

Program Lattice MachXO2/3 with CircuitPython

## Overview

This is a CircuitPython library that allows updating the internal flash in a MachXO2/3 device through I2C with a CircuitPython board.

## Dependencies

This library depends on:

* [Adafruit CircuitPython](https://github.com/adafruit/circuitpython)
* [Adafruit_CircuitPython_BusDevice](https://github.com/adafruit/Adafruit_CircuitPython_BusDevice|)
  
It is highly recommended to use an ARM Cortex M4 board with sufficient memory to run CircuitPython.  External SPI memory is likely required to store the MachXO image.  This code was developed with the [Adafruit Feather M4 Express](https://www.adafruit.com/product/3857)

## Examples

* load_jed.py is an example for programming a MachXO2/3 from a .jed file
* load_hex.py is an example for programming a MachXO2/3 from a .hex file

### Example Files

Some example hex and jedec files are provided in the examples folder for demonstration purposes.

* tiny256.hex/tiny256.jec are examples for the [TinyFPGA AX1](https://store.tinyfpga.com/products/tinyfpga-a1) that blinks the LED
* xo3starter.hex/xo3starter.jed are examples for the [MachXO3LF Starter Kit](http://www.latticesemi.com/en/Products/DevelopmentBoardsAndKits/MachXO3LFStarterKit) that applies a counter to the LEDs controlled by the switches

## Status

This can currently program the configuration flash through I2C and has a variety of helper functions for erasing and status.
To be done:

* Add support for EBR initialization
* Add support for UFM programming
* Add more documentation about feature row (before allowing a program function that could brick boards)
* Add equivalent library for SPI programming

## Credits

This was based loosely on [Adafruit_CircuitPython_AVRprog](https://github.com/adafruit/Adafruit_CircuitPython_AVRprog), but the code was all written from scratch
