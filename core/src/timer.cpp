#include "timer.h"
#include "utils.h"

Timer::Timer()
	: counter(0),
	div(0),
	tima(0),
	tma(0),
	tac(0),
	timaCooldown(0)
{}

uint8_t Timer::readTimer(uint16_t addr) const{
	if (addr == DIV) {
		return div;
	}
	else if (addr == TIMA) {
		return tima;
	} 
	else if (addr == TMA) {
		return tma;
	}
	else if (addr == TAC) {
		return tac;
	}
	else {
		return 0;
	}
}

void Timer::writeTimer(uint16_t addr, uint8_t val) {
	if (addr == DIV) {
		div = 0;
		counter = 0;
	}
	else if (addr == TIMA) {
		tima = val;
		timaCooldown = 0;
	}
	else if (addr == TMA) {
		tma = val;
	}
	else if (addr == TAC) {
		tac = val;
	}
}

bool Timer::tick(uint8_t mCycles) {
	bool interrupt = false;
	uint8_t tCycles = 4 * mCycles;

	for (uint8_t i = 0; i < tCycles; i++) {
		uint16_t oldDiv16 = (uint16_t(div) << 8) | counter;
		bool oldBit = (oldDiv16 & getTimaPeriod()) != 0;

		counter += 1;

		if (counter == 0) {
			div++;
		}

		uint16_t newDiv16 = (uint16_t(div) << 8) | counter;
		bool newBit = (newDiv16 & getTimaPeriod()) != 0;

		bool enabled = get_bit8(TAC_ENABLE_BIT, tac);

		if (timaCooldown != 0) {
			timaCooldown -= 1;
			if (timaCooldown == 0) {
				tima = tma;
				interrupt = true;
			}
			continue;
		}

		if (enabled && oldBit && !newBit) {
			uint8_t oldTima = tima;
			tima += 1;
			bool timaOverflow = (tima < oldTima);

			if (timaOverflow) {
				timaCooldown = TIMA_COOLDOWN_OVERFLOW;
			}
		}
	}

	return interrupt;	
}

uint16_t Timer::getTimaPeriod() {
	switch (tac & 0b11) {
	case 0b00:
		return (1 << 9);
	case 0b01:
		return (1 << 3);
	case 0b10:
		return (1 << 5);
	case 0b11:
		return (1 << 7);
	default:
		return 0;
	}
}

bool Timer::timaStatus() {
	return ((uint16_t(div) & getTimaPeriod()) != 0);
}