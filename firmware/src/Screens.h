#ifndef PAYBASZ_SCREENS_H
#define PAYBASZ_SCREENS_H

#include "ScreenBase.h"

static uint32_t paymentTotalAmount = 0;

class InitScreen : public ScreenBase {
private:
    uint16_t sleepTimeout;
public:
    void onActivate();
    void onKeyPressEvent(char key);
    void onIdle();
};

class MenuScreen : public ScreenBase {
private:
    uint16_t sleepTimeout;
public:
    MenuScreen() : sleepTimeout(0) {};
    void onActivate();
    void onKeyPressEvent(char key);
    void onIdle();
};

class ReadingScreen : public ScreenBase {
public:
    void onActivate();
    void onDeactivate();
    void onKeyPressEvent(char key);
    static void readCardTask(const char * card);
    bool getOrientation();
};

class SleepScreen : public ScreenBase {
public:
    void onActivate();
    void onKeyPressEvent(char key);
};

class PayAddItemScreen : public ScreenBase {
private:
    uint32_t total;
    uint32_t current;
public:
    PayAddItemScreen() : total(0), current(0) {};
    void onActivate();
    void onKeyPressEvent(char key);
};

class PaymentScreen : public ScreenBase {
public:
    void onActivate();
    void onKeyPressEvent(char key);
    bool getOrientation();
    static void paymentTask(const char* card);
};

// FIXME: DEPRECATED
class PaymentLoadingScreen : public ScreenBase {
private:
    int timePassed;
    int state;
public:
    PaymentLoadingScreen() : timePassed(0), state(0) {};
    void onActivate();
    void onIdle();
};

class SuccessScreen : public ScreenBase {
public:
    void onActivate();
    void onKeyPressEvent(char key);
    bool getOrientation();
};

class NoChangeBackToMenuScreen : public ScreenBase {
public:
    void onKeyPressEvent(char key);
};

class ReverseNoChangeBackToMenuScreen : public ScreenBase {
public:
    void onKeyPressEvent(char key);
    bool getOrientation();
};

class CommandScreen : public ScreenBase {
private:
    char command[9];
    uint8_t len;
public:
    void onActivate();
    void onKeyPressEvent(char key);
};

class BalanceScreen : public ScreenBase {
public:
    void onActivate();
    void onDeactivate();
    void onKeyPressEvent(char key);
    static void accountBalanceTask(const char * card);
    bool getOrientation();
};

class BeepScreen : public ScreenBase {
public:
    void onActivate();
    void onDeactivate();
};

static InitScreen * INIT_SCREEN_INSTANCE = new InitScreen();
static MenuScreen * MENU_SCREEN_INSTANCE = new MenuScreen();
static ReadingScreen * READING_SCREEN_INSTANCE = new ReadingScreen();
static SleepScreen * SLEEP_SCREEN_INSTANCE = new SleepScreen();
static PayAddItemScreen * PAY_ADD_ITEM_SCREEN_INSTANCE = new PayAddItemScreen();
static PaymentScreen * PAYMENT_SCREEN_INSTANCE = new PaymentScreen();
static PaymentLoadingScreen * PAYMENT_LOADING_SCREEN_INSTANCE = new PaymentLoadingScreen();
static SuccessScreen * SUCCESS_SCREEN_INSTANCE = new SuccessScreen();
static NoChangeBackToMenuScreen * NO_CHANGE_SCREEN_INSTANCE = new NoChangeBackToMenuScreen();
static ReverseNoChangeBackToMenuScreen * REVERSE_NO_CHANGE_SCREEN_INSTANCE = new ReverseNoChangeBackToMenuScreen();
static CommandScreen * COMMAND_SCREEN_INSTANCE = new CommandScreen();
static BalanceScreen * BALANCE_SCREEN_INSTANCE = new BalanceScreen();
static BeepScreen * BEEP_SCREEN_INSTANCE = new BeepScreen();

#endif //PAYBASZ_SCREENS_H
