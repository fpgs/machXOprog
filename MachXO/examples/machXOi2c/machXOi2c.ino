/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

/* This example shows how to access the configuration port in a Lattice MachXO2/3
 *   device.  It utilizes the TinyUSB from Adafruit to expose on-board external Flash 
 *   as USB Mass Storage to simplify file transfers.  A simple serial command interface 
 *   is provided to allow for interacting with the target device. 
 * Following libraries are required
 *   - MachXO (this library)
 *   - Adafruit_TinyUSB https://github.com/adafruit/Adafruit_TinyUSB_Arduino
 *   - Adafruit_SPIFlash https://github.com/adafruit/Adafruit_SPIFlash
 *   - SdFat https://github.com/adafruit/SdFat
 *
 * Note: Adafruit fork of SdFat enabled ENABLE_EXTENDED_TRANSFER_CLASS and FAT12_SUPPORT
 * in SdFatConfig.h, which is needed to run SdFat on external flash. You can use original
 * SdFat library and manually change those macros
 */

#include "SPI.h"
#include "SdFat.h"
#include "Adafruit_SPIFlash.h"
#include "Adafruit_TinyUSB.h"
#include "MachXO.h"

#if defined(__SAMD51__) || defined(NRF52840_XXAA)
  Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS, PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
#else
  #if (SPI_INTERFACES_COUNT == 1)
    Adafruit_FlashTransport_SPI flashTransport(SS, &SPI);
  #else
    Adafruit_FlashTransport_SPI flashTransport(SS1, &SPI1);
  #endif
#endif

Adafruit_SPIFlash flash(&flashTransport);

// file system object from SdFat
FatFileSystem fatfs;

FatFile root;
FatFile file;

// USB Mass Storage object
Adafruit_USBD_MSC usb_msc;

// Lattice MachXO Programmer
MachXO machXO;

// Additional Programming I/O
#define XO_PRGMN (9)
#define XO_INITN (10)

// Set to true when PC write to flash
bool changed;

#define COMMAND_BUFFER_LENGTH  127
int  charCnt;
char c;
char commandBuf[COMMAND_BUFFER_LENGTH +1];
const char *delimiters            = ", \t";

#define RD1124_I2C_ADDR  0x41

// the setup function runs once when you press reset or power the board
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(XO_PRGMN, INPUT_PULLUP);
  pinMode(XO_INITN, INPUT_PULLUP);
  digitalWrite(XO_PRGMN, LOW);
  digitalWrite(XO_INITN, LOW);

  flash.begin();

  // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
  usb_msc.setID("Lattice", "MachXO I2C", "0.1");

  // Set callback
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);

  // Set disk size, block size should be 512 regardless of spi flash page size
  usb_msc.setCapacity(flash.pageSize()*flash.numPages()/512, 512);

  // MSC is ready for read/write
  usb_msc.setUnitReady(true);
  
  usb_msc.begin();

  // Init file system on the flash
  fatfs.begin(&flash);

  Wire.begin();

  Serial.begin(115200);
  while ( !Serial ) delay(10);   // wait for native usb

  Serial.println("Lattice MachXO I2C/SPI Programming example, utilizing");
  Serial.println("Adafruit TinyUSB Mass Storage External Flash example");
  Serial.println("Includes I2C access for RD1124");
  Serial.print("JEDEC ID: "); Serial.println(flash.getJEDECID(), HEX);
  Serial.print("Flash size: "); Serial.println(flash.size());

  changed = true; // to print contents initially
  charCnt = 0;
  commandBuf[0] = 0;
}

void loop()
{
  while (Serial.available()) {
    c = Serial.read();
    switch (c) {
      case '\r':
      case '\n':
        commandBuf[charCnt] = 0;
        if (charCnt > 0) {
          runCommand();
          charCnt = 0;
        }
        break;
      case '\b':
        if (charCnt >0) {
          commandBuf[--charCnt] = 0;
          Serial << byte('\b') << byte(' ') << byte('\b');
        }
        break;
      default:
        if (charCnt < COMMAND_BUFFER_LENGTH) {
          commandBuf[charCnt++] = c;
        }
        commandBuf[charCnt] = 0;
        break;
    }
  }
  
  if ( changed )
  {
    changed = false;
    printDir();
  }
  
//  Serial.println();
  delay(100); // refresh every 0.1 second

}

void printBufHEX(char *msg, uint8_t *buf, int cnt) {
  Serial.print(msg);
  Serial.print("0x");
  char tmpStr[3];
  for (int i = 0; i < cnt; i++) {
    sprintf(tmpStr, "%02X", buf[i]);
    Serial.print(tmpStr);
  }
  Serial.println();
}

void xoDetails() {
  uint8_t dataBuf[8];
  machXO.readDeviceID(dataBuf);
  printBufHEX("Device ID:  ", dataBuf, 4);
  machXO.readUserCode(dataBuf);
  printBufHEX("User Code:  ", dataBuf, 4);
  machXO.readFeatureRow(dataBuf);
  printBufHEX("Feature Row:  ", dataBuf, 8);
  machXO.readFeatureBits(dataBuf);
  printBufHEX("Feature Bits:  ", dataBuf, 2);
  machXO.readStatus(dataBuf);
  printBufHEX("Status:  ", dataBuf, 4);
}

void xoStatus() {
  uint8_t dataBuf[4];
  machXO.readStatus(dataBuf);
  printBufHEX("Status:  ", dataBuf, 4);
}

void xoConfig() {
  machXO.enableConfigOffline();
  Serial.println("Enabled Offline Configuration");
}

void xoRefresh() {
  machXO.refresh();
  Serial.println("Refreshed MachXO Configuration");
}

void xoErase() {
  machXO.erase(MACHXO_ERASE_CONFIG_FLASH | MACHXO_ERASE_UFM);
  Serial.println("Erasing...");
  machXO.waitBusy();
  Serial.println("Erased Configuration and UFM");
}

void xoProgPin(char *prog) {
  switch (prog[0]) {
    case '0':
    case 'L':
    case 'l':
      digitalWrite(XO_PRGMN, LOW);
      pinMode(XO_PRGMN, OUTPUT);
      Serial.println("Program_N pin low");
      break;
    case '1':
    case 'H':
    case 'h':
    default:
      pinMode(XO_PRGMN, INPUT_PULLUP);
      Serial.println("Program_N pin high");
      break;  
  }
}

void xoInitPin(char *init) {
  switch (init[0]) {
    case '0':
    case 'L':
    case 'l':
      digitalWrite(XO_INITN, LOW);
      pinMode(XO_INITN, OUTPUT);
      Serial.println("Init_N pin low");
      break;
    case '1':
    case 'H':
    case 'h':
    default:
      pinMode(XO_INITN, INPUT_PULLUP);
      Serial.println("Init_N pin high");
      break;  
  }
}

void xoLoadHEX(char *filename) {
  if (!file.open(filename)) {
      Serial.print("Failed to open ");
      Serial.println(filename);
      return;    
  } else {
    Serial.print("Opened ");
    Serial.println(filename);
    machXO.loadHex(file);
    file.close();
    Serial.print("Loaded ");    
    Serial.println(filename);   
    machXO.programDone(); 
  }
}

void rd1124Enable() {
  Wire.beginTransmission(RD1124_I2C_ADDR);
  Wire.write(0x06);
  Wire.endTransmission(true);
  Serial.println("RD1124 Enabled");
}

void rd1124Disable() {
  Wire.beginTransmission(RD1124_I2C_ADDR);
  Wire.write(0x04);
  Wire.endTransmission(true);
  Serial.println("RD1124 Disabled");
}

void rd1124MemTst() {
  uint8_t dataBuf[8];
  Wire.beginTransmission(RD1124_I2C_ADDR);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.write(0x01);
  Wire.write(0x02);
  Wire.write(0x03);
  Wire.write(0x04);
  Wire.write(0x05);
  Wire.write(0x06);
  Wire.write(0x07);
  Wire.endTransmission(true);
  Wire.beginTransmission(RD1124_I2C_ADDR);
  Wire.write(0x02);
  Wire.write(0x08);
  Wire.write(0x10);
  Wire.write(0x11);
  Wire.write(0x12);
  Wire.write(0x13);
  Wire.write(0x14);
  Wire.write(0x15);
  Wire.write(0x16);
  Wire.write(0x17);
  Wire.endTransmission(true);
  Serial.println("Filled with:  0x00-0x07, 0x10-0x17");
  Wire.beginTransmission(RD1124_I2C_ADDR);
  Wire.write(0x0B);
  Wire.write(0x04);
  Wire.endTransmission(false);
  Wire.requestFrom(RD1124_I2C_ADDR, 8);
  if (Wire.available() >= 8) {
    for (int i = 0; i < 8; i++) {
      dataBuf[i] = Wire.read();
    }
  }
  printBufHEX("Read back from 0x04:  ", dataBuf, 8);
}



void rd1124Help(){
  Serial.println("MachXO RD1124 Interface");
  Serial.println("Implemented commands:");
  Serial.println("Z/z D/d   - RD1124 Disable");
  Serial.println("Z/z E/e   - RD1124 Enable");
  Serial.println("Z/z M/m   - RD1124 Memory Test");
}

void xoRD1124(char *rd1124cmd) {
  switch (rd1124cmd[0]) {
    case 'E':
    case 'e':
      rd1124Enable();
      break;
    case 'D':
    case 'd':
      rd1124Disable();
      break;
    case 'M':
    case 'm':
      rd1124MemTst();
      break;
    default:
      Serial.print("Unknown command:  ");
      Serial.println(rd1124cmd);
      rd1124Help();
  }
}

void printDir() {
    if ( !root.open("/") )
    {
      Serial.println("open root failed");
      return;
    }

    Serial.println("Flash contents:");

    // Open next file in root.
    // Warning, openNext starts at the current directory position
    // so a rewind of the directory may be required.
    while ( file.openNext(&root, O_RDONLY) )
    {
      file.printFileSize(&Serial);
      Serial.write(' ');
      file.printName(&Serial);
      if ( file.isDir() )
      {
        // Indicate a directory.
        Serial.write('/');
      }
      Serial.println();
      file.close();
    }
    root.close();  
    Serial.println();
}

void printHelp(){
  Serial.println("MachXO Programming Example Command Interface");
  Serial.println("Valid commands:");
  Serial.println("H/h/? - Print this help information");
  Serial.println("D/d   - Print root directory");
  Serial.println("X/x   - MachXO Device Details");
  Serial.println("S/s   - MaxhXO Status");
  Serial.println("C/c   - MaxhXO enable offline Configuration");
  Serial.println("E/e   - MaxhXO Erase configuration and UFM");
  Serial.println("R/r   - MachXO Refresh");
  Serial.println("I/i [0/low/1/high] - MachXO INITN pin");
  Serial.println("P/p [0/low/1/high] - MachXO PROGRAMN pin");
  Serial.println("L/l [filename] - MachXO Load hex file");
  Serial.println("Z/z [command] - I2C RD1124 Command");
}

void runCommand() {
  char * commandName = strtok(commandBuf, delimiters);
  switch (commandName[0]) {
    case 'H':
    case 'h':
    case '?':
      printHelp();
      break;
    case 'X':
    case 'x':
      xoDetails();
      break;
    case 'S':
    case 's':
      xoStatus();
      break;
    case 'C':
    case 'c':
      xoConfig();
      break;
    case 'E':
    case 'e':
      xoErase();
      break;
    case 'R':
    case 'r':
      xoRefresh();
      break;
    case 'L':
    case 'l':
      xoLoadHEX(strtok(NULL, delimiters));
      break;
    case 'P':
    case 'p':
      xoProgPin(strtok(NULL, delimiters));
      break;
    case 'I':
    case 'i':
      xoInitPin(strtok(NULL, delimiters));
      break;
    case 'Z':
    case 'z':
      xoRD1124(strtok(NULL, delimiters));
      break;
    case 'D':
    case 'd':
      printDir();
      break;
    default:
      Serial.print("Unknown command:  ");
      Serial.println(commandName);
      printHelp();
  }
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and 
// return number of copied bytes (must be multiple of block size) 
int32_t msc_read_cb (uint32_t lba, void* buffer, uint32_t bufsize)
{
  // Note: SPIFLash Bock API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.readBlocks(lba, (uint8_t*) buffer, bufsize/512) ? bufsize : -1;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and 
// return number of written bytes (must be multiple of block size)
int32_t msc_write_cb (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
{
  digitalWrite(LED_BUILTIN, HIGH);

  // Note: SPIFLash Bock API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.writeBlocks(lba, buffer, bufsize/512) ? bufsize : -1;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_cb (void)
{
  // sync with flash
  flash.syncBlocks();

  // clear file system's cache to force refresh
  fatfs.cacheClear();

  changed = true;

  digitalWrite(LED_BUILTIN, LOW);
}
