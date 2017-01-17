// simple_text_scroller.ino
#include <Adafruit_NeoPixel.h>
#include <ninSpectrumDisplay.h>

#define MSGEQ7_STROBE_PIN 11 // strobe pins on digital 4
#define MSGEQ7_RESET_PIN  10 // reset pins on digital 5
#define MSGEQ7_VOUT_PIN	  A1 // data pin on ADC 1

//ninSpectrumDisplay nin(MSGEQ7_STROBE_PIN, MSGEQ7_RESET_PIN, MSGEQ7_VOUT_PIN, 2, 3, 4, 5, 6, 7, 8);
ninSpectrumDisplay nin(MSGEQ7_STROBE_PIN, MSGEQ7_RESET_PIN, MSGEQ7_VOUT_PIN);

void setup() {
	// Clears the display initially
	nin.clearDisplay();

	// Sets default font
	nin.setFont(defFont);

	// Sets color to dark blue
	nin.setColor(0x040000);

	nin.printLine(1,1,7,10);
	nin.showBuffer();
	delay(1000);

	nin.printLine(1,10,7,1);
	nin.showBuffer();
	delay(2000);
}

void loop() {
}

