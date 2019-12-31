#include "nfc-mifarereader-spi.h"

NFCMiFareReaderSpi::NFCMiFareReaderSpi()
{
  nfc = new Adafruit_PN532(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
}