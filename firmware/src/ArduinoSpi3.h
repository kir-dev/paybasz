#ifndef PAYBASZ_ARDUINOSPI3_H
#define PAYBASZ_ARDUINOSPI3_H

#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>

/// ADOPTED FROM SSD1331 LIB DRIVER

#define LCD_LOW LOW
#define LCD_HIGH HIGH
#define lcd_gpioWrite digitalWrite

class ArduinoSpi3 {
public:
    static SPIClass * SPI3;

    ArduinoSpi3(int8_t csPin = -1, int8_t dcPin = -1, int8_t clkPin = -1, int8_t mosiPin = -1, uint32_t freq = 8000000);
    ~ArduinoSpi3();

    void begin();
    void end();
    void start();
    void stop();
    void send(uint8_t data);
    void sendBuffer(const uint8_t *buffer, uint16_t size);

private:
    int8_t m_cs;
    int8_t m_dc;
    int8_t m_clk;
    int8_t m_mosi;
    uint32_t m_frequency;
};


#endif //PAYBASZ_ARDUINOSPI3_H
