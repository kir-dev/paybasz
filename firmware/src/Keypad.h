#ifndef PAYBASZ_KEYPAD_H
#define PAYBASZ_KEYPAD_H

class Keypad {
    static constexpr unsigned long TRIGGER_TIMEOUT = 100;
    static constexpr unsigned char KEYPAD_ROW_COUNT = 5;
    static constexpr unsigned char KEYPAD_COLUMN_COUNT = 4;

    unsigned long lastKeypadTimes[KEYPAD_ROW_COUNT][KEYPAD_COLUMN_COUNT] = {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
    };

    int lastKeypadReads[KEYPAD_ROW_COUNT][KEYPAD_COLUMN_COUNT] = {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
    };

    unsigned char KEYPAD_ROW_PINS[KEYPAD_ROW_COUNT];
    unsigned char KEYPAD_COLUMN_PINS[KEYPAD_COLUMN_COUNT];

    static void onKeyPressedDefault(char key);
    void (*keyPressed)(char);

public:
    static constexpr char KEYPAD_SIGNS[KEYPAD_ROW_COUNT][KEYPAD_COLUMN_COUNT] = {
            {'F','G','#','*'},
            {'1','2','3','U'},
            {'4','5','6','D'},
            {'7','8','9','X'},
            {'L','0','R','E'}
    };

    Keypad(unsigned char col1, unsigned char col2, unsigned char col3, unsigned char col4,
           unsigned char row1, unsigned char row2, unsigned char row3, unsigned char row4, unsigned char row5);
    void setupKeypad();
    void handleKeypad(unsigned long millis);
    void setKeyPressListener(void (*listener)(char));
    void clearKeyPressListener();
};

#endif //PAYBASZ_KEYPAD_H
