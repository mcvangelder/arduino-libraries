#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "nfc-mifareclassic-spi.h"

NFCMiFareClassicSpi::NFCMiFareClassicSpi()
{
  nfc = new Adafruit_PN532(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
}

void NFCMiFareClassicSpi::initialize()
{
#ifndef ESP8266
  while (!Serial)
    ; // for Leonardo/Micro/Zero
#endif
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc->begin();

  uint32_t versiondata = nfc->getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("Didn't find PN53x board");
    while (1)
      ;
  }

  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc->SAMConfig();

  Serial.println("Waiting for an ISO14443A Card ...");
}

ReadStatus NFCMiFareClassicSpi::read()
{
  ReadStatus status;
  uint8_t uidRaw[] = {0,0,0,0,0,0,0};
  uint8_t uidLength;

  status.success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uidRaw, &uidLength);
  status.uidRaw = uidRaw;
  status.uidLength = uidLength;

  if (status.success)
  {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");
    Serial.print(status.uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc->PrintHex(status.uidRaw, status.uidLength);

    Serial.println("");
  }
  return status;
}
