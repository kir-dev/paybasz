#ifndef PAYBASZ_DISPLAYMANAGER_H
#define PAYBASZ_DISPLAYMANAGER_H

#include <stdint.h>
#include <lcdgfx.h>
#include "ArduinoSpi3.h"

static const int canvasWidth = 96;
static const int canvasHeight = 64;
static uint8_t canvasData[canvasWidth * canvasHeight * 2];
static NanoCanvas16 canvas(canvasWidth, canvasHeight, canvasData);

class DisplayManager {
    const uint8_t DISPLAY_RST;
    const uint8_t DISPLAY_DC;
    const uint8_t HSPI_CLK;
    const uint8_t HSPI_MOSI;
    const uint8_t HSPI_MISO;
    const uint8_t DISPLAY_CS;
    DisplaySSD1331_96x64x16_CustomSPI<ArduinoSpi3> * display;

public:
    DisplayManager(uint8_t rst, uint8_t dc, uint8_t clk, uint8_t mosi, uint8_t miso, uint8_t cs)
        : DISPLAY_RST(rst), DISPLAY_DC(dc), HSPI_CLK(clk), HSPI_MOSI(mosi), HSPI_MISO(miso), DISPLAY_CS(cs) {};
    void setupDisplay();
    void displayPaymentScreen(int amount);
    void displayAddEntry(int actual, int total);
    void displayLoadingScreen(int state);
    void displayCommandScreen(char * command, int length);
    void displayDoneStatus();
    void displayFailedStatus(String reason);
    void displayReadingScreen();
    void displayReadingScreen(const char* hashedTag);
    void displayReadCardAmountScreen();
    void displayCardAmountScreen(int amount, bool loan, bool allow);
    void displaySplashScreen();
    void displayMenu();
    void displayEmpty();
    void updateSyncFlag();
    void updateErrorFlag(char * message);
    void updateConnFlag();
    void setOrientation(bool orientation);
};

#endif //PAYBASZ_DISPLAYMANAGER_H
