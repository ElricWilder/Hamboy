#include "sprite.h"

Sprite::Sprite() 
	: coords(0, 0),
	  tileNum(0),
	  bgPriority(false),
	  xFlip(false),
	  yFlip(false),
	  palette1(false)
{}

uint8_t Sprite::readU8(uint16_t addr) const{
	uint16_t offset = addr % 4;
	uint8_t ret = 0;

	switch (offset) {
	case 0:
		return coords.y;
	case 1:
		return coords.x;
	case 2:
		return tileNum;
	case 3:
		set_bit8(BG_PRIORITY_BIT, ret, bgPriority);
		set_bit8(Y_FLIP_BIT, ret, yFlip);
		set_bit8(X_FLIP_BIT, ret, xFlip);
		set_bit8(PALETTE_BIT, ret, palette1);
		return ret;
	default:
		return 0;
	}
}

void Sprite::writeU8(uint16_t addr, uint8_t val) {
	uint16_t offset = addr % 4;
	switch (offset) {
	case 0:
		coords.y = val;
	case 1:
		coords.x = val;
	case 2:
		tileNum = val;
	case 3:
		bgPriority = get_bit8(BG_PRIORITY_BIT, val);
		yFlip = get_bit8(Y_FLIP_BIT, val);
		xFlip = get_bit8(X_FLIP_BIT, val);
		palette1 = get_bit8(PALETTE_BIT, val);
	}
}

bool Sprite::getBgPriority() const {
	return bgPriority;
}

Point Sprite::getCoords() const{
	signed int x = coords.x - X_OFFSET;
	signed int y = coords.y - Y_OFFSET;
	Point coords = { size_t(x), size_t(y) };
	return coords;
}

uint8_t Sprite::getTileNum() const{
	return this->tileNum;
}

bool Sprite::isXFlipped() const {
	return this->xFlip;
}

bool Sprite::isYFlipped() const {
	return this->yFlip;
}

bool Sprite::usePallete1() const {
	return this->palette1;
}