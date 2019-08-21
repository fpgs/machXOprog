# machXOprog
Program Lattice MachXO2/3 with CircuitPython or Arduino

## CircuitPython
### Overview
This is a CircuitPython library that allows updating the internal flash in a MachXO2/3 device through I2C with a CircuitPython board.

### Dependencies
This library depends on:
* [Adafruit CircuitPython](https://github.com/adafruit/circuitpython)
* [Adafruit_CircuitPython_BusDevice](https://github.com/adafruit/Adafruit_CircuitPython_BusDevice|)
  
It is highly recommended to use an ARM Cortex M4 board with sufficient memory to run CircuitPython.  External SPI memory is likely required to store the MachXO image.  This code was developed with the [Adafruit Feather M4 Express](https://www.adafruit.com/product/3857)

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
An Arduino library for the MachXO2/3 is provided in the MachXO directory.  More details can be found in [MachXO/README.md](./MachXO/README.md) 

## Arduino SPI 
### machxo_spi.ino will be deprecated when SPI support is implemented in the MachXO library
machxo_spi/machxo_spi.ino

### Overview
This is an Arduino Sketch that allows updating the internal flash in a MachXO2/3 device through SPI with an Arduino board.

### Dependencies
This example sketch relies on the SD library to store the image to be loaded onto the device.  This code was developed with the [Adafruit Feather M0 Adalogger](https://www.adafruit.com/product/2796)

## Arduino I2C 
### machxo_i2c.ino is deprecated.  Use the MachXO library instead
machxo_i2c/machxo_i2c.ino

### Overview
This is an Arduino Sketch that allows updating the internal flash in a MachXO2/3 device through I2C with an Arduino board.

### Dependencies
This example sketch relies on an external SPI flash fat filesystem to store the image to be loaded onto the device.  The following libraries are required:
* [Adafruit_SPIFlash](https://github.com/adafruit/Adafruit_SPIFlash)
* [Adafruit_QSPI](https://github.com/adafruit/Adafruit_QSPI)
This code was developed with the [Adafruit Feather M4 Express](https://www.adafruit.com/product/3857)

### Loading files into external SPI flash
The MachXO Library includes examples that utilize the TinyUSB library so that files can be copied to the board just like a USB thumb drive.  
