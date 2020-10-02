#define CONFIG_ARDUINO_SPI_ENABLE
#define CONFIG_PLATFORM_SPI_ENABLE

#include <lcdgfx.h>
#include <stdint.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#include "esp_log.h"
#include "Keypad.h"
#include "DisplayManager.h"
#include "RfidReader.h"
#include "ScreenBase.h"
#include "ArduinoSpi3.h"
#include "Screens.h"
#include "BoardPins.h"
#include "NetworkHelper.h"
#include "Firmare.gen.h" // FIXME: generate

Keypad * keypad;
DisplayManager * displayManager;
RfidReader * rfidReader;
NetworkHelper * networkHelper;

void setup() {
    Serial.begin(9600);
    // Display
    displayManager = new DisplayManager(
            PIN_DISPLAY_RST,
            PIN_DISPLAY_DC,
            PIN_DISPLAY_CLK,
            PIN_SPI_MOSI,
            PIN_SPI_MISO,
            PIN_DISPLAY_CS);
    displayManager->setupDisplay();
    displayManager->displaySplashScreen();

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

    // Serial
    while (!Serial);
    delay(DEBUG_WAIT_MS);
    Serial.println("[SETUP] Starting: paybasz");
    Serial.println("[SETUP] Display: GO"); // FIXME: calc true GO. NO-GO
    Serial.println("[SETUP] Keypad: GO");

    // Wifi
    networkHelper = new NetworkHelper();
    networkHelper->setupWifi();
    NetworkHelper::setupUrls(TEST_BASE_URL, "proto1");

    // Rfid
    rfidReader = new RfidReader(PIN_RFID_RST, PIN_RFID_CS, displayManager);
    rfidReader->setupRfidReader();
    Serial.println("[SETUP] RFID: GO");

    // Screens
    ScreenBase::setupScreen(keypad, displayManager, rfidReader, networkHelper);
    keypad->setKeyPressListener(ScreenBase::handleKeyEvents);
    ScreenBase::setActiveScreen(INIT_SCREEN_INSTANCE);
    Serial.println("[SETUP] Screens: GO");

    // Piezo Buzzer
    ledcSetup(BUZZER_CHANNEL, BUZZER_SOUND_HZ, 8);
    ledcAttachPin(PIN_BUZZER, BUZZER_CHANNEL);

    // Other
    Serial.println("[SETUP] Setup completed");

}

void loop() {
    try {
        delay(10);
        unsigned long millis = esp_log_timestamp();
        ScreenBase::getActiveScreen()->onIdle();
        keypad->handleKeypad(millis);
        rfidReader->handleReaderTasks();
    } catch (...) {
        Serial.println("[LOOP] Unhandled exception");
    }
}
