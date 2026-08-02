#include "io.h"
#include "timer.h"
#include <memory>
#include <iostream>
IO::IO() 
	: buttons{},
	dpadSelected(false),
	faceSelected(false),
	ram{},
	timer()
{}


uint8_t IO::readU8(uint16_t addr) const {
	if (addr >= DIV && addr <= TAC) {
		return timer.readTimer(addr);
	}
	else if (addr == JOYPAD_ADDR) {
		return readJoypad();
	}
	else {
		uint16_t relativeAddr = addr - IO_START;
		return ram[relativeAddr];
	}
}

uint8_t IO::readJoypad() const{
	if (faceSelected == dpadSelected) {
		return 0;
	}

	size_t ret = 0;
	if (dpadSelected) {
		for (Buttons btn : DPAD_BUTTONS) {
			int idx = size_t(btn);
			uint8_t mask = (buttons[idx] ? 0 : 1) << (idx - 4);
			ret |= mask;
		}
	}
	else {
		for (Buttons btn : FACE_BUTTONS) {
			int idx = size_t(btn);
			uint8_t mask = (buttons[idx] ? 0 : 1) << idx;
			ret |= mask;
		}
	}
	//printf("Joypad read: %02X (face=%d dpad=%d)\n", ret, faceSelected, dpadSelected);

	return ret;
}

bool IO::updateTimer(uint8_t cycles) {
	return timer.tick(cycles);
}

void IO::setButton(Buttons button, bool pressed) {
	buttons[size_t(button)] = pressed;
}

void IO::writeU8(uint16_t addr, uint8_t val) {
	if (addr >= DIV && addr <= TAC) {
		timer.writeTimer(addr, val);
	}
	else if (addr == JOYPAD_ADDR) {
		faceSelected = !get_bit8(FACE_SELECT_BIT, val);
		dpadSelected = !get_bit8(DPAD_SELECT_BIT, val);
	}
	else if (addr == 0xFF01) {
		std::cout << val << "\n";
	}
	else if (addr == 0xFF0F) {
		ram[addr - IO_START] |= (val & 0x1F);
	}
	else {
		uint16_t relativeAddr = addr - IO_START;
		ram[relativeAddr] = val;
	}
}