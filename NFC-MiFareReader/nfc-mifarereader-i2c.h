#ifndef NFC_MIFAREREADER_I2C_H
#define NFC_MIFAREREADER_I2C_H

#include "nfc-mifarereader.h"
#include "pn532nxp.h"

class NFCMiFareReaderI2C : public NFCMiFareReader
{
public:
    NFCMiFareReaderI2C(uint8_t pn532IRQ = PN532_IRQ, uint8_t pn532ResetPin = PN532_RESET);
    void initialize();
    uint8_t read(ReadStatus &status);
    bool activateCardReader();

private:
    PN532NXP *pn532;
};
#endif