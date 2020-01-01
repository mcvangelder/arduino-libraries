#include <Wire.h>
#include <SPI.h>
#include "nfc-mifarereader.h"

void NFCMiFareReader::initialize()
{
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
  Serial.println(); Serial.println(); Serial.println();
}

uint8_t NFCMiFareReader::read(ReadStatus &status)
{
  Serial.println("Waiting for an ISO14443A Card ...");
  status.success = nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, status.uidRaw, &status.uidLength);

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
  
  return status.success;
}
