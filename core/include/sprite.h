#pragma once

#include "utils.h"
#include <utility>

constexpr uint8_t BG_PRIORITY_BIT = 7;
constexpr uint8_t Y_FLIP_BIT = 6;
constexpr uint8_t X_FLIP_BIT = 5;
constexpr uint8_t PALETTE_BIT = 4;

constexpr signed Y_OFFSET = 16;
constexpr signed X_OFFSET = 8;


struct Sprite {
	Point coords;
	uint8_t tileNum;
	bool bgPriority;
	bool xFlip;
	bool yFlip;
	bool palette1;

	Sprite();
	uint8_t readU8(uint16_t addr) const;
	void writeU8(uint16_t addr, uint8_t val);

	bool getBgPriority() const;
	Point getCoords() const;
	uint8_t getTileNum() const;
	bool isXFlipped() const;
	bool isYFlipped() const;
	bool usePallete1() const;
};