#include "RfidReader.h"
#include "ArduinoSpi3.h"
#include <MFRC522.h>
#include <stdint.h>
#include "mbedtls/md.h"

void RfidReader::hash(char * input, char * output) {
    uint8_t shaResult[32];

    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    const size_t payloadLength = strlen(input);

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char *) input, payloadLength);
    mbedtls_md_finish(&ctx, shaResult);
    mbedtls_md_free(&ctx);

    for(int i = 0; i < sizeof(shaResult); i++) {
        char str[3];
        sprintf(str, "%02X", (int)shaResult[i]);
        output[i*2] = str[0];
        output[i*2 + 1] = str[1];
    }
    output[64] = '\0';
}

void RfidReader::setupRfidReader() {
    pinMode(RST_PIN, OUTPUT);
    pinMode(CS_PIN, OUTPUT);

    spiDevice = new MFRC522_SPI(CS_PIN, RST_PIN, ArduinoSpi3::SPI3);
    mfrc522 = new MFRC522(spiDevice);
    mfrc522->PCD_Init();
    mfrc522->PCD_DumpVersionToSerial();
    mfrc522->PCD_SetAntennaGain(mfrc522->RxGain_max);
}

void RfidReader::readCard() {
    if (!mfrc522->PICC_IsNewCardPresent() || !mfrc522->PICC_ReadCardSerial()) {
        return;
    }
    Serial.println("[RFID] Reader read complete");

    if (mfrc522->uid.size == 0) {
        Serial.println("[RFID] Bad card (size = 0)");
    } else {

        char tag[sizeof(mfrc522->uid.uidByte) * 4] = { 0 };
        for (int i = 0; i < mfrc522->uid.size; i++) {
            char buff[4]; // 2 hex digits, dash and '\0'
            snprintf(buff, sizeof(buff), "%s%02X", i ? "-" : "", mfrc522->uid.uidByte[i]);
            strncat(tag, buff, sizeof(tag));
        };
        char hashedTag[65];
        hash(tag, hashedTag);
        readHandler(hashedTag);
        stopReading();
    };

    mfrc522->PICC_HaltA();
}

void RfidReader::handleReaderTasks() {
    if (readingStarted) {
        readCard();
    }
}

void RfidReader::stopReading() {
    readingStarted = false;
    readHandler = defaultReadHandler;
    Serial.println("[RFID] Stop reading");
}

void RfidReader::startReading(void (*handler)(const char *)) {
    Serial.println("[RFID] Start reading...");
    readHandler = handler;
    readingStarted = true;
}

void RfidReader::defaultReadHandler(const char *card) { }

bool RfidReader::selfTest() {
    Serial.println("[RFID] Self testing");
    return mfrc522->PCD_PerformSelfTest();
}

void RfidReader::reset() {
    Serial.println("[RFID] Reset start");
    mfrc522->PCD_Reset();
    Serial.println("[RFID] Reset finished");
}

void RfidReader::reInit() {
    Serial.println("[RFID] Reinit start");
    mfrc522->PCD_Init();
    mfrc522->PCD_DumpVersionToSerial();
    mfrc522->PCD_SetAntennaGain(mfrc522->RxGain_max);
    Serial.println("[RFID] Reinit finished");
}

void RfidReader::setGain(int mode) {
    Serial.print("[RFID] Setting gain to 3/");
    Serial.println(mode);
    switch (mode) {
        case 1:
            mfrc522->PCD_SetAntennaGain(mfrc522->RxGain_min);
            return;
        case 2:
            mfrc522->PCD_SetAntennaGain(mfrc522->RxGain_avg);
            return;
        case 3:
            mfrc522->PCD_SetAntennaGain(mfrc522->RxGain_max);
            return;
    }
}
