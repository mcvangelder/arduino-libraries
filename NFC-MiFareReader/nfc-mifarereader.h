#ifndef NFC_MIFAREREADER_H
#define NFC_MIFAREREADER_H

#include <Adafruit_PN532.h>

struct ReadStatus
{
    uint8_t success = 0;
    uint8_t uidRaw[7] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t uidLength = 0;
};

class NFCMiFareReader
{
public:
    virtual void initialize();
    virtual uint8_t read(ReadStatus &status);

protected:
    Adafruit_PN532 *nfc;
    NFCMiFareReader() { };
};
#endif