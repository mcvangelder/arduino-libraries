#include "nfc-mifarereader-i2c.h"
#include "pn532nxp.h"

#include <Wire.h>

NFCMiFareReaderI2C::NFCMiFareReaderI2C(uint8_t pn532IRQ, uint8_t pn532ResetPin)
{
    pn532 = new PN532NXP(pn532IRQ, pn532ResetPin);
}

void NFCMiFareReaderI2C::initialize()
{
    Serial.println("NFC MiFare Reader I2C initialize");

    pn532->begin();
    uint8_t boardVer = 0, firmwareMjr = 0, firmwareMnr = 0;

    bool isDiscovered = pn532->discover(boardVer, firmwareMjr, firmwareMnr);
    if (!isDiscovered)
    {
        Serial.print("Didn't find PN53x board");
        while (1)
            ;
    }

    Serial.print("Found chip PN5");
    Serial.println(boardVer, HEX);
    Serial.print("Firmware ver. ");
    Serial.print(firmwareMjr, DEC);
    Serial.print('.');
    Serial.println(firmwareMnr, DEC);

    if (!pn532->configureSecureAccessModule())
    {
        Serial.println("Unable to configure SAM");
        while (1)
            ;
    }
}

bool NFCMiFareReaderI2C::activateCardReader()
{
    return pn532->setPassiveReadCardMode(PN532_MIFARE_ISO14443A);
}

uint8_t NFCMiFareReaderI2C::read(ReadStatus &status)
{
    Serial.println("Reading card");
    status.success = pn532->readPassiveCardTargetId(status.uidRaw, status.uidLength);
    detachInterrupt(PN532_IRQ);

    return status.success;
}