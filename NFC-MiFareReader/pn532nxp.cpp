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

    Serial.println("PN532NXP::discover");
    printBuffer(buffer, 12);

    uint8_t offset = 7;
    boardVersion = buffer[offset++];
    firmwareMajor = buffer[offset++];
    firmwareMinor = buffer[offset];

    return true;
}

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

void PN532NXP::writeCommand(PN532NXP::Commands command)
{
    uint8_t checksum;

    // "Commands" being an enum will never have a size greater than 255 bytes,
    // force conversion of long to byte
    auto numBytes = (byte)sizeof(command);
    numBytes = numBytes < 2 ? numBytes + 1 : numBytes;

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

    Serial.print("writeCommand: 0x");Serial.println(command, HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)PN532_PREAMBLE, HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)PN532_PREAMBLE, HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)PN532_STARTCODE2, HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)numBytes, HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)(~numBytes + 1), HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)PN532_HOSTTOPN532, HEX);

    for (uint8_t i = 0; i < numBytes - 1; i++)
    {
        WIRE.write(cmd[i]);
        checksum += cmd[i];
        Serial.print(F(" 0x"));
        Serial.print((byte)cmd[i], HEX);
    }

    WIRE.write((byte)~checksum);
    WIRE.write((byte)PN532_POSTAMBLE);

    WIRE.endTransmission();

    Serial.print(F(" 0x"));
    Serial.print((byte)~checksum, HEX);
    Serial.print(F(" 0x"));
    Serial.print((byte)PN532_POSTAMBLE, HEX);
    Serial.println();
}

bool PN532NXP::ackWriteCommand(PN532NXP::Commands command, uint16_t timeout)
{
    writeCommand(command);

    if (!isReady(timeout))
    {
        return false;
    }

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
    for(int i = 0; i < 6; i++)
    {
        isEqual &= (buff[i] == PN532_ACK_SEQUENCE[i]);
    }

    return isEqual;
}

void PN532NXP::printBuffer(uint8_t *buffer, uint8_t length)
{
    for (int i = 0; i < length; i++)
    {
        Serial.print(F(" 0x"));
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}