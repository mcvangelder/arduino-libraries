#ifndef NFC_MIFAREREADER_SPI_H
#define NFC_MIFAREREADER_SPI_H

#include "nfc-mifarereader.h"

#define PN532_SCK (2)
#define PN532_MOSI (3)
#define PN532_SS (4)
#define PN532_MISO (5)

class NFCMiFareReaderSpi : public NFCMiFareReader
{
public:
    NFCMiFareReaderSpi();
};
#endif