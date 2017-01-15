# ninSpectrumDisplay Arduino Library

This is the Arduino Library for the *ninSpectrumDisplay* which you can find on my blog (link) to build it by yourself.

The Display consists of an Arduino Nano, an MSGEQ7 IC for the frequency splitting and 7 WS2812 Strips a 10 LEDs to visualize
the input sound levels. Additionally you can use the display to show text or small images, a sample font file you can find
in the library folder.

Note: If you want to change the size of the Display, you had to change some values in the header file of the library too.

## Features
* 24-Bit Colors
* Integrated Spectrum Analyzer function
* Full set pixel functions
* Print scrolling text
* Own fonts applicable (see fonts.h)

## Works on
* All Arduino boards (Uno, Due, Mini, Micro, Yun...)

## Quick Start

Download the zip file from the repository or check it out anywhere.

Then you have to copy the *ninSpectrumDisplay* folder to your *Arduino/libraries* 
folder in your Arduino environment.

.
.
.

## Documentation

The library uses doxygen to generate automagically an API documentation of all functions
of the library, the doxygen definition was found in *doxyfile*

## License

...
