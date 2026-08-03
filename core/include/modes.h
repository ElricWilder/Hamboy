#pragma once
#include <cstdint>

static constexpr size_t HBLANK_LEN = 204;
static constexpr size_t VBLANK_LEN = 456;
static constexpr size_t OAM_READ_LEN = 80;
static constexpr size_t VRAM_READ_LEN = 172;
static constexpr uint8_t VBLANK_LINE_START = 144;
static constexpr uint8_t VBLANK_LINE_END = 153;

enum class LcdModeType : uint8_t {
	HBLANK = 0,
	VBLANK = 1,
	OAMReadMode = 2,
	VRAMReadMode = 3
};

enum class LcdResults {
	NoAction,
	RenderFrame,
	RenderLine
};

inline uint8_t get_idx(LcdModeType mode) {
	return static_cast<uint8_t>(mode);
}

struct Lcd {
	LcdModeType mode;
	size_t cycles;
	uint8_t line;

	Lcd()
		: mode(LcdModeType::HBLANK),
		cycles(0),
		line(0)
	{}

	uint8_t getLine() const {
		return line;
	}

	LcdModeType getMode() const {
		return mode;
	}

	LcdResults step(uint8_t cyclesPassed);
};
