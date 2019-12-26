#include <Adafruit_PN532.h>

#ifndef NFC_MIFARECLASSIC_SPI_H
#define NFC_MIFARECLASSIC_SPI_H

#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

struct ReadStatus {
    uint8_t success;
    uint32_t cardId;
};

class NFCMiFareClassicSpi {
    public:
    NFCMiFareClassicSpi();
    void Initialize();
    ReadStatus Read();

    private:
    Adafruit_PN532* nfc; //(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
};
#endif