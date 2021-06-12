#include "DisplayManager.h"
#include <lcdgfx.h>
#include <SPI.h>
#include <stdint.h>
#include "ArduinoSpi3.h"
#include "Branding.h"
#include "Firmware.h"

constexpr int DISPLAY_WIDTH = 96;

void DisplayManager::setupDisplay() {
    pinMode(DISPLAY_RST, OUTPUT);
    pinMode(DISPLAY_CS, OUTPUT);
    display = new DisplaySSD1331_96x64x16_CustomSPI<ArduinoSpi3>(DISPLAY_RST, DISPLAY_DC, DISPLAY_CS, DISPLAY_DC, HSPI_CLK, HSPI_MOSI, 8000000);
    display->begin();
    display->clear();
    lcd_delay(500);
}

void DisplayManager::displayPaymentScreen(int amount) {
    canvas.clear();
    canvas.setFixedFont(courier_new_font11x16_digits);
    canvas.setColor(RGB_COLOR16(255, 255, 0));
    const String amountStr = String(amount);
    const int amountLength = amountStr.length();
    int start = (DISPLAY_WIDTH - (amountLength * 12)) / 2 - (amountLength > 3 ? 3 : 0) + 3;
    for (int i = amountLength - 1; i >= 0; i--) {
        const char digit[2] = {amountStr[amountLength - i - 1], '\0'};
        canvas.printFixed(start, 21, digit, STYLE_NORMAL);

        start += 12;
        if (i == 3)
            start += 6;
    }

    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(21, 4, "FIZETENDO", STYLE_NORMAL);
    canvas.putPixel(71, 2);
    canvas.putPixel(72, 1);
    canvas.putPixel(73, 2);
    canvas.putPixel(74, 1);

    canvas.printFixed(27, 55, "pay", STYLE_NORMAL);
    canvas.setColor(RGB_COLOR16(0, 255, 0));
    canvas.printFixed(45, 57, "basz", STYLE_NORMAL);

    canvas.setColor(RGB_COLOR16(173, 255, 47));
    canvas.fillRect(0, 16, 1, 49);
    canvas.fillRect(94, 16, 95, 49);
    canvas.fillRect(0, 16, 95, 17);
    canvas.fillRect(0, 49, 95, 50);

    canvas.setColor(RGB_COLOR16(173, 255, 47));
    canvas.printFixed(27, 38, "[ JMF ]", STYLE_NORMAL);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displayAddEntry(int actual, int total) {
    canvas.clear();
    canvas.setFixedFont(courier_new_font11x16_digits);
    canvas.setColor(RGB_COLOR16(0, 255, 255));
    const String amountStr = String(actual);
    const int amountLength = amountStr.length();
    int start = (DISPLAY_WIDTH - (amountLength * 12)) / 2 - (amountLength > 3 ? 3 : 0) + 3;
    for (int i = amountLength - 1; i >= 0; i--) {
        const char digit[2] = {amountStr[amountLength - i - 1], '\0'};
        canvas.printFixed(start, 21, digit, STYLE_NORMAL);
        start += 12;
        if (i == 3)
            start += 6;
    }

    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(27, 4, "FIZETES", STYLE_NORMAL);
    canvas.putPixel(60, 2);
    canvas.putPixel(61, 1);
    canvas.printFixed(54, 55, "[2xENT]", STYLE_NORMAL);

    canvas.setColor(RGB_COLOR16(0, 0, 255));
    canvas.printFixed(2, 55, "=", STYLE_NORMAL);
    canvas.printFixed(9, 55, String(total).c_str(), STYLE_NORMAL);

    canvas.setColor(RGB_COLOR16(30, 144, 255));
    canvas.printFixed(2, 18, "+", STYLE_NORMAL);
    canvas.fillRect(0, 16, 1, 49);
    canvas.fillRect(94, 16, 95, 49);
    canvas.fillRect(0, 16, 95, 17);
    canvas.fillRect(0, 49, 95, 50);

    canvas.printFixed(27, 38, "[ JMF ]", STYLE_NORMAL);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displayLoadingScreen(int state) {
    canvas.setColor(RGB_COLOR16(0, 0, 0));
    canvas.fillRect(2, 2, 93, 52);
    if (state == 0) {
        canvas.clear();
        canvas.setColor(RGB_COLOR16(255, 255, 0));
        canvas.fillRect(32, 28, 40, 36);
    } else {
        canvas.setColor(RGB_COLOR16(255, 255, 0));
        canvas.fillRect(34, 30, 38, 34);
    }

    if (state == 1 || state == 3) {
        canvas.setColor(RGB_COLOR16(255, 255, 0));
        canvas.fillRect(44, 28, 52, 36);
    } else {
        canvas.setColor(RGB_COLOR16(255, 255, 0));
        canvas.fillRect(46, 30, 50, 34);
    }

    if (state == 2) {
        canvas.setColor(RGB_COLOR16(255, 255, 0));
        canvas.fillRect(56, 28, 64, 36);
    } else {
        canvas.setColor(RGB_COLOR16(255, 255, 0));
        canvas.fillRect(58, 30, 62, 34);
    }
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displayCommandScreen(char * command, int length) {
    canvas.clear();
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(27, 4, "PARANCS", STYLE_NORMAL);

    canvas.printFixed(66, 55, "[ENT]", STYLE_NORMAL);
    canvas.setColor(RGB_COLOR16(128, 128, 128));
    canvas.printFixed(2, 55, "[ESC]", STYLE_NORMAL);

    canvas.setFixedFont(courier_new_font11x16_digits);
    if (length <= 5) {
        canvas.setColor(RGB_COLOR16(0, 255, 0));
    } else if (length <= 7) {
        canvas.setColor(RGB_COLOR16(0, 255, 255));
    } else {
        canvas.setColor(RGB_COLOR16(255, 0, 0));
    }
    canvas.printFixed((DISPLAY_WIDTH-(length*11)) / 2, 24, command, STYLE_NORMAL);

    canvas.fillRect(10, 42, 85, 44);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displayDoneStatus() {
    canvas.clear();
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(RGB_COLOR16(0, 255, 0));
    canvas.drawLine(28, 28, 42, 42);
    canvas.drawLine(28, 27, 42, 41);
    canvas.drawLine(28, 29, 42, 43);
    canvas.drawLine(42, 42, 76, 8);
    canvas.drawLine(42, 41, 76, 7);
    canvas.drawLine(42, 43, 76, 9);
    canvas.drawRect(0, 0, 95, 63);
    canvas.drawRect(1, 1, 94, 62);

    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(27, 54, "SIKERES", STYLE_NORMAL);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displayFailedStatus(String reason) {
    canvas.clear();
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(RGB_COLOR16(255, 0, 0));
    canvas.drawLine(32, 16, 64, 48);
    canvas.drawLine(32, 15, 64, 47);
    canvas.drawLine(32, 17, 64, 49);
    canvas.drawLine(32, 48, 64, 16);
    canvas.drawLine(32, 47, 64, 15);
    canvas.drawLine(32, 49, 64, 17);
    canvas.drawRect(0, 0, 95, 63);
    canvas.drawRect(1, 1, 94, 62);

    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(18, 54, "SIKERTELEN", STYLE_NORMAL);
    canvas.setColor(RGB_COLOR16(128, 0, 0));
    canvas.printFixed((DISPLAY_WIDTH - (reason.length() * 6)) / 2, 3, reason.c_str(), STYLE_NORMAL);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displayReadingScreen() {
    canvas.clear();
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(RGB_COLOR16(173, 255, 47));
    canvas.printFixed(30, 21, "KARTYA", STYLE_NORMAL);
    canvas.putPixel(39, 19);
    canvas.putPixel(40, 18);
    canvas.printFixed(21, 32, "LEOLVASAS", STYLE_NORMAL);
    canvas.putPixel(66, 30);
    canvas.putPixel(67, 29);

    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(27, 52, "pay", STYLE_NORMAL);
    canvas.setColor(RGB_COLOR16(0, 255, 0));
    canvas.printFixed(45, 53, "basz", STYLE_NORMAL);

    canvas.setColor(RGB_COLOR16(255, 215, 0));
    canvas.fillRect(0, 0, 1, 63);
    canvas.fillRect(94, 0, 95, 63);
    canvas.fillRect(0, 0, 95, 1);
    canvas.fillRect(0, 62, 95, 63);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displayReadingScreen(const char* hashedTag) {
    canvas.clear();
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(RGB_COLOR16(173, 255, 47));
    canvas.printFixed(30, 21, "KARTYA", STYLE_NORMAL);
    canvas.putPixel(39, 19);
    canvas.putPixel(40, 18);

    char tag[12];
    tag[0] = hashedTag[0];
    tag[1] = hashedTag[1];
    tag[2] = hashedTag[2];
    tag[3] = '-';
    tag[4] = hashedTag[3];
    tag[5] = hashedTag[4];
    tag[6] = hashedTag[5];
    tag[7] = '-';
    tag[8] = hashedTag[6];
    tag[9] = hashedTag[7];
    tag[10] = hashedTag[8];
    tag[11] = '\0';

    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(15, 32, tag, STYLE_NORMAL);

    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(27, 52, "pay", STYLE_NORMAL);
    canvas.setColor(RGB_COLOR16(0, 255, 0));
    canvas.printFixed(45, 53, "basz", STYLE_NORMAL);

    canvas.setColor(RGB_COLOR16(255, 215, 0));
    canvas.fillRect(0, 0, 1, 63);
    canvas.fillRect(94, 0, 95, 63);
    canvas.fillRect(0, 0, 95, 1);
    canvas.fillRect(0, 62, 95, 63);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displayReadCardAmountScreen() {
    canvas.clear();
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(RGB_COLOR16(0, 191, 255));
    canvas.printFixed(24, 21, "EGYENLEG", STYLE_NORMAL);
    canvas.printFixed(21, 32, "LEOLVASAS", STYLE_NORMAL);
    canvas.putPixel(66, 30);
    canvas.putPixel(67, 29);

    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(27, 52, "pay", STYLE_NORMAL);
    canvas.setColor(RGB_COLOR16(0, 255, 0));
    canvas.printFixed(45, 53, "basz", STYLE_NORMAL);

    canvas.setColor(RGB_COLOR16(0, 191, 255));
    canvas.fillRect(0, 0, 1, 63);
    canvas.fillRect(94, 0, 95, 63);
    canvas.fillRect(0, 0, 95, 1);
    canvas.fillRect(0, 62, 95, 63);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displayCardAmountScreen(int amount, bool loan, bool allow) {
    canvas.clear();
    canvas.setFixedFont(courier_new_font11x16_digits);
    canvas.setColor(RGB_COLOR16(255, 255, 255));
    const String amountStr = String(amount);
    const int amountLength = amountStr.length();
    int start = (DISPLAY_WIDTH - (amountLength * 12)) / 2 - (amountLength > 3 ? 3 : 0) + 3;
    for (int i = amountLength - 1; i >= 0; i--) {
        const char digit[2] = {amountStr[amountLength - i - 1], '\0'};
        canvas.printFixed(start, 21, digit, STYLE_NORMAL);

        start += 12;
        if (i == 3 && (amount < -999 || amount > -100))
            start += 6;
    }

    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(24, 4, "EGYENLEG", STYLE_NORMAL);

    if (!allow) {
        canvas.setColor(RGB_COLOR16(255, 0, 0));
        canvas.printFixed(21, 56, "LETILTVA!", STYLE_NORMAL);
    } else if (loan) {
        canvas.setColor(RGB_COLOR16(255, 255, 0));
        canvas.printFixed(12, 56, "HITEL: LEHET", STYLE_NORMAL);
    } else {
        canvas.setColor(RGB_COLOR16(0, 191, 255));
    }

    canvas.fillRect(0, 16, 1, 49);
    canvas.fillRect(94, 16, 95, 49);
    canvas.fillRect(0, 16, 95, 17);
    canvas.fillRect(0, 49, 95, 50);
    canvas.printFixed(27, 38, "[ JMF ]", STYLE_NORMAL);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displaySplashScreen() {
    canvas.clear();
    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.drawBitmap1(0, 0, 96, 64, bannerWhiteParts);
    canvas.setColor(RGB_COLOR16(0, 255, 0));
    canvas.drawBitmap1(36, 36, 56, 24, bannerGreenParts);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displaySetupSplashScreen() {
    canvas.clear();
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(RGB_COLOR16(255, 255, 0));
    canvas.printFixed(27, 17, "[SETUP]", STYLE_NORMAL);
    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(4, 25, setupSsid, STYLE_NORMAL);
    canvas.printFixed(4, 34, setupPassword, STYLE_NORMAL);
    canvas.setColor(RGB_COLOR16(128, 128, 0));
    canvas.printFixed(4, 43, "192.168.0.1", STYLE_NORMAL);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displayMenu() {
    canvas.clear();
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(RGB_COLOR16(0, 0, 255));
    canvas.drawRect(0, 0, 95, 63);
    canvas.drawRect(1, 1, 94, 62);

    canvas.setColor(RGB_COLOR16(255, 255, 255));
    canvas.printFixed(36, 5, "MENU", STYLE_NORMAL);
    canvas.putPixel(56, 3);
    canvas.putPixel(58, 3);

    canvas.setColor(RGB_COLOR16(0, 128, 255));
    canvas.printFixed(4, 17, "F1   Beolvas", STYLE_NORMAL);
    canvas.printFixed(4, 25, "F2   Fizetes", STYLE_NORMAL);
    canvas.printFixed(4, 33, "#    Egyenleg", STYLE_NORMAL);
    canvas.printFixed(4, 41, "*    Parancs", STYLE_NORMAL);
    canvas.printFixed(4, 49, "*0#  Alvo", STYLE_NORMAL);
    display->drawCanvas(0, 0, canvas);
}

void DisplayManager::displayEmpty() {
    display->clear();
}

void DisplayManager::displayRebootNowScreen() {
        canvas.clear();
        canvas.setFixedFont(ssd1306xled_font6x8);
        canvas.setColor(RGB_COLOR16(255, 0, 0));
        canvas.drawRect(0, 0, 95, 63);
        canvas.drawRect(1, 1, 94, 62);

        canvas.setColor(RGB_COLOR16(255, 255, 255));
        canvas.printFixed(18, 28, "REBOOT NOW", STYLE_NORMAL);
        display->drawCanvas(0, 0, canvas);
};

void DisplayManager::updateSyncFlag() {
    display->setColor(RGB_COLOR16(0, 128, 0));
    display->printFixed(68, 4, "SYNC", STYLE_NORMAL);
}

void DisplayManager::updateConnFlag() {
    display->setColor(RGB_COLOR16(128, 128, 0));
    display->printFixed(68, 4, "CONN", STYLE_NORMAL);
}

void DisplayManager::updateErrorFlag(char * message) {
    display->setColor(RGB_COLOR16(128, 0, 0));
    display->printFixed(68, 4, message, STYLE_NORMAL);
    display->printFixed(68, 4, message, STYLE_NORMAL);
}

void DisplayManager::setOrientation(bool orientation) {
    display->getInterface().setRotation(orientation ? 0 : 2);
}
