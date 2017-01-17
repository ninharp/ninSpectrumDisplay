// simple_text_scroller.ino
#include <Adafruit_NeoPixel.h>
#include <ninSpectrumDisplay.h>

#define MSGEQ7_STROBE_PIN 11 // strobe pins on digital 4
#define MSGEQ7_RESET_PIN  10 // reset pins on digital 5
#define MSGEQ7_VOUT_PIN	  A1 // data pin on ADC 1

//ninSpectrumDisplay nin(MSGEQ7_STROBE_PIN, MSGEQ7_RESET_PIN, MSGEQ7_VOUT_PIN, 2, 3, 4, 5, 6, 7, 8);
ninSpectrumDisplay nin(MSGEQ7_STROBE_PIN, MSGEQ7_RESET_PIN, MSGEQ7_VOUT_PIN);

void setup() {
	// Clears the display initially and also clears the displayBuffer
	nin.clearDisplay();

	// Sets color to dark blue, that color is used if color parameter is omitted
	nin.setColor(0x040000);

}

void loop() {
	nin.clearDisplay();
	delay(400);

	nin.printLine(1,1,7,10, 0x000004);
	delay(400);

	nin.printLine(1,10,7,1, 0x000400);
	delay(400);

	nin.printLine(4,1,4,10, 0x040000);
	delay(400);

	nin.printLine(1,5,7,5, 0x040404);
	delay(400);

}

