#pragma once

#ifdef LED_FLASHER_USE_VECTOR
#include <ArduinoSTL.h>
#include <vector>
#include <algorithm>
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
//#define CFG_LAST_LED_STATE	5
//#define CFG_LAST_LED_STATE	5

#define LED_ON	1
#define LED_OFF	0

class LedFlasher {
private:
#ifdef LED_FLASHER_USE_VECTOR
	static std::vector<LedFlasher*> leds;
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
	}

public:

#ifdef LED_FLASHER_USE_VECTOR
	static void RunAll() {
		for (auto led : leds) {
			if (led == nullptr) {
				leds.erase(std::remove(leds.begin(), leds.end(), led), leds.end());
			}
			else
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
#ifdef LED_FLASHER_USE_VECTOR
		leds.push_back(this);
#endif
	}
	~LedFlasher() {
		if (bitRead(config, CFG_RAM_CLEANUP) && patternMap != nullptr)
			delete[] patternMap;

#ifdef LED_FLASHER_USE_VECTOR
		if (!leds.empty())
			leds.erase(std::remove(leds.begin(), leds.end(), this), leds.end());
#endif
	}

	void SetPattern(uint32_t * _pattern, uint8_t _length, bool _loop = true, bool _deleteLastMap = false) {
		if (_pattern == patternMap)
			return;
		
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
			if (bitRead(config, CFG_PATTERN_CHANGED)) {
				if (patternMap[0] == 0)
					SwitchLed(LED_OFF);
				else
					SwitchLed(LED_ON);
			}
			bitClear(config, CFG_PATTERN_CHANGED);
			return;
		}

		uint32_t currentMillis = millis();

		if ((currentMillis < nextSwitchMillis && !bitRead(config, CFG_PATTERN_CHANGED)))
			return;

		if (!bitRead(config, CFG_IS_ENABLED))
			return;

		SwitchLed(currentIndex % 2 == 0);

		nextSwitchMillis = currentMillis + patternMap[currentIndex];
		//prevent overflowing
		if (currentMillis > nextSwitchMillis)
			nextSwitchMillis = currentMillis;

		currentIndex++;
		if (currentIndex >= length) {
			currentIndex = 0;
			if (!bitRead(config, CFG_IS_LOOPED))
				bitClear(config, CFG_IS_ENABLED);
		}

		bitClear(config, CFG_PATTERN_CHANGED);
	}
};

#ifdef LED_FLASHER_USE_VECTOR
//keep compiler happy
std::vector<LedFlasher*> LedFlasher::leds;
#endif
