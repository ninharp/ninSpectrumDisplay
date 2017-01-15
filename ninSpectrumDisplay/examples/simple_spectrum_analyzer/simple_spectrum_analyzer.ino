// simple_spectrum_analyzer.ino
#include <Adafruit_NeoPixel.h>
#include <ninSpectrumDisplay.h>

#define MSGEQ7_STROBE_PIN 11 // strobe pins on digital 4
#define MSGEQ7_RESET_PIN  10 // reset pins on digital 5
#define MSGEQ7_VOUT_PIN	  A1 // data pin on ADC 1

ninSpectrumDisplay nin(MSGEQ7_STROBE_PIN, MSGEQ7_RESET_PIN, MSGEQ7_VOUT_PIN);
//ninSpectrumDisplay nin(MSGEQ7_STROBE_PIN, MSGEQ7_RESET_PIN, MSGEQ7_VOUT_PIN, 2, 3, 4, 5, 6, 7, 8);

void setup() {
}

void loop() {
	nin.showSpectrum();
}

