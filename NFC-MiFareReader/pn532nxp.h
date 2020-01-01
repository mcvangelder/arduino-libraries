#ifndef PN532NXP_H
#define PN532NXP_H

#include <Arduino.h>
// #define PN532NXP_DEBUG

#define PN532_IRQ (2)
#define PN532_RESET (3)

#define PN532_I2C_ADDRESS (0x48 >> 1)
#define PN532_I2C_READBIT (0x01)
#define PN532_I2C_BUSY (0x00)
#define PN532_I2C_READY (0x01)
#define PN532_I2C_READYTIMEOUT (20)

#define PN532_PREAMBLE (0x00)
#define PN532_STARTCODE1 (0x00)
#define PN532_STARTCODE2 (0xFF)
#define PN532_POSTAMBLE (0x00)

#define PN532_HOSTTOPN532 (0xD4)
#define PN532_PN532TOHOST (0xD5)

#define PN532_MIFARE_ISO14443A (0x00)

#define WIRE Wire

static constexpr const uint8_t PN532_ACK_SEQUENCE[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
class PN532NXP
{
public:
    PN532NXP(uint8_t irqPin, uint8_t resetPin);
    void begin();
    bool discover(uint8_t &boardVersion, uint8_t &firmwareMajor, uint8_t &firmwareMinor);
    bool configureSecureAccessModule();
    bool readPassiveCardTargetId(uint8_t cardBaudRate, uint8_t *uid, uint8_t &uidLength, uint16_t timeout = 0);

private:
    enum Commands : uint8_t
    {
        GET_FIRMWARE_VERSION = 0x02,
        SAM_CONFIGURATION = 0x14,
        INLIST_PASSIVE_TARGET = 0x4A
    };
    uint8_t m_irqPin;
    uint8_t m_resetPin;

    void resetPN532();
    void read(uint8_t *buffer, uint8_t length);
    void writeCommand(Commands command,  uint8_t *data = nullptr, uint8_t dataLength = 0);
    bool ackWriteCommand(PN532NXP::Commands command, uint8_t *data = nullptr, uint8_t dataLength = 0, uint16_t timeout = 0);
    bool isReady(uint16_t timeout);
    bool isAcknowledged();

    void printBuffer(uint8_t *buffer, uint8_t length, const char* header);
};
#endif