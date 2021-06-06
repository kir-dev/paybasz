#include "PermanentMemory.h"
#include <EEPROM.h>

void PermanentMemory::fillEmpty(char * var, unsigned int length) {
    for (unsigned int i = 0; i < length; ++i)
        var[i] = 0;
}

bool PermanentMemory::isLocked() {
    return this->state & 0b01;
}

bool PermanentMemory::isSetup() {
    return (this->state & 0b10) >> 1;
}

void setSetup(bool value) {
    if (value) {
        this->state |= 0b10;
    } else {
        this->state &= (~0b10);
    }
}

void PermanentMemory::save() {
    EEPROM.write(0, status);
    writeToEeprom(pin, STATUS_LENGTH, PIN_LENGTH);
    writeToEeprom(ssid, STATUS_LENGTH + PIN_LENGTH, SSID_LENGTH);
    writeToEeprom(wifiPassword, STATUS_LENGTH + PIN_LENGTH + SSID_LENGTH, WPASSWD_LENGTH);
    writeToEeprom(gatewayName, STATUS_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH, GW_NAME_LENGTH);
    writeToEeprom(token, STATUS_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH + GW_NAME_LENGTH, TOKEN_LENGTH);
    writeToEeprom(baseUrl, STATUS_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH + GW_NAME_LENGTH + TOKEN_LENGTH, BASE_URL_LENGTH);
    EEPROM.commit();
}

void writeToEeprom(char * data, unsigned int from, unsigned int length) {
    for (int i = 0; i < length; ++i)
        EEPROM.write(from + i, data[i]);
}

void PermanentMemory::load() {
    status = EEPROM.read(0);
    loadFromEeprom(pin, STATUS_LENGTH, PIN_LENGTH);
    loadFromEeprom(ssid, STATUS_LENGTH + PIN_LENGTH, SSID_LENGTH);
    loadFromEeprom(wifiPassword, STATUS_LENGTH + PIN_LENGTH + SSID_LENGTH, WPASSWD_LENGTH);
    loadFromEeprom(gatewayName, STATUS_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH, GW_NAME_LENGTH);
    loadFromEeprom(token, STATUS_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH + GW_NAME_LENGTH, TOKEN_LENGTH);
    loadFromEeprom(baseUrl, STATUS_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH + GW_NAME_LENGTH + TOKEN_LENGTH, BASE_URL_LENGTH);
}

void loadFromEeprom(char * data, unsigned int from, unsigned int length) {
    for (int i = 0; i < length; ++i)
        data[i] = EEPROM.read(from + i);
}