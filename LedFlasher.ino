#define LED_FLASHER_NOT_USE_VECTOR
#include "LedFlasher.h"
LedFlasher NokiaSmsTune(13);
LedFlasher NokiaSmsTune1(11);

void setup() {
	NokiaSmsTune.SetPattern(new uint32_t[16]{ 15,200,15,200,15,300,200,250,200,300,15,200,15,200,15,2000 }, 16);
	NokiaSmsTune1.SetPattern(ALWAYS_OFF);
}

void loop() {
	NokiaSmsTune.Run();
	NokiaSmsTune1.Run();
}