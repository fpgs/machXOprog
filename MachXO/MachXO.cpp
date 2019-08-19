// The MIT License (MIT)
// Copyright (c) 2019 Greg Steiert for Lattice Semiconductor

/* This is a library for programming the Lattice MachXO2/3 device 
   through I2C or SPI.

*/

#include "MachXO.h"
#include "Arduino.h"

// I2C Constructor
MachXO::MachXO(TwoWire *theWire, uint8_t theAddr)
    : _cs(-1), _mosi(-1), _miso(-1), _sck(-1) {
  _wire = theWire;
  _i2caddr = theAddr;
}

// Hardware SPI Constructor
MachXO::MachXO(int8_t cspin, SPIClass *theSPI)
    : _cs(cspin), _mosi(-1), _miso(-1), _sck(-1) {
  _spi = theSPI;
}

// Bitbang SPI Constructor
MachXO::MachXO(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin)
    : _cs(cspin), _mosi(mosipin), _miso(misopin), _sck(sckpin) {
}

uint32_t MachXO::cmdxfer(uint8_t *wbuf, int wcnt, uint8_t *rbuf, int rcnt) {
    if (_cs == -1)
    {
        Wire.beginTransmission(_i2caddr);
        for (int i = 0; i < wcnt; i++)
        {
            Wire.write(wbuf[i]);
        }
        if (rcnt)
        {
            Wire.endTransmission(false);
            Wire.requestFrom(_i2caddr, rcnt);
            if (Wire.available() >= rcnt)
            {
                for (int i = 0; i < rcnt; i++)
                {
                    rbuf[i] = Wire.read();
                }
            }
        }
        else
        {
            Wire.endTransmission(true);
        }
    }
    return 0;
}

uint32_t MachXO::readDeviceID(uint8_t *ibuf) {
  uint8_t obuf[4] = {0xE0, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, ibuf, 4);
}

uint32_t MachXO::readUserCode(uint8_t *ibuf) {
  uint8_t obuf[4] = {0xC0, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, ibuf, 4);
}

uint32_t MachXO::readStatus(uint8_t *ibuf) {
  uint8_t obuf[4] = {0x3C, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, ibuf, 4);
}

uint32_t MachXO::readFeatureBits(uint8_t *ibuf) {
  uint8_t obuf[4] = {0xFB, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, ibuf, 2);
}

uint32_t MachXO::readFeatureRow(uint8_t *ibuf) {
  uint8_t obuf[4] = {0xE7, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, ibuf, 8);
}

uint32_t MachXO::readOTPFuses(uint8_t *ibuf) {
  uint8_t obuf[4] = {0xFA, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, ibuf, 1);
}

uint32_t MachXO::readFlash(uint8_t *ibuf) {
  uint8_t obuf[4] = {0x73, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, ibuf, 16);
}

uint32_t MachXO::readUFM(uint8_t *ibuf) {
  uint8_t obuf[4] = {0xCA, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, ibuf, 16);
}

uint32_t MachXO::eraseUFM() {
  uint8_t obuf[4] = {0xCB, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, NULL, 0);
}

uint32_t MachXO::erase(uint32_t flags) {
  uint8_t obuf[4] = {0x0E, (0x0F & (flags>>16)), 0x00, 0x00};
  return cmdxfer(obuf, 4, NULL, 0);
}

uint32_t MachXO::enableConfigTransparent() {
  uint8_t obuf[3] = {0x74, 0x08, 0x00};
  return cmdxfer(obuf, 3, NULL, 0);
}

uint32_t MachXO::enableConfigOffline() {
  uint8_t obuf[3] = {0xC6, 0x08, 0x00};
  return cmdxfer(obuf, 3, NULL, 0);
}

uint32_t MachXO::isBusy() {
  uint8_t ibuf[1];
  uint8_t obuf[4] = {0xF0, 0x00, 0x00, 0x00};
  cmdxfer(obuf, 4, ibuf, 1);
  return ((ibuf[0] & 0x80) ? 1 : 0); 
}

uint32_t MachXO::waitBusy() {
  uint32_t waitCnt = 0;
  while (isBusy()) {
    waitCnt += 1;
  }
  return (waitCnt);
}

uint32_t MachXO::resetConfigAddress() {
  uint8_t obuf[4] = {0x46, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, NULL, 0);
}

uint32_t MachXO::resetUFMAddress() {
  uint8_t obuf[4] = {0x47, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, NULL, 0);
}

uint32_t MachXO::setConfigAddress(uint32_t page) {
    uint8_t obuf[8];
    obuf[0] = 0xB4;
    obuf[1] = 0x00;
    obuf[2] = 0x00;
    obuf[3] = 0x00;
    obuf[4] = 0x00;
    obuf[5] = 0x00;
    obuf[6] = (page >> 8) & 0xFF;
    obuf[7] = (page)&0xFF;
    return cmdxfer(obuf, 8, NULL, 0);
}

uint32_t MachXO::setUFMAddress(uint32_t page) {
    uint8_t obuf[8];
    obuf[0] = 0xB4;
    obuf[1] = 0x00;
    obuf[2] = 0x00;
    obuf[3] = 0x00;
    obuf[4] = 0x40;
    obuf[5] = 0x00;
    obuf[6] = (page >> 8) & 0xFF;
    obuf[7] = (page) & 0xFF;
    return cmdxfer(obuf, 8, NULL, 0);
}

uint32_t MachXO::programPage(uint8_t *obuf) {
  return cmdxfer(obuf, 20, NULL, 0);
}

uint32_t MachXO::programDone() {
  uint8_t obuf[4] = {0x5E, 0x00, 0x00, 0x00};
  return cmdxfer(obuf, 4, NULL, 0);
}

uint32_t MachXO::refresh() {
  uint8_t obuf[3] = {0x79, 0x00, 0x00};
  return cmdxfer(obuf, 3, NULL, 0); 
}

uint32_t MachXO::wakeup() {
  uint8_t obuf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
  return cmdxfer(obuf, 4, NULL, 0);
}

uint32_t MachXO::loadHex(FatFile hexFile) {
  int byteCnt = 0;
//  FatFile hexFile = fatfs->open(fileName);
    int pageCnt = 0;
    char nextChr;
    char hexByteStr[3] = {0, 0, 0};
    uint8_t pageBuf[20];
    pageBuf[0] = 0x70;
    pageBuf[1] = 0x00;
    pageBuf[2] = 0x00;
    pageBuf[3] = 0x01;
    resetConfigAddress();
    while (hexFile.available()) {
      nextChr = hexFile.read();
      if (!isHexadecimalDigit(nextChr)) {
        byteCnt = 0;
      } else {
        if (byteCnt > 15) {
          Serial.println("too many hex digits");
        } else {
          if (hexFile.available()) {
            hexByteStr[0] = nextChr;
            hexByteStr[1] = hexFile.read();
            pageBuf[byteCnt+4] = strtoul(hexByteStr, NULL, 16);
            byteCnt += 1;
            if (byteCnt == 16) {
              programPage(pageBuf);
              pageCnt += 1;
            }
          } else {
            Serial.println("uneven number of hex digits");
          }
        }
      }
    }
    Serial.print(pageCnt);
    Serial.println(" pages written");
    if (byteCnt == 16) {
      byteCnt = 0;
    }
    Serial.print(byteCnt);
    Serial.println(" bytes left over");
    hexFile.close();
  return (byteCnt);
}
