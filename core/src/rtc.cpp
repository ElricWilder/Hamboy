#include "rtc.h"
#include "utils.h"

Rtc::Rtc()
	: seconds(0),
	minutes(0),
	hours(0),
	days(0),
	enabled(false),
	halted(false),
	start(std::chrono::steady_clock::now())
{
}

void Rtc::latchTime() {
	using Clock = std::chrono::steady_clock;

	auto now = Clock::now();
	auto delta = now - start;

	uint64_t dSec = std::chrono::duration_cast<std::chrono::seconds>(delta).count();
	seconds = static_cast<uint8_t>(dSec % SECS_IN_MIN);

	uint64_t dMin = dSec / SECS_IN_MIN;
	minutes = static_cast<uint8_t>(dMin % MINS_IN_HOUR);

	uint64_t dHour = dMin / MINS_IN_HOUR;
	hours = static_cast<uint8_t>(dHour % HOURS_IN_DAY);

	uint64_t dDays = dHour / HOURS_IN_DAY;
	days = static_cast<uint8_t>(dDays);
}

bool Rtc::isEnabled() const{
	return enabled;
}

uint8_t Rtc::readByte(uint8_t bank) const {
	switch (bank) {
	case 0x08:
		return seconds;
	case 0x09:
		return minutes;
	case 0x0A:
		return hours;
	case 0x0B:
		return (days & 0xFF);
	case 0x0C: {
		uint8_t ret = 0;
		ret = set_bit8(DAY_HIGH_BIT, ret, get_bit16(9, days));
		ret = set_bit8(HALT_BIT, ret, halted);
		ret = set_bit8(DAY_OVERFLOW_BIT, ret, get_bit16(10, days));
	}
	default:
		return 0;
	}
}

void Rtc::writeByte(uint8_t bank, uint8_t val) {
	switch (bank) {
	case 0x08:
		seconds = val;
		break;
	case 0x09:
		minutes = val;
		break;
	case 0x0A:
		hours = val;
		break;
	case 0x0B:
		days = ((days & 0xFF00) | uint16_t(val));
		break;
	case 0x0C:
		days = set_bit16(9, days, get_bit8(val, DAY_HIGH_BIT));
		halted = get_bit8(HALT_BIT, val);
		days = set_bit16(10, days, get_bit8(val, DAY_OVERFLOW_BIT));
		break;
	default:
		if (val == 0x00) {
			enabled = false;
		}
		else if ((val == 0x01) && (!enabled)) {
			enabled = true;
			latchTime();
		}
		
	}
}