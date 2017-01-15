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

#include <ninSpectrumDisplay.h>
#include "Adafruit_NeoPixel.h"

ninSpectrumDisplay::ninSpectrumDisplay(uint8_t STROBE, uint8_t RESET, uint8_t VOUT)
{ 
   ninSpectrumDisplay(STROBE, RESET, VOUT, 2, 3, 4, 5, 6, 7, 8); 
}

ninSpectrumDisplay::ninSpectrumDisplay(uint8_t STROBE, uint8_t RESET, uint8_t VOUT, uint8_t line1Pin, uint8_t line2Pin, uint8_t line3Pin, uint8_t line4Pin, uint8_t line5Pin, uint8_t line6Pin, uint8_t line7Pin) {

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
  for (uint8_t i = 0; i < LEDS_BAND; i++)
    displayBuffer[i] = 0x00;
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

void ninSpectrumDisplay::printChar(uint8_t x, uint8_t y, char ch)
{
  /*
  uint8_t charBuffer[LEDS_BAND] = {  
              0b00000000,
              0b00111000,
              0b00111000,
              0b00100000,
              0b00111000,
              0b00111000,
              0b00100000,
              0b00111000,
              0b00111000,
              0b00000000
               };

  for (uint8_t i = 0; i < LEDS_BAND; i++) {
    uint16_t temp = charBuffer[i] + (displayBuffer[i] << 8);
    temp <<= MSGEQ7_MAX_BAND+1; //currPos;
    displayBuffer[i] = temp >> 8;
    displayBuffer[i] &=~ (1<<0);
    //displayBuffer[i] = charBuffer[i];
  }
  uint8_t charBuffer[LEDS_BAND];
  for (uint8_t i = 0; i < LEDS_BAND; i++) {
    charBuffer[i] = pgm_read_byte(&currFont.font[i+4]);
  }
  */

  for (uint8_t i = 0; i < LEDS_BAND; i++) {
    uint16_t temp = pgm_read_byte(&currFont.font[i+4+currFont.y_size]) + (displayBuffer[LEDS_BAND-1-i] << 8);
    temp <<= currFont.x_size+3-x; //MSGEQ7_MAX_BAND; //currPos;
    displayBuffer[LEDS_BAND-1-i] = temp >> 7;
    //displayBuffer[LEDS_BAND-1-i] >>= 2;
    //displayBuffer[LEDS_BAND-1-i] = charBuffer[i];
  }

  uint8_t mask = 0b10000000;
  for (uint8_t band = 0; band < currFont.x_size; band++) {
    for (uint8_t led = LEDS_BAND-currFont.y_size; led < LEDS_BAND; led++) {
      if (displayBuffer[led] & mask) {
        analyzer[band].band.setPixelColor(led-3+y, currColor);
      } else {
        analyzer[band].band.setPixelColor(led-3+y, 0x000000);
      }
    }
    //currColor = 0x0000AA; //_ColorWheel(((band * 256 / 100) + j) & 255);

    mask >>= 1;
    analyzer[band].band.show();
  }

  for (uint8_t i = 0; i < LEDS_BAND; i++) displayBuffer[i] = 0x00;

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

void ninSpectrumDisplay::setColor(uint32_t color) { currColor = color; }

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

void ninSpectrumDisplay::_AllOff(uint8_t band)
{
  //for (uint8_t i = 0; i < MSGEQ7_MAX_BAND; i++)
  //{
  for (uint8_t x = 0; x < LEDS_BAND; x++) 
    analyzer[band].band.setPixelColor(x, 0x000000);
  //}
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
