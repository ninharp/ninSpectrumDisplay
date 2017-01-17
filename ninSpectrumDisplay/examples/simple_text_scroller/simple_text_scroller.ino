// simple_text_scroller.ino
#include <Adafruit_NeoPixel.h>
#include <ninSpectrumDisplay.h>

#define MSGEQ7_STROBE_PIN 11 // strobe pins on digital 4
#define MSGEQ7_RESET_PIN  10 // reset pins on digital 5
#define MSGEQ7_VOUT_PIN	  A1 // data pin on ADC 1

//ninSpectrumDisplay nin(MSGEQ7_STROBE_PIN, MSGEQ7_RESET_PIN, MSGEQ7_VOUT_PIN, 2, 3, 4, 5, 6, 7, 8);
ninSpectrumDisplay nin(MSGEQ7_STROBE_PIN, MSGEQ7_RESET_PIN, MSGEQ7_VOUT_PIN);

// includes the default font (font.c)
extern uint8_t defFont[];

void setup() {
	// Clears the display initially
	nin.clearDisplay();

	// Sets color to dark blue
	nin.setColor(0x000005);

	// Sets default font
	nin.setFont(defFont);
}

void loop() {
	// Prints the scrolling text
	nin.scrollString(1, 100, "Hallo Welt");
	delay(500);
}

