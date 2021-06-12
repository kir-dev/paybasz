#include "PermanentMemory.h"
#include <EEPROM.h>

void PermanentMemory::fillEmpty(char * var, unsigned int length) {
    for (unsigned int i = 0; i < length; ++i)
        var[i] = 0;
}

PermanentMemory::PermanentMemory() {
    this->state = 'X';
    this->fillEmpty(this->pinCode, PIN_LENGTH);
    this->fillEmpty(this->ssid, SSID_LENGTH);
    this->fillEmpty(this->wifiPassword, WPASSWD_LENGTH);
    this->fillEmpty(this->gatewayName, GW_NAME_LENGTH);
    this->fillEmpty(this->token, TOKEN_LENGTH);
    this->fillEmpty(this->baseUrl, BASE_URL_LENGTH);
}

bool PermanentMemory::isLocked() {
    return false;
}

bool PermanentMemory::isSetup() {
    return this->state == 'T' || this->state == 0xFF;
}

void PermanentMemory::setSetup(bool value) {
    this->state = value ? 'T' : 'F';
}

void writeToEeprom(char * data, unsigned int from, unsigned int length) {
    for (int i = 0; i < length; ++i) {
        EEPROM.write(from + i, data[i]);
    }
}

void PermanentMemory::save() {
    EEPROM.write(0, this->state);
    writeToEeprom(this->pinCode, STATE_LENGTH, PIN_LENGTH);
    writeToEeprom(this->ssid, STATE_LENGTH + PIN_LENGTH, SSID_LENGTH);
    writeToEeprom(this->wifiPassword, STATE_LENGTH + PIN_LENGTH + SSID_LENGTH, WPASSWD_LENGTH);
    writeToEeprom(this->gatewayName, STATE_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH, GW_NAME_LENGTH);
    writeToEeprom(this->token, STATE_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH + GW_NAME_LENGTH, TOKEN_LENGTH);
    writeToEeprom(this->baseUrl, STATE_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH + GW_NAME_LENGTH + TOKEN_LENGTH, BASE_URL_LENGTH);
    EEPROM.commit();
    Serial.println("[EEPROM] Saved");
}

void loadFromEeprom(char * data, unsigned int from, unsigned int length) {
    for (int i = 0; i < length; ++i) {
        data[i] = EEPROM.read(from + i);
    }
}

void PermanentMemory::load() {
    this->state = EEPROM.read(0);
    loadFromEeprom(this->pinCode, STATE_LENGTH, PIN_LENGTH);
    loadFromEeprom(this->ssid, STATE_LENGTH + PIN_LENGTH, SSID_LENGTH);
    loadFromEeprom(this->wifiPassword, STATE_LENGTH + PIN_LENGTH + SSID_LENGTH, WPASSWD_LENGTH);
    loadFromEeprom(this->gatewayName, STATE_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH, GW_NAME_LENGTH);
    loadFromEeprom(this->token, STATE_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH + GW_NAME_LENGTH, TOKEN_LENGTH);
    loadFromEeprom(this->baseUrl, STATE_LENGTH + PIN_LENGTH + SSID_LENGTH + WPASSWD_LENGTH + GW_NAME_LENGTH + TOKEN_LENGTH, BASE_URL_LENGTH);
}

void PermanentMemory::factoryReset() {
    this->fillEmpty(this->pinCode, PIN_LENGTH);
    this->fillEmpty(this->ssid, SSID_LENGTH);
    this->fillEmpty(this->wifiPassword, WPASSWD_LENGTH);
    this->fillEmpty(this->gatewayName, GW_NAME_LENGTH);
    this->fillEmpty(this->token, TOKEN_LENGTH);
    this->fillEmpty(this->baseUrl, BASE_URL_LENGTH);
    this->setSetup(true);
    this->save();
}