#ifndef NFC_MIFAREREADER_I2C_H
#define NFC_MIFAREREADER_I2C_H

#include "nfc-mifarereader.h"
#include "pn532nxp.h"

class NFCMiFareReaderI2C : public NFCMiFareReader
{
public:
    NFCMiFareReaderI2C();
    void initialize();
    uint8_t read(ReadStatus &status);

private:
    PN532NXP *pn532;
};
#endif