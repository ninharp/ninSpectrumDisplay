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

// TODO: Fix issue with x=4 printChar
// TODO: Move pins for led lines to constructor
// TODO: define alternate constructors
// TODO: fulfill the default font
// TODO: test other display sizes (calculations for fonts especially)
// TODO: github repository 
// TODO: README.md (images and so on blah blah)

#include <ninSpectrumDisplay.h>

ninSpectrumDisplay::ninSpectrumDisplay(int STROBE, int RESET, int VOUT)
{ 
    strobePin = STROBE;
    resetPin = RESET;
    voutPin = VOUT;

    currPos = 0;
    currCharn = 0;
    currColor = 0x0000AA;

    // Setup Strips
    analyzer[0].band = Adafruit_NeoPixel(LEDS_BAND, 2, NEO_GRB + NEO_KHZ800);
    analyzer[1].band = Adafruit_NeoPixel(LEDS_BAND, 3, NEO_GRB + NEO_KHZ800);
    analyzer[2].band = Adafruit_NeoPixel(LEDS_BAND, 4, NEO_GRB + NEO_KHZ800);
    analyzer[3].band = Adafruit_NeoPixel(LEDS_BAND, 5, NEO_GRB + NEO_KHZ800);
    analyzer[4].band = Adafruit_NeoPixel(LEDS_BAND, 6, NEO_GRB + NEO_KHZ800);
    analyzer[5].band = Adafruit_NeoPixel(LEDS_BAND, 7, NEO_GRB + NEO_KHZ800);
    analyzer[6].band = Adafruit_NeoPixel(LEDS_BAND, 8, NEO_GRB + NEO_KHZ800);

    pinMode(resetPin, OUTPUT); // reset pin mode
    pinMode(strobePin, OUTPUT); // strobe pin mode

    // Begin strips and set all leds to off
    for (uint8_t band = 0; band < MSGEQ7_MAX_BAND; band++) {
      analyzer[band].band.begin();
      _AllOff(band);
    }

    // Clear image buffer
    for (uint8_t i = 0; i < LEDS_BAND; i++)
      imageBuffer[i] = 0x00;
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
    uint16_t temp = charBuffer[i] + (imageBuffer[i] << 8);
    temp <<= MSGEQ7_MAX_BAND+1; //currPos;
    imageBuffer[i] = temp >> 8;
    imageBuffer[i] &=~ (1<<0);
    //imageBuffer[i] = charBuffer[i];
  }
  uint8_t charBuffer[LEDS_BAND];
  for (uint8_t i = 0; i < LEDS_BAND; i++) {
    charBuffer[i] = pgm_read_byte(&currFont.font[i+4]);
  }
  */

  for (uint8_t i = 0; i < LEDS_BAND; i++) {
    uint16_t temp = pgm_read_byte(&currFont.font[i+4+currFont.y_size]) + (imageBuffer[LEDS_BAND-1-i] << 8);
    temp <<= currFont.x_size+3-x; //MSGEQ7_MAX_BAND; //currPos;
    imageBuffer[LEDS_BAND-1-i] = temp >> 7;
    //imageBuffer[LEDS_BAND-1-i] >>= 2;
    //imageBuffer[LEDS_BAND-1-i] = charBuffer[i];
  }

  uint8_t mask = 0b10000000;
  for (uint8_t band = 0; band < currFont.x_size; band++) {
    for (uint8_t led = LEDS_BAND-currFont.y_size; led < LEDS_BAND; led++) {
      if (imageBuffer[led] & mask) {
        analyzer[band].band.setPixelColor(led-3+y, currColor);
      } else {
        analyzer[band].band.setPixelColor(led-3+y, 0x000000);
      }
    }
    //currColor = 0x0000AA; //_ColorWheel(((band * 256 / 100) + j) & 255);

    mask >>= 1;
    analyzer[band].band.show();
  }

  for (uint8_t i = 0; i < LEDS_BAND; i++) imageBuffer[i] = 0x00;

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
