#include "nfc-mifarereader-i2c.h"

NFCMiFareReaderI2C::NFCMiFareReaderI2C()
{
    nfc = new Adafruit_PN532(PN532_IRQ, PN532_RESET);
}