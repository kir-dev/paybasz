#ifndef PAYBASZ_BOARDPINS_H
#define PAYBASZ_BOARDPINS_H

#include <stdint.h>

/// -----------------------------------------
/// NOTE
/// This file is for ESP32 dev.v1 board
/// The used pin numbers are GPIO pin numbers
/// -----------------------------------------

/// PINS

// SSD1331: 16bit rgb oled display
constexpr uint8_t PIN_DISPLAY_RST = 32;
constexpr uint8_t PIN_DISPLAY_DC = 33;
constexpr uint8_t PIN_DISPLAY_CLK = 14;
constexpr uint8_t PIN_SPI_MOSI = 13;
constexpr uint8_t PIN_SPI_MISO = 12;
constexpr uint8_t PIN_DISPLAY_CS = 15;

// RC522: Rfid reader
constexpr uint8_t PIN_RFID_RST = 25;
constexpr uint8_t PIN_RFID_CS = 17;

// Keypad pins: membran switch
constexpr uint8_t PIN_KEYPAD_COL_1 = 23;
constexpr uint8_t PIN_KEYPAD_COL_2 = 22;
constexpr uint8_t PIN_KEYPAD_COL_3 = 21;
constexpr uint8_t PIN_KEYPAD_COL_4 = 19;
constexpr uint8_t PIN_KEYPAD_ROW_1 = 35;
constexpr uint8_t PIN_KEYPAD_ROW_2 = 26;
constexpr uint8_t PIN_KEYPAD_ROW_3 = 36;
constexpr uint8_t PIN_KEYPAD_ROW_4 = 39;
constexpr uint8_t PIN_KEYPAD_ROW_5 = 34;

// Piezo buzzer
constexpr int32_t PIN_BUZZER = 18;

/// OTHER CONFIG

constexpr int32_t DEBUG_WAIT_MS = 1000;
constexpr int32_t BUZZER_CHANNEL = 1;
constexpr int32_t BUZZER_SOUND_HZ = 2300;

#endif //PAYBASZ_BOARDPINS_H
