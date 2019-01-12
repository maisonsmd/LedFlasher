#define LED_FLASHER_NOT_USE_VECTOR
#include "LedFlasher.h"
#include <MemoryFree.h>

LedFlasher NokiaSmsTune(11);
LedFlasher NokiaSmsTune1(13);

void setup() {
	Serial.begin(115200);
	NokiaSmsTune.SetPattern(new uint32_t[16]{ 2000,15,200,15,200,15,300,200,250,200,300,15,200,15,200,15 }, 16);
	//NokiaSmsTune1.SetPattern(new uint32_t[16]{ 2000,15,200,15,200,15,300,200,250,200,300,15,200,15,200,15 }, 16);
}

void loop() {
	NokiaSmsTune.Run();
	//NokiaSmsTune1.Run();
	Serial.println(freeMemory());
}