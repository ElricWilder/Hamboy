#include "ppu.h"
#include "utils.h"
#include "tile.h"
#include "cpu.h"
#include <stdexcept>
#include <iostream>

Ppu::Ppu()
	: screenBuffer{}, mode(), tiles{}, maps{}, lcdRegs{}, oam{}, bgColorIndexLine{}, windowLineCounter{}
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
	uint8_t lcdc;
	bool wndEnabled;
	uint8_t wy;
	uint8_t wx;
	uint8_t wxStart;
	bool wndActiveHoriz;

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
		if (newLine == 0) {
			windowLineCounter = 0;
		}
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
		case LcdModeType::VRAMReadMode:
			lcdc = readLcdReg(LCDC);
			wndEnabled =
				get_bit8(LCDC_WNDW_ENABLED_BIT, lcdc) &&
				get_bit8(LCDC_BG_WNDW_ENABLED_BIT, lcdc);

			wy = readLcdReg(WY);
			wx = readLcdReg(WX);
			wxStart = (wx >= 7 ? wx - 7 : 0);
			wndActiveHoriz = (wxStart < SCREEN_WIDTH);

			if (wndEnabled && newLine >= wy && wndActiveHoriz) {
				windowLineCounter++;
			}
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
		return unpack_u8(readLcdReg(OBP1));
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
	std::array<uint8_t, 4> palette = getBgPalette();
	Point viewport = getViewportCoords();

	size_t currentY = size_t(viewport.y) + line;
	size_t y = currentY % MAP_PIXELS;
	size_t tileRow = (y / TILESIZE);
	size_t rowInTile = (y % TILESIZE);

	size_t mapOffset = getBgTileMapIndex() * TILE_MAP_TABLE_SIZE;

	for (size_t px = 0; px < SCREEN_WIDTH; ) {
		size_t currentX = size_t(viewport.x) + px;
		size_t x = currentX % MAP_PIXELS;
		size_t tileCol = (x / TILESIZE);

		size_t mapNum = tileRow * LAYERSIZE + tileCol;
		uint8_t rawTileIndex = maps[mapOffset + mapNum];

		size_t tileIndex;
		int8_t signedIndex = static_cast<int8_t>(rawTileIndex);

		if (getBgWndwTileSetIndex() == 1) {
			tileIndex = rawTileIndex; // unsigned
		}
		else {
			tileIndex = 256 + signedIndex; // signed
		}

		const Tile& tile = tiles[tileIndex];
		auto rowData = tile.getRow(rowInTile);

		for (int i = 0; i < 8; ++i) {
			size_t screenX = px + i;
			if (screenX >= SCREEN_WIDTH) {
				break;
			}

			uint8_t cell = rowData[i];
			bgColorIndexLine[screenX] = cell;
			uint8_t colorIdx = palette[cell];
			const auto& color = GB_PALETTE[colorIdx];

			size_t base = screenX * 4;

			buffer[base + 0] = color[0];
			buffer[base + 1] = color[1];
			buffer[base + 2] = color[2];
			buffer[base + 3] = color[3];
		}
		px += TILESIZE;
	}
}

void Ppu::renderWindow(std::array<uint8_t, SCREEN_WIDTH * 4>& buffer, uint8_t line) {
	size_t mapOffset = getWndwTileMapIndex() * TILE_MAP_TABLE_SIZE;
	std::array<uint8_t, 4> palette = getBgPalette();
	Point coords = getWindowCoords();

	if (coords.x >= SCREEN_WIDTH || coords.y > line) {
		return;
	}

	size_t winY = windowLineCounter;
	size_t tileRow = winY / TILESIZE;
	size_t rowInTile = winY % TILESIZE;

	for (size_t px = size_t(coords.x); px < SCREEN_WIDTH; ) {
		size_t winX = px - coords.x;
		size_t tileCol = winX / TILESIZE;

		size_t mapNum = tileRow * LAYERSIZE + tileCol;
		uint8_t rawTileIndex = maps[mapOffset + mapNum];

		size_t tileIndex;
		int8_t signedIndex = static_cast<int8_t>(rawTileIndex);

		if (getBgWndwTileSetIndex() == 1) {
			tileIndex = rawTileIndex;
		}
		else {
			tileIndex = 256 + signedIndex;
		}

		const Tile& tile = tiles[tileIndex];
		auto rowData = tile.getRow(rowInTile);

		for (int i = 0; i < 8; ++i) {
			size_t screenX = px + i;
			if (screenX >= SCREEN_WIDTH) {
				break;
			}

			uint8_t cell = rowData[i];
			bgColorIndexLine[screenX] = cell;
			uint8_t colorIdx = palette[cell];
			const auto& color = GB_PALETTE[colorIdx];

			size_t base = screenX * 4;

			buffer[base + 0] = color[0];
			buffer[base + 1] = color[1];
			buffer[base + 2] = color[2];
			buffer[base + 3] = color[3];
		}

		px += TILESIZE;
	}
}

void Ppu::renderSprites(std::array<uint8_t, SCREEN_WIDTH * 4>& buffer, uint8_t line) {
	auto sprites = getSpritesForLine(line);

	std::sort(sprites.begin(), sprites.end(),
		[](const SpriteEntry& a, const SpriteEntry& b) {
			int ax = a.spr.getCoords().x;
			int bx = b.spr.getCoords().x;

			if (ax != bx) {
				return ax > bx;
			}

			return a.oamIndex > b.oamIndex;
		}
	);

	auto bgPalette = getBgPalette();
	bool is8x16 = areSprites8x16();

	for (const SpriteEntry& entry : sprites) {
		const Sprite& spr = entry.spr;
		int height = is8x16 ? 16 : 8;
		Point coords = spr.getCoords();
		int signedLine = line;

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
			int tileRow = y / 8;
			if (yFlipped) {
				tileRow = 1 - tileRow;
			}
			sprIdx = (rawTile & 0xFE) + tileRow;
		}
		else {
			sprIdx = rawTile;
		}

		const Tile& tile = tiles[sprIdx];

		int screenY = y + coords.y;
		if (screenY < 0 || screenY >= SCREEN_HEIGHT) {
			continue;
		}

		int yInSprite = y;
		int rowInTile = yInSprite % 8;

		if (yFlipped) {
			rowInTile = 7 - rowInTile;
		}

		auto rowData = tile.getRow(size_t(rowInTile));

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
			uint8_t bgCell = bgColorIndexLine[screenX];
			uint8_t bgColorIndex = bgPalette[bgCell];

			bool spriteTransparent = (cell == 0);

			if (spriteTransparent) {
				continue;
			}

			if (behindBg && bgColorIndex != 0) {
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

std::vector<SpriteEntry> Ppu::getSpritesForLine(uint8_t line) const {
	std::vector<SpriteEntry> result;
	result.reserve(10);

	bool is8x16 = areSprites8x16();

	for (int i = 0; i < NUM_OAM_SPRITES; i++) {
		const Sprite& spr = oam[i];
		Point coords = spr.getCoords();
		int height = is8x16 ? 16 : 8;

		if (line >= coords.y && line < coords.y + height) {
			result.push_back(SpriteEntry{spr, i });

			if (result.size() == 10) {
				break;
			}
		}
	}
	
	return result;
}

void Ppu::renderScanline() {

	uint8_t line = readLcdReg(LY);

	if (line >= SCREEN_HEIGHT) {
		return;
	}

	std::array<uint8_t, SCREEN_WIDTH * 4> row{};
	row.fill(0xFF);
	std::fill(bgColorIndexLine.begin(), bgColorIndexLine.end(), 0);

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

