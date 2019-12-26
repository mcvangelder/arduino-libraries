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
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;

  status.success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (status.success)
  {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc->PrintHex(uid, uidLength);

    if (uidLength == 4)
    {
      Serial.print("Seems to be a Mifare Classic card #");
      // We probably have a Mifare Classic card ...
      status.cardId = uid[0];
      status.cardId <<= 8;
      status.cardId |= uid[1];
      status.cardId <<= 8;
      status.cardId |= uid[2];
      status.cardId <<= 8;
      status.cardId |= uid[3];
    }

    Serial.println("");
  }
  return status;
}
