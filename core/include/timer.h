#pragma once
#include <cstdint>

constexpr uint16_t DIV  = 0xFF04;
constexpr uint16_t TIMA = 0xFF05;
constexpr uint16_t TMA  = 0xFF06;
constexpr uint16_t TAC  = 0xFF07;
constexpr uint8_t TAC_ENABLE_BIT = 2;
constexpr uint8_t TIMA_COOLDOWN_OVERFLOW = 4;

struct Timer {
	uint8_t counter;
	uint8_t div;
	uint8_t tima;
	uint8_t tma;
	uint8_t tac;
	uint8_t timaCooldown;

	Timer();
	uint8_t readTimer(uint16_t addr) const;
	void writeTimer(uint16_t addr, uint8_t val);
	bool tick(uint8_t mCycles);
	uint16_t getTimaPeriod();
	bool timaStatus();
};