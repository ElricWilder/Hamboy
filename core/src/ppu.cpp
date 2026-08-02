#include "ppu.h"
#include "utils.h"
#include "tile.h"
#include "cpu.h"
#include <stdexcept>
#include <iostream>

Ppu::Ppu()
	: screenBuffer{}, mode(), tiles{}, maps{}, lcdRegs{}, oam{}
{
}


uint8_t Ppu::readVRAM(uint16_t addr) const {
	if (addr >= TILE_SET_START && addr <= TILE_SET_STOP) {
		uint16_t relativeAddr = addr - TILE_SET_START;
		uint16_t tileIDx = relativeAddr / BYTES_PER_TILE;
		uint16_t offset = relativeAddr % BYTES_PER_TILE;

		return tiles[tileIDx].readU8(offset);
	}
	else if (addr >= TILE_MAP_START && addr <= TILE_MAP_STOP) {
		uint16_t relativeAddr = addr - TILE_MAP_START;
		return maps[relativeAddr];
	}
	else {
		// throw error
	}
}

void Ppu::writeVRAM(uint16_t addr, uint8_t val) {
	if (addr >= TILE_SET_START && addr <= TILE_SET_STOP) {
		uint16_t relativeAddr = addr - TILE_SET_START;
		uint16_t tileIDx = relativeAddr / BYTES_PER_TILE;
		uint16_t offset = relativeAddr % BYTES_PER_TILE;

		tiles[tileIDx].writeU8(offset, val);
	}
	else if (addr >= TILE_MAP_START && addr <= TILE_MAP_STOP) {
		uint16_t relativeAddr = addr - TILE_MAP_START;
		maps[relativeAddr] = val;

	}
	else {
		// throw error
	}
}

PpuUpdateResult Ppu::update(uint8_t cycles) {
	LcdModeType lastMode = mode.getMode();
	uint8_t lastLine = mode.getLine();

	LcdResults lcdResult = mode.step(cycles);

	uint8_t newLine = mode.getLine();
	LcdModeType newMode = mode.getMode();

	uint8_t stat = readLcdReg(STAT);
	bool irq = false;
	bool vblankIrq = false;

	// check to see if LY has changed
	if (lastLine != newLine) {
		writeLcdReg(LY, newLine);
		uint8_t lyc = readLcdReg(LYC);
		bool equal = (newLine == lyc);
		// set STAT bit 2 if LY and LYC are equal
		stat = set_bit8(STAT_LY_EQ_LYC_BIT, stat, equal);
		// enable irq if at lyc trigger line and the LYC irq bit is set
		irq = (equal && get_bit8(STAT_LY_LYC_IRQ_BIT, stat));
	}

	 // check to see if mode has changed
	if (lastMode != newMode) {
		switch (newMode) {
		case LcdModeType::HBLANK:
			irq |= get_bit8(STAT_HBLANK_IRQ_BIT, stat);
			break;
		case LcdModeType::VBLANK:
			irq |= get_bit8(STAT_VBLANK_IRQ_BIT, stat);
			if (LY == 144) {
				vblankIrq = true;
			}
			break;
		case LcdModeType::OAMReadMode:
			irq |= get_bit8(STAT_OAM_IRQ_BIT, stat);
			break;
		default:
			break;
		}
	}

	// Assign last 2 bits of stat register the current mode ID
	stat &= 0b11111100;
	stat |= get_idx(newMode);
	writeLcdReg(STAT, stat);

	return PpuUpdateResult{ lcdResult, irq, vblankIrq };
}

uint8_t Ppu::readLcdReg(uint16_t addr) const{
	uint16_t relativeAddr = addr - LCD_REG_START;
	// hardcoded value for gb doctor test
	if (relativeAddr == 4) {
		//return 0x90;
		return lcdRegs[relativeAddr];
	}
	else {
		return lcdRegs[relativeAddr];
	}
}

void Ppu::writeLcdReg(uint16_t addr, uint8_t val) {
	if (addr == SCX || addr == SCY) {
		printf("SCROLL WRITE: %04X = %02X\n", addr, val);
	}
	uint16_t relativeAddr = addr - LCD_REG_START;
	lcdRegs[relativeAddr] = val;
}

bool Ppu::isLcdEnabled() const{
	uint8_t lcdc = readLcdReg(LCDC);
	return get_bit8(LCDC_LCD_ENABLED_BIT, lcdc);
}

uint8_t Ppu::getWndwTileMapIndex() const{
	uint8_t lcdc = readLcdReg(LCDC);
	if (get_bit8(LCDC_WNDW_MAP_BIT, lcdc)) {
		return 1;
	}
	else {
		return 0;
	}
}

bool Ppu::isWndwLayerDisplayed() {
	uint8_t lcdc = readLcdReg(LCDC);
	return (get_bit8(LCDC_WNDW_ENABLED_BIT, lcdc) && get_bit8(LCDC_BG_WNDW_ENABLED_BIT, lcdc));
}

uint8_t Ppu::getBgWndwTileSetIndex() const{
	uint8_t lcdc = readLcdReg(LCDC);
	if (get_bit8(LCDC_BG_WNDW_TILE_BIT, lcdc)) {
		return 1;
	}
	else {
		return 0;
	}
}

uint8_t Ppu::getBgTileMapIndex() const{
	uint8_t lcdc = readLcdReg(LCDC);
	if (get_bit8(LCDC_BG_MAP_BIT, lcdc)) {
		return 1;
	}
	else {
		return 0;
	}
}

bool Ppu::areSprites8x16() const {
	uint8_t lcdc = readLcdReg(LCDC);
	return get_bit8(LCDC_SPR_SIZE_BIT, lcdc);
}

bool Ppu::isSpriteLayerDisplayed(){
	uint8_t lcdc = readLcdReg(LCDC);
	return get_bit8(LCDC_SPR_ENABLED_BIT, lcdc);
}

bool Ppu::isBgLayerDisplayed(){
	uint8_t lcdc = readLcdReg(LCDC);
	return get_bit8(LCDC_BG_WNDW_ENABLED_BIT, lcdc);
}

Point Ppu::getViewportCoords() const{
	uint8_t x = readLcdReg(SCX);
	uint8_t y = readLcdReg(SCY);
	return Point(x, y);
}

// WX returns x + 7 so we need to subtract 7 but prevent underflow
Point Ppu::getWindowCoords() const{
	uint8_t x = readLcdReg(WX);
	uint8_t y = readLcdReg(WY);
	uint8_t sx;
	if (x >= 7) {
		sx = x - 7;
	}
	else {
		sx = 0;
	}

	return Point(sx, y);
}

std::array<uint8_t, 4> Ppu::getBgPalette() const{
	return unpack_u8(readLcdReg(BGP));
}

std::array<uint8_t, 4> Ppu::getSpritePalette(uint8_t index) const{
	switch (index) {
	case 0:
		return unpack_u8(readLcdReg(OBP0));
	case 1:
		return unpack_u8(readLcdReg(OBP0));
	default:
		throw std::runtime_error("Sprite Palette Index Not Found");
	}
}

std::array<uint8_t, DISPLAY_BUFFER> Ppu::render() const{
	if (isLcdEnabled()) {
		return screenBuffer;
	}
	else {
		return {};
	}
}

void Ppu::renderBg(std::array<uint8_t, SCREEN_WIDTH * 4>& buffer, uint8_t line) {
	size_t mapOffset = getBgTileMapIndex() * TILE_MAP_TABLE_SIZE;
	std::array<uint8_t, 4> palette = getBgPalette();
	Point viewport = getViewportCoords();

	size_t currentY = size_t(viewport.y) + line;
	size_t y = currentY % MAP_PIXELS;
	size_t row = currentY % TILESIZE;
	for (size_t px = 0; px < SCREEN_WIDTH; px++) {
		size_t currentX = size_t(viewport.x) + px;
		size_t x = currentX % MAP_PIXELS;
		size_t col = currentX % TILESIZE;

		size_t mapNum = (y / TILESIZE) * LAYERSIZE + (x / TILESIZE);
		uint8_t rawTileIndex = maps[mapOffset + mapNum];
		size_t tileIndex;
		int8_t signedIndex = static_cast<int8_t>(rawTileIndex);

		if (getBgWndwTileSetIndex() == 1) {
			tileIndex = rawTileIndex; // unsigned
		}
		else {
			tileIndex = 256 + signedIndex; // signed
		}

		if (tileIndex >= NUM_TILES) {
			printf("BAD TILE INDEX: raw=%u signed=%d final=%zu\n",
				rawTileIndex, signedIndex, tileIndex);
		}

		const Tile& tile = tiles[tileIndex];
		auto rowData = tile.getRow(row);

		uint8_t cell = rowData[col];
		uint8_t colorIdx = palette[cell];
		const auto& color = GB_PALETTE[colorIdx];

		size_t base = px * 4;
		buffer[base + 0] = color[0];
		buffer[base + 1] = color[1];
		buffer[base + 2] = color[2];
		buffer[base + 3] = color[3];
	}
}

void Ppu::renderWindow(std::array<uint8_t, SCREEN_WIDTH * 4>& buffer, uint8_t line) {
	size_t mapOffset = getWndwTileMapIndex() * TILE_MAP_TABLE_SIZE;
	std::array<uint8_t, 4> palette = getBgPalette();
	Point coords = getWindowCoords();

	if (coords.x >= SCREEN_WIDTH || coords.y > line) {
		return;
	}

	size_t y = size_t(line - coords.y);
	size_t row = y % TILESIZE;

	for (size_t x = size_t(coords.x); x < SCREEN_WIDTH; x++) {
		size_t col = x % TILESIZE;
		size_t mapNum = (y / TILESIZE) * LAYERSIZE + (x / TILESIZE);

		uint8_t rawTileIndex = maps[mapOffset + mapNum];
		size_t tileIndex;

		if (getBgWndwTileSetIndex() == 1) {
			tileIndex = rawTileIndex;
		}
		else {
			tileIndex = 256 + int8_t(rawTileIndex);
		}

		const Tile& tile = tiles[tileIndex];
		auto rowData = tile.getRow(row);

		uint8_t cell = rowData[col];
		uint8_t colorIdx = palette[cell];
		const auto& color = GB_PALETTE[colorIdx];

		size_t base = x * 4;
		buffer[base + 0] = color[0];
		buffer[base + 1] = color[1];
		buffer[base + 2] = color[2];
		buffer[base + 3] = color[4];
	}
}

void Ppu::renderSprites(std::array<uint8_t, SCREEN_WIDTH * 4>& buffer, uint8_t line) {
	auto sprites = sortSprites();
	auto bgPalette = getBgPalette();
	bool is8x16 = areSprites8x16();

	for (const Sprite& spr : sprites) {
		int height = is8x16 ? 16 : 6;
		Point coords = spr.getCoords();
		int signedLine = int8_t(line);

		if (signedLine < coords.y || coords.y + height <= signedLine) {
			continue;
		}

		auto palette = getSpritePalette(spr.usePallete1());
		bool behindBg = spr.getBgPriority();

		int y = signedLine - coords.y;
		bool yFlipped = spr.isYFlipped();

		uint8_t rawTile = spr.getTileNum();
		uint8_t sprIdx;

		if (is8x16) {
			if ((y < 8 && !yFlipped || y >= 8 && yFlipped)) {
				sprIdx = rawTile & 0xFE;
			}
			else {
				sprIdx = rawTile | 0x01;
			}
		}
		else {
			sprIdx = rawTile;
		}

		const Tile& tile = tiles[sprIdx];

		int screenY = y + coords.y;
		if (screenY < 0 || screenY >= SCREEN_HEIGHT) {
			continue;
		}

		int dataY = yFlipped ? (height - y - 1) : y;
		dataY %= 8;

		auto rowData = tile.getRow(size_t(dataY));

		for (int x = 0; x < 8; x++) {
			int dataX = spr.isXFlipped() ? (7 - x) : x;
			uint8_t cell = rowData[dataX];

			if (cell == 0) {
				continue;
			}

			int screenX = coords.x + x;
			if (screenX < 0 || screenX >= SCREEN_WIDTH) {
				continue;
			}

			size_t bufferIdx = size_t(screenX) * 4;

			const auto& currentRGBA = *reinterpret_cast<const std::array<uint8_t, 4>*>(&buffer[bufferIdx]);
			const auto& bgTransparent = GB_PALETTE[bgPalette[0]];

			if (behindBg && currentRGBA != bgTransparent) {
				continue;
			}

			uint8_t colorIdx = palette[cell];
			const auto& color = GB_PALETTE[colorIdx];
			buffer[bufferIdx + 0] = color[0];
			buffer[bufferIdx + 1] = color[1];
			buffer[bufferIdx + 2] = color[2];
			buffer[bufferIdx + 3] = color[3];
		}
	}
}

uint8_t Ppu::readOAM(uint16_t addr) const{
	uint16_t relativeAddr = addr - OAM_START;
	uint16_t oamIdx = relativeAddr / BYTES_PER_SPRITE;
	return oam[oamIdx].readU8(addr);
}

void Ppu::writeOAM(uint16_t addr, uint8_t val) {
	uint16_t relativeAddr = addr - OAM_START;
	uint16_t oamIdx = relativeAddr / BYTES_PER_SPRITE;
	oam[oamIdx].writeU8(addr, val);
}

bool Ppu::canWriteVRAM() const {
	return mode.mode != LcdModeType::VRAMReadMode;
}

std::vector<Sprite> Ppu::sortSprites() const {
	std::vector<Sprite> sprites(oam, oam + NUM_OAM_SPRITES);

	std::reverse(sprites.begin(), sprites.end());

	std::sort(sprites.begin(), sprites.end(),
		[](const Sprite& a, const Sprite& b) {
			return a.getCoords().x > b.getCoords().x;
		}
	);

	return sprites;
}

void Ppu::renderScanline() {

	uint8_t line = readLcdReg(LY);

	if (line >= SCREEN_HEIGHT) {
		return;
	}

	std::array<uint8_t, SCREEN_WIDTH * 4> row{};
	row.fill(0xFF);

	if (isBgLayerDisplayed()) {
		renderBg(row, line);
	}

	if (isWndwLayerDisplayed()) {
		renderWindow(row, line);
	}

	if (isSpriteLayerDisplayed()) {
		renderSprites(row, line);
	}

	size_t startIdx = size_t(line) * SCREEN_WIDTH * 4;
	size_t endIdx = size_t(line + 1) * SCREEN_WIDTH * 4;
	std::copy(row.begin(), row.end(), screenBuffer.begin() + startIdx);

	//dumpFramebuffer();
}

void Ppu::dumpFramebuffer() {
	std::cout << "Framebuffer dump (first 256 bytes):\n";
	for (int i = 0; i < 256; i++) {
		printf("%02X ", screenBuffer[i]);
		if ((i + 1) % 16 == 0)
			printf("\n");
	}
}

