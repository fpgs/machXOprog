/*
  SD card test

  This example shows how use the utility libraries on which the'
  SD library is based in order to get info about your SD card.
  Very useful for testing a card when you're not sure whether its working or not.

  The circuit:
    SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module.
 		Pin 4 used here for consistency with other Arduino examples


  created  28 Mar 2011
  by Limor Fried
  modified 9 Apr 2012
  by Tom Igoe
*/
// include the SD library:
#include <SPI.h>
#include <SD.h>

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int sdCSpin = 4; //Feather M0 Adalogger
const int xoSnPin = 5;

const int LSC_ERASE_SRAM = 1 << 16;
const int LSC_ERASE_FEATURE_ROW = 1 << 17;
const int LSC_ERASE_CONFIG_FLASH = 1 << 18;
const int LSC_ERASE_UFM = 1 << 19;


uint32_t read_device_id() {
  uint32_t devID = 0;
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xE0); // IDCODE_PUB
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  devID = SPI.transfer(0x00) << 24;
  devID |= SPI.transfer(0x00) << 16;
  devID |= SPI.transfer(0x00) << 8;
  devID |= SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (devID);
}

uint32_t read_user_code() {
  uint32_t userCode = 0;
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xC0); // USERCODE
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  userCode = SPI.transfer(0x00) << 24;
  userCode |= SPI.transfer(0x00) << 16;
  userCode |= SPI.transfer(0x00) << 8;
  userCode |= SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (userCode);
}

uint32_t read_status() {
  uint32_t statusReg = 0;
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0x3C); // LSC_READ_STATUS
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  statusReg = SPI.transfer(0x00) << 24;
  statusReg |= SPI.transfer(0x00) << 16;
  statusReg |= SPI.transfer(0x00) << 8;
  statusReg |= SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (statusReg);
}

uint32_t read_feature_bits() {
  uint32_t featureBits = 0;
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xFB); // LSC_READ_FEABITS
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  featureBits |= SPI.transfer(0x00) << 8;
  featureBits |= SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (featureBits);
}

uint32_t read_feature_row(uint8_t *readBuf) {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xE7); // LSC_READ_FEATURE
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  readBuf[0] = SPI.transfer(0x00);
  readBuf[1] = SPI.transfer(0x00);
  readBuf[2] = SPI.transfer(0x00);
  readBuf[3] = SPI.transfer(0x00);
  readBuf[4] = SPI.transfer(0x00);
  readBuf[5] = SPI.transfer(0x00);
  readBuf[6] = SPI.transfer(0x00);
  readBuf[7] = SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (8);
}

uint32_t read_otp_fuses() {
  uint32_t otpFuses = 0;
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xFA); // LSC_READ_OTP
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  otpFuses = SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (otpFuses);
}

uint32_t read_flash(uint8_t *readBuf) {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0x73); // LSC_READ_INCR_NV
  SPI.transfer(0x10); // SSPI interface
  SPI.transfer(0x00);
  SPI.transfer(0x01); // 1 page
  readBuf[0] = SPI.transfer(0x00);
  readBuf[1] = SPI.transfer(0x00);
  readBuf[2] = SPI.transfer(0x00);
  readBuf[3] = SPI.transfer(0x00);
  readBuf[4] = SPI.transfer(0x00);
  readBuf[5] = SPI.transfer(0x00);
  readBuf[6] = SPI.transfer(0x00);
  readBuf[7] = SPI.transfer(0x00);
  readBuf[8] = SPI.transfer(0x00);
  readBuf[9] = SPI.transfer(0x00);
  readBuf[10] = SPI.transfer(0x00);
  readBuf[11] = SPI.transfer(0x00);
  readBuf[12] = SPI.transfer(0x00);
  readBuf[13] = SPI.transfer(0x00);
  readBuf[14] = SPI.transfer(0x00);
  readBuf[15] = SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (16);
}

uint32_t read_ufm(uint8_t *readBuf) {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xCA); // LSC_READ_NVCM1/UFM
  SPI.transfer(0x10); // SSPI interface
  SPI.transfer(0x00);
  SPI.transfer(0x01); // 1 page
  readBuf[0] = SPI.transfer(0x00);
  readBuf[1] = SPI.transfer(0x00);
  readBuf[2] = SPI.transfer(0x00);
  readBuf[3] = SPI.transfer(0x00);
  readBuf[4] = SPI.transfer(0x00);
  readBuf[5] = SPI.transfer(0x00);
  readBuf[6] = SPI.transfer(0x00);
  readBuf[7] = SPI.transfer(0x00);
  readBuf[8] = SPI.transfer(0x00);
  readBuf[9] = SPI.transfer(0x00);
  readBuf[10] = SPI.transfer(0x00);
  readBuf[11] = SPI.transfer(0x00);
  readBuf[12] = SPI.transfer(0x00);
  readBuf[13] = SPI.transfer(0x00);
  readBuf[14] = SPI.transfer(0x00);
  readBuf[15] = SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (16);
}

uint32_t erase_ufm() {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xCB); // LSC_ERASE_TAG
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (0);
}

uint32_t erase(uint32_t eraseFlags) {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0x0E); // LSC_ERASE
  SPI.transfer(0xFF & (eraseFlags >> 16)); // erase flag bits
  SPI.transfer(0x00); // lower flag bits all zero
  SPI.transfer(0x00); // lower flag bits all zero
  digitalWrite(xoSnPin, HIGH);
  return (0);
}

uint32_t enable_config_transparenet() {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0x74); // LSC_ENABLE_X
  SPI.transfer(0x08);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (0);
}

uint32_t enable_config_offline() {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xC6); // LSC_ENABLE
  SPI.transfer(0x08);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (0);
}

uint32_t check_busy() {
  uint8_t busyByte = 0;
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xF0); // LSC_CHECK_BUSY
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  busyByte = SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return ((busyByte & 0x80) ? 1 : 0);
}

uint32_t wait_busy() {
  uint32_t waitCnt = 0;
  while (check_busy()) {
    waitCnt += 1;
  }
  return (waitCnt);
}

uint32_t reset_config_addr() {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0x46); // LSC_INIT_ADDRESS
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (0);
}

uint32_t reset_ufm_addr() {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0x47); // LSC_INIT_ADDR_UFM
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (0);
}

uint32_t set_config_addr(uint32_t page) {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xB4); // LSC_WRITE_ADDRESS
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00); // Configuration Flash
  SPI.transfer(0x00);
  SPI.transfer(0xFF & (page >> 8)); // high page byte
  SPI.transfer(0xFF & page); // low page byte
  digitalWrite(xoSnPin, HIGH);
  return (0);
}

uint32_t set_ufm_addr(uint32_t page) {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xB4); // LSC_WRITE_ADDRESS
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x40); // UFM
  SPI.transfer(0x00);
  SPI.transfer(0xFF & (page >> 8)); // high page byte
  SPI.transfer(0xFF & page); // low page byte
  digitalWrite(xoSnPin, HIGH);
  return (0);
}

uint32_t program_page(uint8_t *writeBuf) {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0x70); // LSC_PROG_INCR_NV
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x01); // 1 page
  SPI.transfer(writeBuf[0]);
  SPI.transfer(writeBuf[1]);
  SPI.transfer(writeBuf[2]);
  SPI.transfer(writeBuf[3]);
  SPI.transfer(writeBuf[4]);
  SPI.transfer(writeBuf[5]);
  SPI.transfer(writeBuf[6]);
  SPI.transfer(writeBuf[7]);
  SPI.transfer(writeBuf[8]);
  SPI.transfer(writeBuf[9]);
  SPI.transfer(writeBuf[10]);
  SPI.transfer(writeBuf[11]);
  SPI.transfer(writeBuf[12]);
  SPI.transfer(writeBuf[13]);
  SPI.transfer(writeBuf[14]);
  SPI.transfer(writeBuf[15]);
  digitalWrite(xoSnPin, HIGH);
  return (16);
}

uint32_t program_done() {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0x5E); // LSC_PROGRAM_DONE
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (0);
}

uint32_t refresh() {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0x79); // LSC_REFRESH
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  digitalWrite(xoSnPin, HIGH);
  return (0);
}

uint32_t wakeup() {
  digitalWrite(xoSnPin, LOW);
  SPI.transfer(0xFF); // LSC_NOOP
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);
  digitalWrite(xoSnPin, HIGH);
  return (0);
}

uint32_t load_hex(char *fileName) {
  int byteCnt = 0;
  File hexFile = SD.open(fileName);
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
    reset_config_addr();
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
              program_page(pageBuf);
            }
            pageCnt += 1;
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



void setup() {
  uint8_t spiBuf[32];
  char stringBuf[32];
  pinMode(xoSnPin, OUTPUT);  // initialize XO chip select pin
  digitalWrite(xoSnPin, HIGH);
  SPI.begin();
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("\nInitializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(sdCSpin)) {
    Serial.println("Card failed, or not present");
  } else {
    Serial.println("card initialized.");
  }

  uint32_t returnVal;
  Serial.print("IDCODE_PUB:  ");
  sprintf(stringBuf, "0x%08X", read_device_id());
  Serial.println(stringBuf);

  Serial.print("USERCODE:  ");
  sprintf(stringBuf, "0x%08X", read_user_code());
  Serial.println(stringBuf);

  Serial.print("LSC_READ_STATUS:  ");
  sprintf(stringBuf, "0x%08X", read_status());
  Serial.println(stringBuf);

  Serial.print("LSC_READ_FEABITS:  ");
  sprintf(stringBuf, "0x%04X", read_feature_bits());
  Serial.println(stringBuf);

  Serial.print("LSC_READ_FEATURE:  ");
  returnVal = read_feature_row(spiBuf);
  sprintf(stringBuf, "0x%02X%02X%02X%02X%02X%02X%02X%02X",
          spiBuf[0], spiBuf[1], spiBuf[2], spiBuf[3],
          spiBuf[4], spiBuf[5], spiBuf[6], spiBuf[7]);
  Serial.println(stringBuf);

  Serial.println("Enable offline configuration...");
  enable_config_offline();
  Serial.println("Erasing...");
  erase(LSC_ERASE_CONFIG_FLASH | LSC_ERASE_UFM);
  wait_busy();
  Serial.println("Erased");
  Serial.println("Loading machxo3.hex");
  load_hex("machxo3.hex");  
  Serial.println("Programming done");
  program_done();
  Serial.print("LSC_READ_STATUS:  ");
  sprintf(stringBuf, "0x%08X", read_status());
  Serial.println(stringBuf);

}

void loop(void) {
}
