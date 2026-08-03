#pragma once
#include "utils.h"
#include "timer.h"
#include <cstdint>

constexpr uint16_t IO_START = 0xFF00;
constexpr uint16_t IO_STOP = 0xFF3F;

constexpr uint16_t JOYPAD_ADDR = 0xFF00;
constexpr size_t IO_SIZE = size_t(IO_STOP - IO_START + 1);

constexpr uint8_t FACE_SELECT_BIT = 5;
constexpr uint8_t DPAD_SELECT_BIT = 4;

enum class Buttons {
	A      = 0,
	B      = 1,
	Select = 2,
	Start  = 3,
	Right  = 4,
	Left   = 5,
	Up     = 6,
	Down   = 7,
};

const Buttons DPAD_BUTTONS[4] = {
	Buttons::Right,
	Buttons::Left,
	Buttons::Up,
	Buttons::Down
};

const Buttons FACE_BUTTONS[4] = {
	Buttons::A,
	Buttons::B,
	Buttons::Select,
	Buttons::Start
};

struct IO {
	bool buttons[8];
	bool dpadSelected;
	bool faceSelected;
	uint8_t ram[IO_SIZE];
	Timer timer;

	IO();

	uint8_t readU8(uint16_t addr) const;
	uint8_t readJoypad() const;
	bool updateTimer(uint8_t cycles);
	void setButton(Buttons button, bool pressed);
	void writeU8(uint16_t addr, uint8_t val);
};