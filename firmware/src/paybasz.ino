#define CONFIG_ARDUINO_SPI_ENABLE
#define CONFIG_PLATFORM_SPI_ENABLE

#include <lcdgfx.h>
#include <stdint.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <EEPROM.h>

#include "esp_log.h"
#include "Keypad.h"
#include "DisplayManager.h"
#include "RfidReader.h"
#include "ScreenBase.h"
#include "ArduinoSpi3.h"
#include "Screens.h"
#include "BoardPins.h"
#include "NetworkHelper.h"
#include "PermanentMemory.h"
#include "SetupMode.h"
#include "Firmware.h"

Keypad * keypad;
DisplayManager * displayManager;
RfidReader * rfidReader;
NetworkHelper * networkHelper;
PermanentMemory * permanentMemory;

bool setupMode = false;

void bootBeep() {
    ledcWrite(BUZZER_CHANNEL, 240);
    delay(350);
    ledcWrite(BUZZER_CHANNEL, 0);
    delay(1000);
}

void setupDualBeep() {
    ledcWrite(BUZZER_CHANNEL, 240);
    delay(200);
    ledcWrite(BUZZER_CHANNEL, 0);
    delay(200);
    ledcWrite(BUZZER_CHANNEL, 240);
    delay(200);
    ledcWrite(BUZZER_CHANNEL, 0);
    delay(1000);
}

void enterSetupModeManually(char key) {
    if (key == '*')
        setupMode = true;
}

void setup() {
    Serial.begin(115200);

    // EEPROM
    EEPROM.begin(512);

    // Display
    displayManager = new DisplayManager(
            PIN_DISPLAY_RST,
            PIN_DISPLAY_DC,
            PIN_DISPLAY_CLK,
            PIN_SPI_MOSI,
            PIN_SPI_MISO,
            PIN_DISPLAY_CS);
    displayManager->setupDisplay();
    displayManager->displaySplashScreen(0);

    // Keypad
    keypad = new Keypad(
            PIN_KEYPAD_COL_1,
            PIN_KEYPAD_COL_2,
            PIN_KEYPAD_COL_3,
            PIN_KEYPAD_COL_4,
            PIN_KEYPAD_ROW_1,
            PIN_KEYPAD_ROW_2,
            PIN_KEYPAD_ROW_3,
            PIN_KEYPAD_ROW_4,
            PIN_KEYPAD_ROW_5);
    keypad->setupKeypad();
    keypad->handleKeypad(esp_log_timestamp());
    keypad->setKeyPressListener(enterSetupModeManually);

    // Piezo Buzzer
    ledcSetup(BUZZER_CHANNEL, BUZZER_SOUND_HZ, 8);
    ledcAttachPin(PIN_BUZZER, BUZZER_CHANNEL);

    bootBeep();

    // Serial
    while (!Serial);
    delay(DEBUG_WAIT_MS);
    Serial.println("[SETUP] Starting: paybasz");
    keypad->handleKeypad(esp_log_timestamp());
    delay(1000);
    keypad->handleKeypad(esp_log_timestamp());

    // Permanent memory
    permanentMemory = new PermanentMemory();
    permanentMemory->load();

    if (setupMode || permanentMemory->isSetup()) {
        setupMode = true;
        Serial.println("[SETUP] Entering SETUP configuration mode...");
        setupDualBeep();
        displayManager->displaySetupSplashScreen();
        setupModeSetup();
        return;
    }
    displayManager->displaySplashScreen(1);

    Serial.println("[SETUP] Loaded config:");
    Serial.println("[SETUP] - SSID: " + String(permanentMemory->ssid));
    Serial.println("[SETUP] - NAME: " + String(permanentMemory->gatewayName));
    Serial.println("[SETUP] - TOKEN: " + String(permanentMemory->token));
    Serial.println("[SETUP] - BASE URL: " + String(permanentMemory->baseUrl));

    // Wifi
    networkHelper = new NetworkHelper();
    networkHelper->setupWifi(permanentMemory->ssid, permanentMemory->wifiPassword);
    NetworkHelper::setupUrls(
            permanentMemory->baseUrl,
            permanentMemory->gatewayName,
            permanentMemory->token);

    displayManager->displaySplashScreen(2);

    // Rfid
    rfidReader = new RfidReader(PIN_RFID_RST, PIN_RFID_CS, displayManager);
    rfidReader->setupRfidReader();
    Serial.println("[SETUP] RFID: OK");

    // Screens
    ScreenBase::setupScreen(keypad, displayManager, rfidReader, networkHelper, permanentMemory);
    keypad->setKeyPressListener(ScreenBase::handleKeyEvents);
    ScreenBase::setActiveScreen(INIT_SCREEN_INSTANCE);
    Serial.println("[SETUP] Screens: OK");

    // Other
    Serial.println("[SETUP] Setup completed");

}

void loop() {
    try {
        if (setupMode) {
            setupModeLoop();
            return;
        }

        delay(10);
        unsigned long millis = esp_log_timestamp();
        ScreenBase::getActiveScreen()->onIdle();
        keypad->handleKeypad(millis);
        rfidReader->handleReaderTasks();
    } catch (...) {
        Serial.println("[LOOP] Unhandled exception");
    }
}
