#include "nfc-mifarereader-i2c.h"
#include "pn532nxp.h"

#include <Wire.h>

NFCMiFareReaderI2C::NFCMiFareReaderI2C()
{
    pn532 = new PN532NXP(PN532_IRQ, PN532_RESET);
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

    // TO DO: Implement and Call SecureAcessManager Config (see Adafruit PN532 Library)
}