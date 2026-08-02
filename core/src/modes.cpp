#include "modes.h"
#include <iostream>

LcdResults Lcd::step(uint8_t cyclesPassed) {
	cycles += cyclesPassed;

	LcdResults result = LcdResults::NoAction;

	switch (mode) {
	case LcdModeType::HBLANK:
		if (cycles >= HBLANK_LEN) {
			cycles -= HBLANK_LEN;
			line++;
			//std::cout << "line count is: " << int(line) << "\n";

			if (line == VBLANK_LINE_START) {
				mode = LcdModeType::VBLANK;
				result = LcdResults::RenderFrame;
			}
			else {
				mode = LcdModeType::OAMReadMode;
			}
		}
		break;

	case LcdModeType::VBLANK:
		if (cycles >= VBLANK_LEN) {
			cycles -= VBLANK_LEN;
			line++;

			if (line > VBLANK_LINE_END) {
				mode = LcdModeType::OAMReadMode;
				line = 0;
			}
		}
		break;

	case LcdModeType::OAMReadMode:
		if (cycles >= OAM_READ_LEN) {
			cycles -= OAM_READ_LEN;
			mode = LcdModeType::VRAMReadMode;
		}
		break;

	case LcdModeType::VRAMReadMode:
		if (cycles >= VRAM_READ_LEN) {
			cycles -= VRAM_READ_LEN;
			mode = LcdModeType::HBLANK;
			result = LcdResults::RenderLine;
		}
		break;
	}

	return result;
}