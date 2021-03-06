/*
  ninSpectrumDisplay.cpp - Arduino library support for nin Spectrum Display 70 pixel (7x10)

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


#include "Arduino.h"
#include "ninSpectrumDisplay.h"
#include "Adafruit_NeoPixel.h"

ninSpectrumDisplay::ninSpectrumDisplay(uint8_t STROBE, uint8_t RESET, uint8_t VOUT)
{ 
  _initDisplay(STROBE, RESET, VOUT, 2, 3, 4, 5, 6, 7, 8); 
}

ninSpectrumDisplay::ninSpectrumDisplay(uint8_t STROBE, uint8_t RESET, uint8_t VOUT, uint8_t line1Pin, uint8_t line2Pin, uint8_t line3Pin, uint8_t line4Pin, uint8_t line5Pin, uint8_t line6Pin, uint8_t line7Pin)
{
  _initDisplay(STROBE, RESET, VOUT, line1Pin, line2Pin, line3Pin, line4Pin, line5Pin, line6Pin, line7Pin); 
}

void ninSpectrumDisplay::setFont(uint8_t* font)
{
  currFont.font = font;
  currFont.x_size = fontbyte(0);
  currFont.y_size = fontbyte(1);
  currFont.offset = fontbyte(2);
  currFont.numchars = fontbyte(3);
  //currFont.inverted = 0;
}

void ninSpectrumDisplay::setPixel(uint8_t x, uint8_t y)
{
  uint16_t mask = 1;
  mask <<= y-1;
  displayBuffer[x-1] |= mask;
}

void ninSpectrumDisplay::setColorPixel(uint8_t x, uint8_t y, uint32_t color)
{
  displayBufferColor[x-1][y-1] = color;
}

void ninSpectrumDisplay::printLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2; /* error value e_xy */

  while(1) {
    setPixel(x0, y0);
    if (x0==x1 && y0==y1) break;
    e2 = 2*err;
    if (e2 > dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
    if (e2 < dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
  }
  showBuffer();
}

void ninSpectrumDisplay::printLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint32_t color)
{
  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2; /* error value e_xy */

  while(1) {
    setColorPixel(x0, y0, color);
    if (x0==x1 && y0==y1) break;
    e2 = 2*err;
    if (e2 > dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
    if (e2 < dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
  }
  showBufferColor();
}

void ninSpectrumDisplay::printChar(uint8_t x, uint8_t y, char ch)
{
  ninSpectrumDisplay::printChar(x, y, ch, currColor);
}

void ninSpectrumDisplay::printChar(uint8_t x, uint8_t y, char ch, uint32_t color)
{
  ch = ch - currFont.offset;
  for (uint8_t i = 0; i < 6; i++) {
    byte b = pgm_read_byte(&currFont.font[(ch*currFont.x_size)+i+4]);
    byte pos = 0;
    for (uint8_t j = 128; j > 0; j >>= 1) {
      if (b & j) {
        analyzer[i+x-1].band.setPixelColor(((pos++)+y), color);
      } else {
        analyzer[i+x-1].band.setPixelColor(((pos++)+y), 0x000000);
      }
    }
    analyzer[i+x-1].band.show();
  }
}

void ninSpectrumDisplay::printString(uint8_t x, uint8_t y, uint16_t d, char s[])
{
  printString(x, y, d, s, currColor);
}

void ninSpectrumDisplay::printString(uint8_t x, uint8_t y, uint16_t d, char s[], uint32_t color)
{
  for (uint8_t i = 0; i < strlen(s); i++) {
    printChar(x, y, s[i]);
    delay(d);  
  }
  
}

void ninSpectrumDisplay::scrollString(uint8_t y, uint16_t d, char s[])
{
  for (uint8_t idx = 0; idx < strlen(s); idx++) {
    char ch = s[idx] - currFont.offset;
    char ch_next = 0;
    if (idx < strlen(s))
      ch_next = s[idx+1] - currFont.offset; 
   
    for (uint8_t i = 0; i < currFont.x_size; i++) {
      displayBuffer[i] = pgm_read_byte(&currFont.font[(ch*currFont.x_size)+i+4]);
      displayBuffer[i] <<= y;
      if (ch_next > 0) {
        scrollBuffer[i] = pgm_read_byte(&currFont.font[(ch_next*currFont.x_size)+i+4]);
        scrollBuffer[i] <<= y;
      }
    }
    for (uint8_t c = 0; c <= currFont.x_size; c++) {
      showBuffer();
      delay(d);
      _shiftBufferLeft();
    }
  }
}

void ninSpectrumDisplay::showBuffer(void)
{
  for (uint8_t i = 0; i < MSGEQ7_MAX_BAND; i++) {
    uint8_t pos = 0;
    for (uint16_t j = 512; j > 0; j >>= 1)
        if (displayBuffer[i] & j)
          analyzer[i].band.setPixelColor(pos++, currColor);
        else
          analyzer[i].band.setPixelColor(pos++, offColor);

    analyzer[i].band.show();
  }
}

void ninSpectrumDisplay::showBufferColor(void)
{
  for (uint8_t i = 0; i < MSGEQ7_MAX_BAND; i++) {
      for (uint8_t b = 0; b < LEDS_BAND; b++)
          analyzer[i].band.setPixelColor(b, displayBufferColor[i][b]);
      analyzer[i].band.show();
  }
}

void ninSpectrumDisplay::showSpectrum(void)
{
  _readMSGEQ7();
    
  for(uint8_t band = 0; band < MSGEQ7_MAX_BAND; band++ ) {
    _AllOff(band);

    if (analyzer[band].value >=80)  { analyzer[band].band.setPixelColor(9, 0xCC0000); }
    if (analyzer[band].value >=70)  { analyzer[band].band.setPixelColor(8, 0x990000); }
    if (analyzer[band].value >=60)  { analyzer[band].band.setPixelColor(7, 0x550000); }
    if (analyzer[band].value >=50)  { analyzer[band].band.setPixelColor(6, 0x0F0000); }
    if (analyzer[band].value >=45)  { analyzer[band].band.setPixelColor(5, 0x0A0000); }
    if (analyzer[band].value >=40)  { analyzer[band].band.setPixelColor(4, 0x0A0000); }
    if (analyzer[band].value >=35)  { analyzer[band].band.setPixelColor(3, 0x00000A); }
    if (analyzer[band].value >=30)  { analyzer[band].band.setPixelColor(2, 0x00000A); }
    if (analyzer[band].value >=25)  { analyzer[band].band.setPixelColor(1, 0x000A00); }
    if (analyzer[band].value >=20)  { analyzer[band].band.setPixelColor(0, 0x000A00); }
    
    analyzer[band].band.show();
  }
}

void ninSpectrumDisplay::showDisplay(void) 
{ 
  for (uint8_t i = 0; i < MSGEQ7_MAX_BAND; i++) {
    analyzer[i].band.show();
  }
}

void ninSpectrumDisplay::setColor(uint32_t color) { currColor = color; }

void ninSpectrumDisplay::clearDisplay(void)
{
  for (uint8_t i = 0; i < MSGEQ7_MAX_BAND; i++) {
    displayBuffer[i] = 0x00;
    for (uint8_t j = 0; j < LEDS_BAND; j++) {
      displayBufferColor[i][j] = 0x000000;
      analyzer[i].band.setPixelColor(j, 0x000000);

    }
    analyzer[i].band.show();
  }
}

void ninSpectrumDisplay::_shiftBufferLeft(void)
{
  for (uint8_t i = 0; i < MSGEQ7_MAX_BAND-1; i++) {
    uint16_t t = displayBuffer[i+1];
    displayBuffer[i] = t;
  }

  displayBuffer[MSGEQ7_MAX_BAND-1] = scrollBuffer[0];

  for (uint8_t i = 0; i < MSGEQ7_MAX_BAND-1; i++) {
    uint8_t t = scrollBuffer[i+1];
    scrollBuffer[i] = t;
  }
}

void ninSpectrumDisplay::_readMSGEQ7(void) // Function to read 7 band equalizers
{
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  for(uint8_t band = 0; band < MSGEQ7_MAX_BAND; band++ ) {
    digitalWrite(strobePin, LOW); // strobe pin on the shield - kicks the IC up to the next band 
    delayMicroseconds(30); // 
    analyzer[band].value = analogRead(voutPin); // store reading

    // Constrain any value above 1023 or below filter Value
    analyzer[band].value = constrain(analyzer[band].value, DEFAULT_NOISE_FILTER, 1023);

    // Remap the value to a number between 0 and 255
    analyzer[band].value = map(analyzer[band].value, DEFAULT_NOISE_FILTER, 1023, 0, 255);

    digitalWrite(strobePin, HIGH); 
  }
}

void ninSpectrumDisplay::_initDisplay(uint8_t STROBE, uint8_t RESET, uint8_t VOUT, uint8_t line1Pin, uint8_t line2Pin, uint8_t line3Pin, uint8_t line4Pin, uint8_t line5Pin, uint8_t line6Pin, uint8_t line7Pin)
{
  // set pins to internal available ones
  strobePin = STROBE;
  resetPin = RESET;
  voutPin = VOUT;

  analyzer[0].pin = line1Pin;
  analyzer[1].pin = line2Pin;
  analyzer[2].pin = line3Pin;
  analyzer[3].pin = line4Pin;
  analyzer[4].pin = line5Pin;
  analyzer[5].pin = line6Pin;
  analyzer[6].pin = line7Pin;

  // init default values
  currPos = 0;
  currCharn = 0;
  currColor = 0x0000AA;
  offColor = 0x000000;

  // Setup Strips
  analyzer[0].band = Adafruit_NeoPixel(LEDS_BAND, analyzer[0].pin, NEO_GRB + NEO_KHZ800);
  analyzer[1].band = Adafruit_NeoPixel(LEDS_BAND, analyzer[1].pin, NEO_GRB + NEO_KHZ800);
  analyzer[2].band = Adafruit_NeoPixel(LEDS_BAND, analyzer[2].pin, NEO_GRB + NEO_KHZ800);
  analyzer[3].band = Adafruit_NeoPixel(LEDS_BAND, analyzer[3].pin, NEO_GRB + NEO_KHZ800);
  analyzer[4].band = Adafruit_NeoPixel(LEDS_BAND, analyzer[4].pin, NEO_GRB + NEO_KHZ800);
  analyzer[5].band = Adafruit_NeoPixel(LEDS_BAND, analyzer[5].pin, NEO_GRB + NEO_KHZ800);
  analyzer[6].band = Adafruit_NeoPixel(LEDS_BAND, analyzer[6].pin, NEO_GRB + NEO_KHZ800);

  // Set pins
  pinMode(resetPin, OUTPUT); // reset pin mode
  pinMode(strobePin, OUTPUT); // strobe pin mode

  // Begin strips and set all leds to off
  for (uint8_t band = 0; band < MSGEQ7_MAX_BAND; band++) {
    analyzer[band].band.begin();
    _AllOff(band);
  }

  // Clear image buffer
  for (uint8_t i = 0; i < MSGEQ7_MAX_BAND; i++) {
    displayBuffer[i] = 0x00;
  }

}

void ninSpectrumDisplay::_AllOff(uint8_t band)
{
  for (uint8_t x = 0; x < LEDS_BAND; x++) 
    analyzer[band].band.setPixelColor(x, 0x000000);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t ninSpectrumDisplay::_ColorWheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return analyzer[0].band.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else if(WheelPos < 170) {
      WheelPos -= 85;
      return analyzer[0].band.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    } else {
      WheelPos -= 170;
      return analyzer[0].band.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}
