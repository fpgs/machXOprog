// The MIT License (MIT)
// Copyright (c) 2019 Greg Steiert for Lattice Semiconductor

/* This is a library for programming the Lattice MachXO2/3 device 
   through I2C or SPI.

*/

#ifndef __MachXO_H__
#define __MachXO_H__

#include "Arduino.h"
#include "SdFat.h"
#include <Wire.h>
#include <SPI.h>

#define MACHXO_I2C_ADDR       0x40

//  Forward declarations of Wire and SPI for board/variant combinations that don't have a default 'Wire' or 'SPI' 
extern TwoWire Wire;  /**< Forward declaration of Wire object */
extern SPIClass SPI;  /**< Forward declaration of SPI object */

// Erase Flag Enumeration
enum {
    MACHXO_ERASE_SRAM = (1 << 16),
    MACHXO_ERASE_FEATURE_ROW = (1 << 17),
    MACHXO_ERASE_CONFIG_FLASH = (1 << 18),
    MACHXO_ERASE_UFM = (1 << 19),
};

// Class Definition for MachXO Programming library
class MachXO {
public:
    MachXO(TwoWire *theWire = &Wire, uint8_t theAddr = MACHXO_I2C_ADDR);
    MachXO(int8_t cspin, SPIClass *theSPI = &SPI);
    MachXO(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin);
    uint32_t readDeviceID(uint8_t *ibuf);
    uint32_t readUserCode(uint8_t *ibuf);
    uint32_t readStatus(uint8_t *ibuf);
    uint32_t readFeatureBits(uint8_t *ibuf);
    uint32_t readFeatureRow(uint8_t *ibuf);
    uint32_t readOTPFuses(uint8_t *ibuf);
    uint32_t readFlash(uint8_t *ibuf);
    uint32_t readUFM(uint8_t *ibuf);
    uint32_t eraseUFM();
    uint32_t erase(uint32_t flags);
    uint32_t enableConfigTransparent();
    uint32_t enableConfigOffline();
    uint32_t isBusy();
    uint32_t waitBusy();
    uint32_t resetConfigAddress();
    uint32_t resetUFMAddress();
    uint32_t setConfigAddress(uint32_t page);
    uint32_t setUFMAddress(uint32_t page);
    uint32_t programPage(uint8_t *obuf);
    uint32_t programDone();
    uint32_t refresh();
    uint32_t wakeup();
    uint32_t loadHex(FatFile hexFile);
//    uint32_t loadHex(FatFileSystem *fatfs, char *fileName);

private:
    TwoWire *_wire; /**< Wire object */
    SPIClass *_spi; /**< SPI object */
    uint8_t _i2caddr;
    uint32_t cmdxfer(uint8_t *wbuf, int wcnt, uint8_t *rbuf, int rcnt);
    int8_t _cs, _mosi, _miso, _sck;
};

#endif // __MachXO_H__