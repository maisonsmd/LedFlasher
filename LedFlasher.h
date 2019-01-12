#pragma once


#ifndef LED_FLASHER_NOT_USE_VECTOR
#include <ArduinoSTL.h>
#include <vector>
#include <algorithm>
using namespace std;
#endif

uint32_t _always_on[] = { 1 };
uint32_t _always_off[] = { 0 };

#define ALWAYS_ON	_always_on, 1, false
#define ALWAYS_OFF	_always_off, 1, false

#define CFG_PATTERN_CHANGED	0
#define CFG_IS_LOOPED		1
#define CFG_ACTIVE_STATE	2
#define CFG_IS_ENABLED		3

#define CFG_RAM_CLEANUP		4
#define CFG_LAST_LED_STATE	5

#define LED_ON	1
#define LED_OFF	0

class LedFlasher {
private:
#ifndef LED_FLASHER_NOT_USE_VECTOR
	static vector<LedFlasher*> leds;
#endif

	uint8_t currentIndex = 0;
	uint8_t length = 0;
	uint32_t * patternMap = nullptr;
	uint32_t nextSwitchMillis = 0;
	int8_t pin;
	uint8_t config;

	void SwitchLed(bool _state) {
		if (OnStateChanged)
			OnStateChanged(_state);
		else if (pin != -1) {
			if (_state)
				digitalWrite(pin, bitRead(config, CFG_ACTIVE_STATE));
			else
				digitalWrite(pin, !bitRead(config, CFG_ACTIVE_STATE));
		}
		bitWrite(config, CFG_LAST_LED_STATE, _state);
	}

public:

#ifndef LED_FLASHER_NOT_USE_VECTOR
	static void RunAll() {
		for (auto led : leds) {
			if (led == nullptr) {
				leds.erase(std::remove(leds.begin(), leds.end(), led), leds.end());
			} else
				led->Run();
		}
	}
#endif

	//custom callback
	void(*OnStateChanged)(bool _state) = nullptr;

	LedFlasher(uint8_t _pin = -1, uint8_t _activeState = HIGH) {
		pin = _pin;
		if (pin != -1)
			pinMode(pin, OUTPUT);
		bitWrite(config, CFG_ACTIVE_STATE, _activeState);
	#ifndef LED_FLASHER_NOT_USE_VECTOR
		leds.push_back(this);
	#endif
	}
	~LedFlasher() {
		if (bitRead(config, CFG_RAM_CLEANUP) && patternMap != nullptr) {
			delete[] patternMap;
		}

	#ifndef LED_FLASHER_NOT_USE_VECTOR
		leds.erase(std::remove(leds.begin(), leds.end(), this), leds.end());
	#endif
	}

	void SetPattern(uint32_t * _pattern, uint8_t _length, bool _loop = true, bool _deleteLastMap = false) {
		if (_deleteLastMap && patternMap != nullptr) {
			delete[] patternMap;
		}

		patternMap = _pattern;
		length = _length;
		currentIndex = 0;
		//nextSwitchMillis = millis();

		bitWrite(config, CFG_IS_LOOPED, _loop);
		bitWrite(config, CFG_RAM_CLEANUP, _deleteLastMap);
		bitSet(config, CFG_PATTERN_CHANGED);
		Enable(true);
	}

	void Enable(bool _enable) {
		bitWrite(config, CFG_IS_ENABLED, _enable);
		//SwitchLed(LED_OFF);
		//currentIndex = 0;
	}

	void Loop(bool _loop) {
		bitWrite(config, CFG_IS_LOOPED, _loop);
		if (_loop)
			bitSet(config, CFG_IS_ENABLED);
	}

	void Run() {
		if (patternMap == nullptr || length < 1)
			return;
		if (length == 1) {
			if (patternMap[0] == 0)
				SwitchLed(LED_OFF);
			else
				SwitchLed(LED_ON);
			return;
		}

		uint32_t currentMillis = millis();

		if ((currentMillis < nextSwitchMillis && !bitRead(config, CFG_PATTERN_CHANGED)))
			return;

		if (!bitRead(config, CFG_IS_ENABLED))
			return;

		bitClear(config, CFG_PATTERN_CHANGED);

		bool lastState = bitRead(config, CFG_LAST_LED_STATE);
		SwitchLed(!lastState);

		currentIndex++;
		if (currentIndex >= length) {
			currentIndex = 0;
			if (!bitRead(config, CFG_IS_LOOPED))
				bitClear(config, CFG_IS_ENABLED);
		}

		nextSwitchMillis = currentMillis + patternMap[currentIndex];
	}
};

#ifndef LED_FLASHER_NOT_USE_VECTOR
//keep compiler happy
vector<LedFlasher*> LedFlasher::leds;
#endif