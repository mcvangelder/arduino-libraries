#include "pn532nxp.h"
#include <Wire.h>

PN532NXP::PN532NXP(uint8_t irqPin, uint8_t resetPin) : m_irqPin(irqPin),
                                                       m_resetPin(resetPin)
{
    pinMode(m_irqPin, INPUT);
    pinMode(m_resetPin, OUTPUT);
}

void PN532NXP::begin()
{
    WIRE.begin();
    resetPN532();
}

void PN532NXP::resetPN532()
{
    digitalWrite(m_resetPin, HIGH);
    digitalWrite(m_resetPin, LOW);
    delay(400);
    digitalWrite(m_resetPin, HIGH);
    delay(10);
}

bool PN532NXP::discover(uint8_t &boardVersion, uint8_t &firmwareMajor, uint8_t &firmwareMinor)
{
    uint8_t buffer[12];

    if (!ackWriteCommand(Commands::GET_FIRMWARE_VERSION))
    {
        return false;
    }

    read(buffer, 12);

#ifdef PN532NXP_DEBUG
    printBuffer(buffer, 12, "PN532NXP::discover");
#endif

    uint8_t offset = 7;
    boardVersion = buffer[offset++];
    firmwareMajor = buffer[offset++];
    firmwareMinor = buffer[offset];

    return true;
}

bool PN532NXP::configureSecureAccessModule()
{
    uint8_t normalMode = 0x01;
    uint8_t timeout = 0x14; // 50ms
    uint8_t useIRQ = 0x01;
    uint8_t data[3] = {normalMode, timeout, useIRQ};
    uint8_t successValue = 0x15;

    uint8_t responseBuff[8];

    if (!ackWriteCommand(Commands::SAM_CONFIGURATION, data, 3))
    {
        return false;
    }

    read(responseBuff, 8);

    return (responseBuff[6] == successValue);
}

bool PN532NXP::setPassiveReadCardMode(uint8_t cardBaudeRate, uint16_t timeout)
{
    uint8_t maxCards = 0x1;
    uint8_t data[2] = {maxCards, cardBaudeRate};

    if (!ackWriteCommand(Commands::INLIST_PASSIVE_TARGET, data, 2, timeout))
    {
#ifdef PN532NXP_DEBUG
        Serial.println("PN532NXP::readPassiveCardTargetId: Unable to confirm command received");
#endif
        return false;
    }
    return true;
}

/*
 * This function will read the value of the card read by the PN532 chip. It assumes that
 * #setPassiveReadCardMode has been previously called and was successful.
 * 
 * This allows support for interrupt driven event handling.
*/
bool PN532NXP::readPassiveCardTargetId(uint8_t *uid, uint8_t &uidLength, uint16_t timeout)
{
    if (!isReady(timeout))
    {
#ifdef PN532NXP_DEBUG
        Serial.println("PN532NXP::readPassiveCardTargetId: IRQ Timeout!!!");
#endif
        return false;
    }

    uint8_t response[20];
    read(response, 20);

    /* ISO14443A card response should be in the following format:

    byte            Description
    -------------   ------------------------------------------
    b0..6           Frame header and preamble
    b7              Tags Found
    b8              Tag Number
    b9..10          SENS_RES
    b11             SEL_RES
    b12             NFCID Length
    b13..NFCIDLen   NFCID                                      */

    auto numTags = response[7];
    if (numTags != 1)
    {
        return false;
    }

    uidLength = response[12];
    auto offset = 13;
    for (uint8_t i = 0; i < uidLength; i++)
    {
        uid[i] = response[offset + i];
    }

#ifdef PN532NXP_DEBUG
    printBuffer(uid, uidLength, "PN532NXP::readPassiveCardTargetId: UID Info:");
#endif
    return true;
}

// Begin private Functions
void PN532NXP::read(uint8_t *buffer, uint8_t length)
{
    uint16_t timer = 0;

    // Take into account leading 0x01 with I2C, add additional 2 bytes requested
    WIRE.requestFrom((uint8_t)PN532_I2C_ADDRESS, (uint8_t)(length + 2));
    // Ignoring leading 0x01
    WIRE.read();

    // WIRE.readBytes(buffer, length);
    for (uint8_t i = 0; i < length; i++)
    {
        delay(1);
        buffer[i] = WIRE.read();
    }
}

void PN532NXP::writeCommand(PN532NXP::Commands command, uint8_t *data, uint8_t dataLength)
{
    uint8_t checksum;

    auto numBytes = 1; // each command is 1 byte
    numBytes += dataLength + 1;

    PN532NXP::Commands *cmd = &command;

    WIRE.beginTransmission(PN532_I2C_ADDRESS);
    checksum = PN532_PREAMBLE + PN532_PREAMBLE + PN532_STARTCODE2;
    WIRE.write(PN532_PREAMBLE);
    WIRE.write(PN532_PREAMBLE);
    WIRE.write(PN532_STARTCODE2);

    WIRE.write(numBytes);
    WIRE.write(~numBytes + 1);

    WIRE.write(PN532_HOSTTOPN532);
    checksum += PN532_HOSTTOPN532;

#ifdef PN532NXP_DEBUG
    Serial.print("PN532NXP::writeCommand: 0x");
    Serial.println(command, HEX);
    Serial.print(F("> 0x"));
    Serial.print((byte)PN532_PREAMBLE, HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)PN532_PREAMBLE, HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)PN532_STARTCODE2, HEX);
    Serial.print(F(" => 0x"));
    Serial.print((byte)numBytes, HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)(~numBytes + 1), HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)PN532_HOSTTOPN532, HEX);
#endif

    WIRE.write(cmd[0]);
    checksum += cmd[0];
#ifdef PN532NXP_DEBUG
    Serial.print(F(" -> 0x"));
    Serial.print(cmd[0], HEX);
#endif

    for (int i = 0; i < dataLength; i++)
    {
        WIRE.write(data[i]);
        checksum += data[i];
#ifdef PN532NXP_DEBUG
        Serial.print(F(" ** 0x"));
        Serial.print(data[i], HEX);
#endif
    }

    WIRE.write((byte)~checksum);
    WIRE.write((byte)PN532_POSTAMBLE);

#ifdef PN532NXP_DEBUG
    Serial.print(F(" 0x"));
    Serial.print((byte)~checksum, HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)PN532_POSTAMBLE, HEX);
    Serial.println(" <");
#endif

    WIRE.endTransmission();
}

bool PN532NXP::ackWriteCommand(PN532NXP::Commands command, uint8_t *data, uint8_t dataLength, uint16_t timeout)
{
    writeCommand(command, data, dataLength);

#ifdef PN532NXP_DEBUG
    Serial.print("PN532NXP::ackWriteCommand: Waiting for ack... Timeout (ms): "); Serial.println(timeout);
#endif

    if (!isReady(timeout))
    {
#ifdef PN532NXP_DEBUG
        Serial.println("PN532NXP::ackWriteCommand: No response from board.");
#endif
        return false;
    }

#ifdef PN532NXP_DEBUG
    Serial.println("PN532NXP::ackWriteCommand: IRQ Received");
#endif
    return isAcknowledged();
}

bool PN532NXP::isReady(uint16_t timeout)
{
    uint16_t timer = 0;
    bool isReady = false;

    do
    {
        isReady = digitalRead(m_irqPin) == LOW;
        if (!isReady && timeout != 0)
        {
            timer += 10;
            if (timer > timeout)
            {
                break;
            }
        }
    } while (!isReady);

    return isReady;
}

bool PN532NXP::isAcknowledged()
{
    uint8_t buff[6];

    read(buff, 6);

    auto isEqual = true;
    for (int i = 0; i < 6; i++)
    {
        isEqual &= (buff[i] == PN532_ACK_SEQUENCE[i]);
    }

    return isEqual;
}

void PN532NXP::printBuffer(uint8_t *buffer, uint8_t length, const char* header)
{
#ifdef PN532NXP_DEBUG
    Serial.println(header);
    for (int i = 0; i < length; i++)
    {
        Serial.print(F(" 0x"));
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
#endif
}