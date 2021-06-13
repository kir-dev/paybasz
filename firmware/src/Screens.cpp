#include "Screens.h"
#include "BoardPins.h"

void piezoBeepStart() {
    ledcWrite(BUZZER_CHANNEL, 230);
}

void piezoBeepEnd() {
    delay(300);
    ledcWrite(BUZZER_CHANNEL, 0);
}

void InitScreen::onActivate() {
    sleepTimeout = 0;
}

void InitScreen::onKeyPressEvent(char key) {
    if (key == 'E') {
        ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
    }
}

void InitScreen::onIdle() {
    ++sleepTimeout;
    if (sleepTimeout > 6000) {
        ScreenBase::setActiveScreen(SLEEP_SCREEN_INSTANCE);
    }
}

void MenuScreen::onActivate() {
    ScreenBase::displayManager->displayMenu();
    ScreenBase::displayManager->turnOffLeds();
    sleepTimeout = 0;
}

void MenuScreen::onIdle() {
    ++sleepTimeout;
    if (sleepTimeout > 6000) {
        ScreenBase::setActiveScreen(SLEEP_SCREEN_INSTANCE);
    }
}

void MenuScreen::onKeyPressEvent(char key) {
    switch (key) {
        case 'F': {
            ScreenBase::setActiveScreen(READING_SCREEN_INSTANCE);
            break;
        }
        case 'G': {
            ScreenBase::setActiveScreen(PAY_ADD_ITEM_SCREEN_INSTANCE);
            break;
        }
        case '#': {
            ScreenBase::setActiveScreen(BALANCE_SCREEN_INSTANCE);
            break;
        }
        case '*': {
            ScreenBase::setActiveScreen(COMMAND_SCREEN_INSTANCE);
            break;
        }
        default: break;
    }
}

void ReadingScreen::readCardTask(const char * card) {
    piezoBeepStart();
    piezoBeepEnd();
    ScreenBase::networkHelper->sendReading(card);
    displayManager->displayReadingScreen(card);
}

void ReadingScreen::onActivate() {
    ScreenBase::displayManager->displayReadingScreen();
    ScreenBase::displayManager->setLedColor(255, 255, 255);
    ScreenBase::rfidReader->startReading(readCardTask);
}

void ReadingScreen::onDeactivate() {
    ScreenBase::rfidReader->stopReading();
}

void ReadingScreen::onKeyPressEvent(char key) {
    if (key == 'X' || key == 'E') {
        ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
    } else if (key == 'F' || key == '*') {
        ScreenBase::setActiveScreen(READING_SCREEN_INSTANCE);
    }
}

bool ReadingScreen::getOrientation() {
    return false;
}

void SleepScreen::onActivate() {
    ScreenBase::displayManager->displayEmpty();
}

void SleepScreen::onKeyPressEvent(char key) {
    if (key == 'X' || key == 'E')
        ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
}

void PayAddItemScreen::onActivate() {
    total = 0;
    current = 0;
    cursor = 0;
    for (int i = 0; i < 10; i++)
        item[i] = 0;
    memset(comment, '\0', 255);
    ScreenBase::displayManager->displayAddEntry(current, total);
}

void PayAddItemScreen::onKeyPressEvent(char key) {
    if (cursor == 0) {
        bool changed = false;
        if (current < 99999) {
            switch (key) {
                case '0': current *= 10; current += 0; changed = true; break;
                case '1': current *= 10; current += 1; changed = true; break;
                case '2': current *= 10; current += 2; changed = true; break;
                case '3': current *= 10; current += 3; changed = true; break;
                case '4': current *= 10; current += 4; changed = true; break;
                case '5': current *= 10; current += 5; changed = true; break;
                case '6': current *= 10; current += 6; changed = true; break;
                case '7': current *= 10; current += 7; changed = true; break;
                case '8': current *= 10; current += 8; changed = true; break;
                case '9': current *= 10; current += 9; changed = true; break;
                default: break;
            }
        }
        switch (key) {
            case 'L':
            case 'R': {
                if (current > 0) {
                    current /= 10;
                    changed = true;
                }
                break;
            }
            case 'E': {
                if (total != 0 && current == 0) {
                    paymentTotalAmount = total;
                    ScreenBase::setActiveScreen(PAYMENT_SCREEN_INSTANCE);
                    ScreenBase::displayManager->displayPaymentScreen(total);
                } else {
                    total += current;
                    if (strlen(comment) < 245)
                        sprintf(comment, "%s,%d", comment, current);
                    current = 0;
                    changed = true;
                }
                break;
            }
            case 'X': {
                ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
                break;
            }
            case '#': {
                item[0] = '#';
                cursor = 1;
                current = 0;
                ScreenBase::displayManager->displayAddNamedItem(item, total);
            }
            default:
                break;
        }
        if (changed) {
            if (total > 999999)
                total = 999999;
            ScreenBase::displayManager->displayAddEntry(current, total);
        }
    } else {
        if (key == 'L' || key == 'R') {
            if (cursor > 0) {
                if (current == 0) {
                    cursor--;
                    item[cursor] = '\0';
                }
                current = 0;

                if (cursor == 0) {
                    ScreenBase::displayManager->displayAddEntry(current, total);
                } else {
                    ScreenBase::displayManager->displayAddNamedItem(item, total);
                }
            }

        } else if (key == 'X') {
            ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);

        } else if (key == 'E') {
            if (current == 0) {
                ScreenBase::networkHelper->queryItem(item, &current);
            } else {
                total += current;
                if (strlen(comment) < 245)
                    sprintf(comment, "%s,%s", comment, item);
                current = 0;
                for (int i = 0; i < 10; i++)
                    item[i] = 0;
                cursor = 0;
                ScreenBase::displayManager->displayAddEntry(current, total);
            }
        } else if (cursor < 8 && key != 'G' && key != 'F' && key != 'U' && key != 'D') {
            item[cursor] = key;
            cursor++;
            ScreenBase::displayManager->displayAddNamedItem(item, total);
        }
    }
}

void PaymentScreen::onKeyPressEvent(char key) {
    if (key == 'X')
        ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
}

bool PaymentScreen::getOrientation() {
    return false;
}

void PaymentScreen::paymentTask(const char* card) {
    piezoBeepStart();
    piezoBeepEnd();
    ScreenBase::networkHelper->proceedPayment(card, paymentTotalAmount, comment);
    ScreenBase::setActiveScreen(REVERSE_NO_CHANGE_SCREEN_INSTANCE);
}

void PaymentScreen::onActivate() {
    ScreenBase::displayManager->setLedColor(255, 255, 255);
    ScreenBase::rfidReader->startReading(paymentTask);
}

void PaymentLoadingScreen::onActivate() {
    timePassed = 0;
    state = 0;
}

void PaymentLoadingScreen::onIdle() {
    ++timePassed;
    if (timePassed > 40) {
        ++state;
        state  %= 4;
        timePassed = 0;
        ScreenBase::displayManager->displayLoadingScreen(state);
    }
}

void SuccessScreen::onActivate() {
    ScreenBase::displayManager->displayDoneStatus();
}

void SuccessScreen::onKeyPressEvent(char key) {
    if (key == 'X' || key == 'E') {
        ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
    } else if (key == '*') {
        ScreenBase::setActiveScreen(PAY_ADD_ITEM_SCREEN_INSTANCE);
    }
}

bool SuccessScreen::getOrientation() {
    return false;
}

void NoChangeBackToMenuScreen::onKeyPressEvent(char key) {
    if (key == 'X')
        ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
}

void ReverseNoChangeBackToMenuScreen::onKeyPressEvent(char key) {
    if (key == 'X')
        ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
}

bool ReverseNoChangeBackToMenuScreen::getOrientation() {
    return false;
}

void CommandScreen::onActivate() {
    command[0] = '*';
    for (int i = 1; i < 9; ++i)
        command[i] = '\0';
    len = 1;
    ScreenBase::displayManager->displayCommandScreen(command, len);
}

void testLeds() {
    Serial.println("[TEST] Testing LEDs");
    ScreenBase::displayManager->setLedColor(255, 0, 0);
    delay(1000);
    ScreenBase::displayManager->setLedColor(0, 255, 0);
    delay(1000);
    ScreenBase::displayManager->setLedColor(0, 0, 255);
    delay(1000);
    ScreenBase::displayManager->setLedColor(255, 255, 0);
    delay(1000);
    ScreenBase::displayManager->setLedColor(0, 255, 255);
    delay(1000);
    ScreenBase::displayManager->setLedColor(255, 0, 255);
    delay(1000);
    ScreenBase::displayManager->setLedColor(255, 255, 255);
    delay(1000);
    ScreenBase::displayManager->turnOffLeds();
    delay(500);
    Serial.println("[TEST] Test complete");
    ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
}

void testRainbowLeds() {
    Serial.println("[TEST] Testing LEDs");
    unsigned int rgbColor[3];
    
    rgbColor[0] = 255;
    rgbColor[1] = 0;
    rgbColor[2] = 0;
    
    for (int decColour = 0; decColour < 3; decColour += 1) {
        int incColour = decColour == 2 ? 0 : decColour + 1;
        
        for(int i = 0; i < 255; i += 1) {
            rgbColor[decColour] -= 1;
            rgbColor[incColour] += 1;
            ScreenBase::displayManager->setLedColor(rgbColor[0], rgbColor[1], rgbColor[2]);
            delay(15);
        }
    }
}

void CommandScreen::onKeyPressEvent(char key) {
    bool changed = false;

    if (key == 'X') {
        ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
        return;
    } else if (key == 'E') {
        /// Open menus from command
        if (strcmp(command, "*100#") == 0) {
            ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
        } else if (strcmp(command, "*101#") == 0) {
            ScreenBase::displayManager->displaySplashScreen(2);
            ScreenBase::setActiveScreen(INIT_SCREEN_INSTANCE);
        } else if (strcmp(command, "*102#") == 0) {
            ScreenBase::setActiveScreen(BALANCE_SCREEN_INSTANCE);
        } else if (strcmp(command, "*103#") == 0) {
            ScreenBase::setActiveScreen(READING_SCREEN_INSTANCE);
        } else if (strcmp(command, "*104#") == 0) {
            ScreenBase::setActiveScreen(PAY_ADD_ITEM_SCREEN_INSTANCE);

        /// Meant to be locking
        /// But please don't leave it alone with malicious people
        } else if (strcmp(command, "*400#") == 0) {

        /// TURN OF DISPLAY
        } else if (strcmp(command, "*0#") == 0) {
            ScreenBase::setActiveScreen(SLEEP_SCREEN_INSTANCE);

        /// Dump firmware info
        } else if (strcmp(command, "*7000#") == 0) {
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[FIRMWARE]", "DETAILS", buildVersion, buildDate, ScreenBase::permanentMemory->gatewayName);
            ScreenBase::setActiveScreen(NO_CHANGE_SCREEN_INSTANCE);

        /// Check server status
        } else if (strcmp(command, "*7001#") == 0) {
            ScreenBase::networkHelper->checkStatus();
            ScreenBase::setActiveScreen(NO_CHANGE_SCREEN_INSTANCE);

        /// Check connection and authentication
        } else if (strcmp(command, "*7002#") == 0) {
            ScreenBase::networkHelper->validateConnection();
            ScreenBase::setActiveScreen(NO_CHANGE_SCREEN_INSTANCE);

        /// Pay 0 JMF: Payment test
        } else if (strcmp(command, "*7003#") == 0) {
            paymentTotalAmount = 0;
            comment[0] = '0';
            comment[1] = '\0';
            ScreenBase::setActiveScreen(PAYMENT_SCREEN_INSTANCE);
            ScreenBase::displayManager->displayPaymentScreen(0);

        /// LED color test
        } else if (strcmp(command, "*7004#") == 0) {
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[LED]", "Color fade");
            testLeds();
            ScreenBase::setActiveScreen(NO_CHANGE_SCREEN_INSTANCE);

        /// PWM LED test
        } else if (strcmp(command, "*7005#") == 0) {
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[LED]", "Spectrum fade");
            testRainbowLeds();
            testRainbowLeds();
            testRainbowLeds();
            ScreenBase::displayManager->turnOffLeds();
            Serial.println("[TEST] Test complete");
            ScreenBase::setActiveScreen(NO_CHANGE_SCREEN_INSTANCE);

        /// Buzzer self test
        } else if (strcmp(command, "*7006#") == 0) {
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[Buzzer]", "Self test");
            ScreenBase::setActiveScreen(BEEP_SCREEN_INSTANCE);

        /// RFID self test
        } else if (strcmp(command, "*7007#") == 0) {
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[RFID]", "Self test");
            if (ScreenBase::rfidReader->selfTest()) {
                Serial.println("[RFID] Self testing: Passed");
                ScreenBase::displayManager->showDebugInfo(DisplayManager::FINE, "[RFID]", "Self test", "Passed");
            } else {
                Serial.println("[RFID] Self testing: Failed");
                ScreenBase::displayManager->showDebugInfo(DisplayManager::ERROR, "[RFID]", "Self test", "Failed");
            }
            ScreenBase::setActiveScreen(NO_CHANGE_SCREEN_INSTANCE);

        /// Reset RFID
        } else if (strcmp(command, "*8001#") == 0) {
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[RFID]", "RESET: ...");
            ScreenBase::rfidReader->reset();
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[RFID]", "RESET: DONE");
            ScreenBase::setActiveScreen(NO_CHANGE_SCREEN_INSTANCE);

        /// Re-init RFID
        } else if (strcmp(command, "*8002#") == 0) {
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[RFID]", "RE-INIT: ...");
            ScreenBase::rfidReader->reInit();
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[RFID]", "RE-INIT: DONE");
            ScreenBase::setActiveScreen(NO_CHANGE_SCREEN_INSTANCE);

        /// RFID set gain [min]
        } else if (strcmp(command, "*9101#") == 0) {
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[RFID]", "SET GAIN TO:", "MIN (18 dB)");
            ScreenBase::rfidReader->setGain(1);
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[RFID]", "SET GAIN TO:", "MIN (18 dB)", "DONE");
            ScreenBase::setActiveScreen(NO_CHANGE_SCREEN_INSTANCE);

        /// RFID set gain [avg]
        } else if (strcmp(command, "*9102#") == 0) {
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[RFID]", "SET GAIN TO:", "AVG (33 dB)");
            ScreenBase::rfidReader->setGain(2);
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[RFID]", "SET GAIN TO:", "AVG (33 dB)", "DONE");
            ScreenBase::setActiveScreen(NO_CHANGE_SCREEN_INSTANCE);

        /// RFID set gain [max:default]
        } else if (strcmp(command, "*9103#") == 0) {
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[RFID]", "SET GAIN TO:", "MAX (48 dB)");
            ScreenBase::rfidReader->setGain(3);
            ScreenBase::displayManager->showDebugInfo(DisplayManager::INFO, "[RFID]", "SET GAIN TO:", "AVG (48 dB)", "DONE");
            ScreenBase::setActiveScreen(NO_CHANGE_SCREEN_INSTANCE);

        /// Trigger setup mode
        } else if (strcmp(command, "*707172#") == 0) {
            ScreenBase::permanentMemory->setSetup(true);
            ScreenBase::permanentMemory->save();
            ScreenBase::setActiveScreen(REBOOT_NOW_SCREEN);

        /// Factory reset
        } else if (strcmp(command, "*909192#") == 0) {
            ScreenBase::permanentMemory->factoryReset();
            ScreenBase::setActiveScreen(REBOOT_NOW_SCREEN);

        } else if (strcmp(command, "*69#") == 0) {
            // Easter egg
        } else if (strcmp(command, "*911#") == 0) {
            // Easter egg
        } else if (strcmp(command, "*420#") == 0) {
            // Easter egg
        } else {
            command[1] = '\0';
            len = 1;
            changed = true;
        }
        return;
    } else if (key == 'R' && len >= 2) {
        command[len - 1] = '\0';
        --len;
        changed = true;
    }

    if (len < 8) {
        switch (key) {
            case '0': command[len] = '0'; ++len; changed = true; break;
            case '1': command[len] = '1'; ++len; changed = true; break;
            case '2': command[len] = '2'; ++len; changed = true; break;
            case '3': command[len] = '3'; ++len; changed = true; break;
            case '4': command[len] = '4'; ++len; changed = true; break;
            case '5': command[len] = '5'; ++len; changed = true; break;
            case '6': command[len] = '6'; ++len; changed = true; break;
            case '7': command[len] = '7'; ++len; changed = true; break;
            case '8': command[len] = '8'; ++len; changed = true; break;
            case '9': command[len] = '9'; ++len; changed = true; break;
            case '*': command[len] = '*'; ++len; changed = true; break;
            case '#': command[len] = '#'; ++len; changed = true; break;
            default: break;
        }
    }
    if (changed)
        ScreenBase::displayManager->displayCommandScreen(command, len);
}

void BalanceScreen::accountBalanceTask(const char * card) {
    piezoBeepStart();
    piezoBeepEnd();
    AccountBalance balance = ScreenBase::networkHelper->getAccountBalance(card);
    displayManager->displayCardAmountScreen(balance.balance, balance.loan, balance.allowed);
}

void BalanceScreen::onActivate() {
    ScreenBase::displayManager->displayReadingScreen();
    ScreenBase::displayManager->setLedColor(255, 255, 255);
    ScreenBase::rfidReader->startReading(accountBalanceTask);
}

void BalanceScreen::onDeactivate() {
    ScreenBase::rfidReader->stopReading();
}

void BalanceScreen::onKeyPressEvent(char key) {
    if (key == 'X' || key == 'E') {
        ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
    } else if (key == '*') {
        ScreenBase::setActiveScreen(BALANCE_SCREEN_INSTANCE);
    }
}

bool BalanceScreen::getOrientation() {
    return false;
}

void BeepScreen::onActivate() {
    for (int i = 100; i < 450; i += 50) {
        ledcWrite(BUZZER_CHANNEL, i);
        delay(350);
        ledcWrite(BUZZER_CHANNEL, 0);
        delay(1000);
    }

    ScreenBase::setActiveScreen(MENU_SCREEN_INSTANCE);
}

void BeepScreen::onDeactivate() {
}


void RebootNowScreen::onActivate() {
    ScreenBase::displayManager->displayRebootNowScreen();
}
