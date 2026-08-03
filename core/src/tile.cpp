#include "tile.h"
#include <stdexcept>
#include "utils.h"


Tile::Tile() {
	memset(raw, 0, sizeof(raw));
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			pixels[r][c] = 0;
		}
	}
}

uint8_t Tile::readU8(uint16_t offset) const {
	if (offset >= 16) {
		throw std::out_of_range("Offset to large to fit in this tile");
	}

	size_t row = offset / 2;
	bool high = offset & 1;

	uint8_t result = 0;
	for (int col = 0; col < 8; col++) {
		uint8_t color = pixels[row][col];
		uint8_t bit = high ? ((color >> 1) & 1) : (color & 1);
		result |= bit << (7 - col);
	}
	
	return result;
}

void Tile::writeU8(uint16_t offset, uint8_t val) {
	if (offset >= 16) {
		throw std::out_of_range("Offset to large to fit in this tile");
	}

	raw[offset] = val;

	int row = offset / 2;

	uint8_t lo = raw[row * 2];
	uint8_t hi = raw[row * 2 + 1];

	for (int col = 0; col < 8; col++) {
		int bit = 7 - col;
		uint8_t color =
			((hi >> bit) & 1) << 1 |
			((lo >> bit) & 1);
		pixels[row][col] = color;
	}

}

std::array<uint8_t, 8> Tile::getRow(uint8_t row) const{
	return pixels[row];
}