#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>
#include "tile.h"
#include "modes.h"
#include "sprite.h"
#include "utils.h"

static constexpr uint16_t VRAM_START = 0x8000;
static constexpr uint16_t VRAM_STOP = 0x9FFF;

static constexpr uint16_t TILE_SET_START = 0x8000;
static constexpr uint16_t TILE_SET_STOP = 0x97FF;
static constexpr uint16_t TILE_MAP_START = 0x9800;
static constexpr uint16_t TILE_MAP_STOP = 0x9FFF;

static constexpr uint16_t TILESIZE = 8;
static constexpr uint16_t LAYERSIZE = 32;
static constexpr uint16_t MAP_PIXELS = TILESIZE * LAYERSIZE;

static constexpr size_t NUM_TILE_COLS = SCREEN_WIDTH / 8;
static constexpr size_t NUM_TILE_ROWS = SCREEN_HEIGHT / 8;

static constexpr size_t LAYER_WIDTH = 32;
static constexpr uint16_t BYTES_PER_TILE = 16;
static constexpr size_t NUM_TILES = 384;
static constexpr size_t TILE_MAP_SIZE = (TILE_MAP_STOP - TILE_MAP_START + 1);
static constexpr uint16_t TILE_MAP_TABLE_SIZE = TILE_MAP_SIZE / 2;

static constexpr uint16_t LCD_REG_START = 0xFF40;
static constexpr uint16_t LCD_REG_STOP = 0xFF4B;
static constexpr size_t LCD_REG_SIZE = (LCD_REG_STOP - LCD_REG_START + 1);

// PPU Control Registers
static constexpr uint16_t LCDC = 0xFF40;
static constexpr uint16_t STAT = 0xFF41;
static constexpr uint16_t SCY = 0xFF42;
static constexpr uint16_t SCX = 0xFF43;
static constexpr uint16_t LY = 0xFF44;
static constexpr uint16_t LYC = 0xFF45;
static constexpr uint16_t BGP = 0xFF47;
static constexpr uint16_t OBP0 = 0xFF48;
static constexpr uint16_t OBP1 = 0xFF49;
static constexpr uint16_t WY = 0xFF4A;
static constexpr uint16_t WX = 0xFF4B;

// LCDC bit flags
static constexpr uint8_t LCDC_LCD_ENABLED_BIT = 7;
static constexpr uint8_t LCDC_WNDW_MAP_BIT = 6;
static constexpr uint8_t LCDC_WNDW_ENABLED_BIT = 5;
static constexpr uint8_t LCDC_BG_WNDW_TILE_BIT = 4;
static constexpr uint8_t LCDC_BG_MAP_BIT = 3;
static constexpr uint8_t LCDC_SPR_SIZE_BIT = 2;
static constexpr uint8_t LCDC_SPR_ENABLED_BIT = 1;
static constexpr uint8_t LCDC_BG_WNDW_ENABLED_BIT = 0;

// STAT bit flags
static constexpr uint8_t STAT_LY_LYC_IRQ_BIT = 6;
static constexpr uint8_t STAT_OAM_IRQ_BIT = 5;
static constexpr uint8_t STAT_VBLANK_IRQ_BIT = 4;
static constexpr uint8_t STAT_HBLANK_IRQ_BIT = 3;
static constexpr uint8_t STAT_LY_EQ_LYC_BIT = 2;

static constexpr uint16_t OAM_START = 0xFE00;
static constexpr uint16_t OAM_STOP = 0xFE9F;
static constexpr size_t NUM_OAM_SPRITES = 40;
static constexpr uint16_t BYTES_PER_SPRITE = 4;

struct PpuUpdateResult {
	LcdResults lcdResult;
	bool irq;
	bool vblankIrq;
};

struct Ppu {
	std::array<uint8_t, DISPLAY_BUFFER> screenBuffer = {};
	Lcd mode;
	Tile tiles[NUM_TILES] = {};
	uint8_t maps[TILE_MAP_SIZE] = {};
	uint8_t lcdRegs[LCD_REG_SIZE] = {};
	Sprite oam[NUM_OAM_SPRITES] = {};

	Ppu();

	uint8_t readVRAM(uint16_t addr) const;
	void writeVRAM(uint16_t addr, uint8_t val);
	PpuUpdateResult update(uint8_t cycles);

	uint8_t readLcdReg(uint16_t addr) const;
	void writeLcdReg(uint16_t addr, uint8_t val);

	bool isLcdEnabled() const;
	uint8_t getWndwTileMapIndex() const;
	bool isWndwLayerDisplayed();
	uint8_t getBgWndwTileSetIndex() const;
	uint8_t getBgTileMapIndex() const;
	bool areSprites8x16() const;
	bool isSpriteLayerDisplayed();
	bool isBgLayerDisplayed();

	Point getViewportCoords() const;
	Point getWindowCoords() const;

	std::array<uint8_t, 4> getBgPalette() const;
	std::array<uint8_t, 4> getSpritePalette(uint8_t index) const;

	std::array<uint8_t, DISPLAY_BUFFER> render() const;
	void renderBg(std::array<uint8_t, SCREEN_WIDTH * 4>& buffer, uint8_t line);
	void renderWindow(std::array<uint8_t, SCREEN_WIDTH * 4>& buffer, uint8_t line);

	uint8_t readOAM(uint16_t addr) const;
	void writeOAM(uint16_t addr, uint8_t val);

	bool canWriteVRAM() const;

	void renderSprites(std::array<uint8_t, SCREEN_WIDTH * 4>& buffer, uint8_t line);
	std::vector<Sprite> sortSprites() const;

	void renderScanline();
	void dumpFramebuffer();
};