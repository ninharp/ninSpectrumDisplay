/*
  ninSpectrumDisplay.h - Arduino library support for nin Spectrum Display 70 pixel (7x10)

  Copyright (C) 2017 Michael Sauer. All right reserved
  
  This library has been made to make it easy to use the nin Spectrum Display
  as a graphics display or a spectrum analyzer with MSGEQ7 IC on an Arduino.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
*/

#ifndef NINSPECTRUMDISPLAY_H
#define NINSPECTRUMDISPLAY_H

#include "Arduino.h"
#include <avr/pgmspace.h>
#include "Adafruit_NeoPixel.h"

#define MSGEQ7_STROBE_PIN_DEFAULT 11  // strobe pins on digital 4
#define MSGEQ7_RESET_PIN_DEFAULT  10  // reset pins on digital 5
#define MSGEQ7_DATA_PIN_DEFAULT   A1  // data pin on ADC 1

#define MSGEQ7_MAX_BAND    7          // count of bands
#define LEDS_BAND 10                  // LEDs per band

#define DEFAULT_NOISE_FILTER 10

#define fontbyte(x) pgm_read_byte(&currFont.font[x]) 

struct FONT_t
{
  uint8_t* font;
  uint8_t x_size;
  uint8_t y_size;
  uint8_t offset;
  uint8_t numchars;
  //uint8_t inverted;
};

typedef struct ANALYZER_t {
  Adafruit_NeoPixel band;
  uint16_t value;
  uint8_t pin;
} ANALYZER_t;

class ninSpectrumDisplay {
  public:
    ninSpectrumDisplay(uint8_t STROBE, uint8_t RESET, uint8_t VOUT);
    ninSpectrumDisplay(uint8_t STROBE, uint8_t RESET, uint8_t VOUT, uint8_t line1Pin, uint8_t line2Pin, uint8_t line3Pin, uint8_t line4Pin, uint8_t line5Pin, uint8_t line6Pin, uint8_t line7Pin);
    void showSpectrum(void);
    
    void showDisplay(void);
    void clearDisplay(void);
    void showBuffer(void);
    void showBufferColor(void);

    void setColor(uint32_t color);

    void setPixel(uint8_t x, uint8_t y);
    void setColorPixel(uint8_t x, uint8_t y, uint32_t color);
    
    void printLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
    void printLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint32_t color);
    
    void printChar(uint8_t x, uint8_t y, char ch);
    void printChar(uint8_t x, uint8_t y, char ch, uint32_t color);
    void printString(uint8_t x, uint8_t y, uint16_t d, char s[]);
    void printString(uint8_t x, uint8_t y, uint16_t d, char s[], uint32_t color);
    void scrollString(uint8_t y, uint16_t d, char s[]);
    void setFont(uint8_t* font);
    
  protected:
    uint8_t strobePin, resetPin, voutPin;
    ANALYZER_t analyzer[MSGEQ7_MAX_BAND];

    uint8_t currPos, currCharn;
    uint32_t currColor;
    uint32_t offColor;
    FONT_t currFont;

    uint16_t displayBuffer[MSGEQ7_MAX_BAND];
    uint16_t scrollBuffer[MSGEQ7_MAX_BAND];
    uint32_t displayBufferColor[MSGEQ7_MAX_BAND][LEDS_BAND];

    void _initDisplay(uint8_t STROBE, uint8_t RESET, uint8_t VOUT, uint8_t line1Pin, uint8_t line2Pin, uint8_t line3Pin, uint8_t line4Pin, uint8_t line5Pin, uint8_t line6Pin, uint8_t line7Pin);
    void _readMSGEQ7(void);
    void _AllOff(uint8_t band);
    void _shiftBufferLeft(void);
    uint32_t _ColorWheel(byte WheelPos);
};

#endif // NINSPECTRUMDISPLAY_H