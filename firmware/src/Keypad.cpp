#include <Arduino.h>
#include "Keypad.h"
#include "Firmware.h"

Keypad::Keypad(unsigned char col1, unsigned char col2, unsigned char col3, unsigned char col4,
                unsigned char row1, unsigned char row2, unsigned char row3, unsigned char row4, unsigned char row5
       ) {
    KEYPAD_COLUMN_PINS[0] = col1;
    KEYPAD_COLUMN_PINS[1] = col2;
    KEYPAD_COLUMN_PINS[2] = col3;
    KEYPAD_COLUMN_PINS[3] = col4;
    KEYPAD_ROW_PINS[0] = row1;
    KEYPAD_ROW_PINS[1] = row2;
    KEYPAD_ROW_PINS[2] = row3;
    KEYPAD_ROW_PINS[3] = row4;
    KEYPAD_ROW_PINS[4] = row5;
    keyPressed = onKeyPressedDefault;
}

void Keypad::onKeyPressedDefault(char key) {}

extern constexpr const char Keypad::KEYPAD_SIGNS [KEYPAD_ROW_COUNT][KEYPAD_COLUMN_COUNT];

void Keypad::setKeyPressListener(void (*listener)(char)) {
    keyPressed = listener;
}

void Keypad::setupKeypad() {
    for (unsigned char i = 0; i < KEYPAD_COLUMN_COUNT; i++)
        pinMode(KEYPAD_COLUMN_PINS[i], OUTPUT);

    for (unsigned char i = 0; i < KEYPAD_ROW_COUNT; i++)
        pinMode(KEYPAD_ROW_PINS[i], INPUT);

    for (unsigned char c = 0; c < KEYPAD_COLUMN_COUNT; c++)
        digitalWrite(KEYPAD_COLUMN_PINS[c], HIGH);
}

void Keypad::handleKeypad(unsigned long millis) {
    for (unsigned char c = 0; c < KEYPAD_COLUMN_COUNT; c++) {
        digitalWrite(KEYPAD_COLUMN_PINS[c], LOW);
        for (unsigned char r = 0; r < KEYPAD_ROW_COUNT; r++) {
            int read = digitalRead(KEYPAD_ROW_PINS[r]);
            if (read != lastKeypadReads[r][c] && (millis - TRIGGER_TIMEOUT) > lastKeypadTimes[r][c]) {
                lastKeypadReads[r][c] = read;
                lastKeypadTimes[r][c] = millis;

                if (read == LOW) {
#ifdef DEBUG_KEYBOARD
                    Serial.printf("BUTTON D %c\r\n", Keypad::KEYPAD_SIGNS[r][c]);
#endif
                    keyPressed(Keypad::KEYPAD_SIGNS[r][c]);
                } else {
#ifdef DEBUG_KEYBOARD
                    Serial.printf("BUTTON U %c\r\n", Keypad::KEYPAD_SIGNS[r][c]);
#endif
                }
            }
        }
        digitalWrite(KEYPAD_COLUMN_PINS[c], HIGH);
    }
}