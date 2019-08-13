// The MIT License (MIT)
// Copyright (c) 2019 Greg Steiert for Lattice Semiconductor

/* This example demo how to program a Lattice MachXO2/3 device through I2C
   from a file located in the fat formatted on-board external Flash.
   - For M0 express series with SPI flash device
     follow library is required
     https://github.com/adafruit/Adafruit_SPIFlash

   - For M4 expres series and nRF52840 with QSPI flash, additional library is reuiqred
     https://github.com/adafruit/Adafruit_QSPI
*/

#include <Wire.h>
#include "Adafruit_SPIFlash.h"
#include "Adafruit_SPIFlash_FatFs.h"

#if defined(__SAMD51__) || defined(NRF52840_XXAA)
#include "Adafruit_QSPI.h"
#include "Adafruit_QSPI_Flash.h"

Adafruit_QSPI_Flash flash;
#else
// Configuration of the flash chip pins and flash fatfs object.
// You don't normally need to change these if using a Feather/Metro
// M0 express board.

// Flash chip type. If you change this be sure to change the fatfs to match as well
#define FLASH_TYPE     SPIFLASHTYPE_W25Q16BV

#if (SPI_INTERFACES_COUNT == 1)
#define FLASH_SS       SS                    // Flash chip SS pin.
#define FLASH_SPI_PORT SPI                   // What SPI port is Flash on?
#else
#define FLASH_SS       SS1                    // Flash chip SS pin.
#define FLASH_SPI_PORT SPI1                   // What SPI port is Flash on?
#endif

Adafruit_SPIFlash flash(FLASH_SS, &FLASH_SPI_PORT);     // Use hardware SPI
#endif

//Adafruit_USBD_MSC usb_msc;
Adafruit_W25Q16BV_FatFs fatfs(flash);

#define XO_I2C_ADDR       0x40
#define HEX_FILE_NAME     "machxo3.hex"

const int XO_ERASE_SRAM = 1 << 16;
const int XO_ERASE_FEATURE_ROW = 1 << 17;
const int XO_ERASE_CONFIG_FLASH = 1 << 18;
const int XO_ERASE_UFM = 1 << 19;

uint32_t machXOcmd(uint32_t cmd, uint8_t *wbuf, int wcnt, uint8_t *rbuf, int rcnt) {
  Wire.beginTransmission(XO_I2C_ADDR);
  Wire.write((cmd >> 24) & 0xFF);
  Wire.write((cmd >> 16) & 0xFF);
  Wire.write((cmd >> 8) & 0xFF);
  Wire.write((cmd) & 0xFF);
  for (int i = 0; i < wcnt; i++) {
    Wire.write(wbuf[i]);
  }
  if (rcnt) {
    Wire.endTransmission(false);
    Wire.requestFrom(XO_I2C_ADDR, rcnt);
    if (Wire.available() >= rcnt) {
      for (int i = 0; i < rcnt; i++) {
        rbuf[i] = Wire.read();
      }
    }
  } else {
    Wire.endTransmission(true);
  }
  return 0;
}

uint32_t xoReadDeviceID(uint8_t *buf) {
  return machXOcmd(0xE0000000, NULL, 0, buf, 4);
}

uint32_t xoReadUserCode(uint8_t *buf) {
  return machXOcmd(0xC0000000, NULL, 0, buf, 4);
}

uint32_t xoReadStatus(uint8_t *buf) {
  return machXOcmd(0x3C000000, NULL, 0, buf, 4);
}

uint32_t xoReadFeatureBits(uint8_t *buf) {
  return machXOcmd(0xFB000000, NULL, 0, buf, 2);
}

uint32_t xoReadFeatureRow(uint8_t *buf) {
  return machXOcmd(0xE7000000, NULL, 0, buf, 8);
}

uint32_t xoReadOTPFuses(uint8_t *buf) {
  return machXOcmd(0xFA000000, NULL, 0, buf, 1);
}

uint32_t xoReadFlash(uint8_t *buf) {
  return machXOcmd(0x73000001, NULL, 0, buf, 16);
}

uint32_t xoReadUFM(uint8_t *buf) {
  return machXOcmd(0xCA000001, NULL, 0, buf, 16);
}

uint32_t xoEraseUFM() {
  return machXOcmd(0xCB000000, NULL, 0, NULL, 0);
}

uint32_t xoErase(uint32_t flags) {
  return machXOcmd((0x0E000000 | (flags & 0x0F0000)), NULL, 0, NULL, 0);
}

uint32_t xoEnableConfigTransparent() {
  Wire.beginTransmission(XO_I2C_ADDR);
  Wire.write(0x74);
  Wire.write(0x08);
  Wire.write(0x00);
  Wire.endTransmission(true);
  return 0;
}

uint32_t xoEnableConfigOffline() {
  Wire.beginTransmission(XO_I2C_ADDR);
  Wire.write(0xC6);
  Wire.write(0x08);
  Wire.write(0x00);
  Wire.endTransmission(true);
  return 0;
}

uint32_t xoIsBusy() {
  uint8_t buf[2];
  machXOcmd(0xF0000000, NULL, 0, buf, 1);
  return ((buf[0] & 0x80) ? 1 : 0); 
}

uint32_t xoWaitBusy() {
  uint32_t waitCnt = 0;
  while (xoIsBusy()) {
    waitCnt += 1;
  }
  return (waitCnt);
}

uint32_t xoResetConfigAddress() {
  return machXOcmd(0x46000000, NULL, 0, NULL, 0);
}

uint32_t xoResetUFMAddress(uint32_t flags) {
  return machXOcmd(0x47000000, NULL, 0, NULL, 0);
}

uint32_t xoSetConfigAddress(uint32_t page) {
  uint8_t buf[4];
  buf[0] = 0x00;
  buf[1] = 0x00;
  buf[2] = (page>>8)&0xFF;
  buf[4] = (page)&0xFF;
  return machXOcmd(0xB4000000, buf, 4, NULL, 0);
}

uint32_t xoSetUFMAddress(uint32_t page) {
  uint8_t buf[4];
  buf[0] = 0x40;
  buf[1] = 0x00;
  buf[2] = (page>>8)&0xFF;
  buf[4] = (page)&0xFF;
  return machXOcmd(0xB4000000, buf, 4, NULL, 0);
}

uint32_t xoProgramPage(uint8_t *buf) {
  return machXOcmd(0x70000001, buf, 16, NULL, 0);
}

uint32_t xoProgramDone() {
  return machXOcmd(0x5E000000, NULL, 0, NULL, 0);
}

uint32_t xoRefresh() {
  Wire.beginTransmission(XO_I2C_ADDR);
  Wire.write(0x79);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission(true);
  return 0;
}

uint32_t xoWakeup() {
  return machXOcmd(0xFFFFFFFF, NULL, 0, NULL, 0);
}

uint32_t xoLoadHex(char *fileName) {
  int byteCnt = 0;
  Adafruit_SPIFlash_FAT::File hexFile = fatfs.open(fileName);
  if (hexFile) {
    if (hexFile.available()) {
      Serial.print(fileName);
      Serial.println(" open");
    } else {
      Serial.print("no data in: ");
      Serial.println(fileName);
    }
    int pageCnt = 0;
    char nextChr;
    char hexByteStr[3] = {0, 0, 0};
    uint8_t pageBuf[16];
    xoResetConfigAddress();
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
            pageBuf[byteCnt] = strtoul(hexByteStr, NULL, 16);
            byteCnt += 1;
            if (byteCnt == 16) {
              xoProgramPage(pageBuf);
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
  } else {
    Serial.print("Problem opening: ");
    Serial.println(fileName);
  }
  return (byteCnt);
}




// the setup function runs once when you press reset or power the board
void setup()
{
#if defined(__SAMD51__) || defined(NRF52840_XXAA)
  flash.begin();
#else
  flash.begin(FLASH_TYPE);
#endif

  pinMode(LED_BUILTIN, OUTPUT);

  Wire.begin();

  Serial.begin(115200);
  while ( !Serial ) delay(10);   // wait for native usb

  Serial.println("Lattice MachXO I2C programming example");

  if (!fatfs.begin()) {
    Serial.println("Error, failed to mount filesystem!");
    Serial.println("Was the flash chip formatted with the fatfs_format example?");
    while (1);
  }
  Serial.println("Mounted filesystem!");

  uint8_t dataBuf[32];
  char stringBuf[32];

  Serial.print("IDCODE_PUB:  ");
  xoReadDeviceID(dataBuf);
  sprintf(stringBuf, "0x%02X%02X%02X%02X",
          dataBuf[0], dataBuf[1], dataBuf[2], dataBuf[3]);
  Serial.println(stringBuf);

  Serial.print("USERCODE:  ");
  xoReadUserCode(dataBuf);
  sprintf(stringBuf, "0x%02X%02X%02X%02X",
          dataBuf[0], dataBuf[1], dataBuf[2], dataBuf[3]);
  Serial.println(stringBuf);

  Serial.print("LSC_READ_STATUS:  ");
  xoReadStatus(dataBuf);
  sprintf(stringBuf, "0x%02X%02X%02X%02X",
          dataBuf[0], dataBuf[1], dataBuf[2], dataBuf[3]);
  Serial.println(stringBuf);

  Serial.print("LSC_READ_FEABITS:  ");
  xoReadFeatureBits(dataBuf);
  sprintf(stringBuf, "0x%02X%02X",
          dataBuf[0], dataBuf[1]);
  Serial.println(stringBuf);

  Serial.print("LSC_READ_FEATURE:  ");
  xoReadFeatureRow(dataBuf);
  sprintf(stringBuf, "0x%02X%02X%02X%02X%02X%02X%02X%02X",
          dataBuf[0], dataBuf[1], dataBuf[2], dataBuf[3],
          dataBuf[4], dataBuf[5], dataBuf[6], dataBuf[7]);
  Serial.println(stringBuf);

  Serial.println("Enable offline configuration...");
  xoEnableConfigOffline();
  Serial.println("Erasing...");
  xoErase(XO_ERASE_CONFIG_FLASH | XO_ERASE_UFM);
  xoWaitBusy();
  Serial.println("Erased");
  Serial.println("Loading machxo3.hex");
  xoLoadHex(HEX_FILE_NAME);  
  Serial.println("Programming done");
  xoProgramDone();
  Serial.print("LSC_READ_STATUS:  ");
  xoReadStatus(dataBuf);
  sprintf(stringBuf, "0x%02X%02X%02X%02X",
          dataBuf[0], dataBuf[1], dataBuf[2], dataBuf[3]);
  Serial.println(stringBuf);
  xoRefresh();
  Serial.println("Exited configuration mode");


}

void loop()
{
  // nothing to do
}
