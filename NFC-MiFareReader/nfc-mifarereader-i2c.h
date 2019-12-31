#ifndef NFC_MIFAREREADER_I2C_H
#define NFC_MIFAREREADER_I2C_H

#include "nfc-mifarereader.h"

#define PN532_IRQ (2)
#define PN532_RESET (3)

class NFCMiFareReaderI2C : public NFCMiFareReader
{
public:
    NFCMiFareReaderI2C();
};

#endif