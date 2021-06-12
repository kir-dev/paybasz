#ifndef PAYBASZ_SCREENBASE_H
#define PAYBASZ_SCREENBASE_H

#include "Keypad.h"
#include "DisplayManager.h"
#include "RfidReader.h"
#include "NetworkHelper.h"
#include "PermanentMemory.h"

class ScreenBase {
private:
    static ScreenBase * activeScreen;
    static bool operatorOrientation;

public:
    ScreenBase() {};
    virtual void onKeyPressEvent(char key);
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onIdle();
    virtual bool getOrientation();

    static Keypad * keypad;
    static DisplayManager * displayManager;
    static RfidReader * rfidReader;
    static NetworkHelper * networkHelper;
    static PermanentMemory * permanentMemory;

    static ScreenBase * getActiveScreen();
    static void setActiveScreen(ScreenBase * newScreen);
    static void handleKeyEvents(char key);
    static void setupScreen(Keypad * keypad, DisplayManager * displayManager, RfidReader * rfidReader, NetworkHelper * networkHelper, PermanentMemory * permanentMemory);
};


#endif //PAYBASZ_SCREENBASE_H
