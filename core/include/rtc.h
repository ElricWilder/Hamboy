#pragma once

#include <chrono>

constexpr uint64_t SECS_IN_MIN = 60;
constexpr uint64_t MINS_IN_HOUR = 60;
constexpr uint64_t HOURS_IN_DAY = 24;

constexpr uint8_t DAY_HIGH_BIT = 0;
constexpr uint8_t HALT_BIT = 6;
constexpr uint8_t DAY_OVERFLOW_BIT = 7;

struct Rtc {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint16_t days;
	bool enabled;
	bool halted;
	std::chrono::steady_clock::time_point start;

	Rtc();
	void latchTime();
	bool isEnabled() const;
	uint8_t readByte(uint8_t bank) const;
	void writeByte(uint8_t bank, uint8_t val);
};