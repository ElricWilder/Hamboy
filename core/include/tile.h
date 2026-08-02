#pragma once
#include <cstdint>
#include <array>
#include <iostream>

struct Tile {
	std::array<uint8_t, 8> pixels[8];
	uint8_t raw[16];

	Tile();
	
	uint8_t readU8(uint16_t offset) const;
	void writeU8(uint16_t offset, uint8_t val);

	std::array<uint8_t, 8> getRow(uint8_t row) const;
};