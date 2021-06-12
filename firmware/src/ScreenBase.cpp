#include "ScreenBase.h"
#include "Keypad.h"
#include "DisplayManager.h"
#include "RfidReader.h"

ScreenBase * BASE_SCREEN = new ScreenBase();

ScreenBase * ScreenBase::activeScreen = BASE_SCREEN;

extern Keypad * ScreenBase::keypad;
extern DisplayManager * ScreenBase::displayManager;
extern RfidReader * ScreenBase::rfidReader;
extern NetworkHelper * ScreenBase::networkHelper;
extern PermanentMemory * ScreenBase::permanentMemory;

ScreenBase * ScreenBase::getActiveScreen() {
    return activeScreen;
}

bool ScreenBase::operatorOrientation = true;

void ScreenBase::setActiveScreen(ScreenBase * newScreen) {
    if (activeScreen != NULL) {
        activeScreen->onDeactivate();
    }
    activeScreen = newScreen;
    if (activeScreen->getOrientation() != operatorOrientation) {
        operatorOrientation = activeScreen->getOrientation();
        ScreenBase::displayManager->setOrientation(operatorOrientation);
    }
    activeScreen->onActivate();
}

void ScreenBase::setupScreen(Keypad * keypad, DisplayManager * displayManager, RfidReader * rfidReader, NetworkHelper * networkHelper, PermanentMemory * permanentMemory) {
    ScreenBase::keypad = keypad;
    ScreenBase::displayManager = displayManager;
    ScreenBase::rfidReader = rfidReader;
    ScreenBase::networkHelper = networkHelper;
    ScreenBase::permanentMemory = permanentMemory;
}

void ScreenBase::handleKeyEvents(char key) {
    activeScreen->onKeyPressEvent(key);
}

void ScreenBase::onKeyPressEvent(char key) { }
void ScreenBase::onActivate() { }
void ScreenBase::onDeactivate() { }
void ScreenBase::onIdle() { }

bool ScreenBase::getOrientation() {
    return true;
}