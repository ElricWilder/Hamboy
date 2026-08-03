#pragma once

#include "rtc.h"

#include <vector>
#include <string>
#include <cstdint>

constexpr uint16_t ROM_START = 0x0000;
constexpr uint16_t ROM_STOP = 0x7FFF;
constexpr size_t TITLE_START = 0x0134;
constexpr size_t TITLE_STOP = 0x0142;
constexpr size_t CART_TYPE_ADDR = 0x0147;
constexpr size_t RAM_SIZE_ADDR = 0x0149;

constexpr uint16_t EXT_RAM_START = 0xA000;
constexpr uint16_t EXT_RAM_STOP = 0xBFFF;

constexpr size_t RAM_BANK_SIZE = 0x2000;
constexpr size_t ROM_BANK_SIZE = 0x4000;

constexpr uint16_t RAM_ENABLE_START = 0x0000;
constexpr uint16_t RAM_ENABLE_STOP = 0x1FFF;
constexpr uint16_t ROM_BANK_NUM_START = 0x2000;
constexpr uint16_t ROM_BANK_NUM_STOP = 0x3FFF;
constexpr uint16_t RAM_BANK_NUM_START = 0x4000;
constexpr uint16_t RAM_BANK_NUM_STOP = 0x5FFF;
constexpr uint16_t ROM_RAM_MODE_START = 0x6000;
constexpr uint16_t ROM_RAM_MODE_STOP = 0x7FFF;

constexpr uint16_t ROM_BANK_LOW_START = 0x2000;
constexpr uint16_t ROM_BANK_LOW_STOP = 0x2FFF;
constexpr uint16_t ROM_BANK_HIGH_START = 0x3000;
constexpr uint16_t ROM_BANK_HIGH_STOP = 0x3FFF;

constexpr uint8_t MBC2_ROM_CONTROL_BIT = 8;

const size_t RAM_SIZES[] = {
	0,
	2,
	8,
	32,
	128,
	64
};

struct Cart {

	enum class MBC {
		NONE,
		MBC1,
		MBC2,
		MBC3,
		MBC5,
		INV,
	};

	std::vector<uint8_t> rom;
	std::vector<uint8_t> ram;
	MBC mbc;
	Rtc rtc;
	uint16_t romBank;
	uint8_t ramBank;
	bool romMode;
	bool ramEnabled;

	// constructor
	Cart();

	void loadCart(const std::vector<uint8_t>& romData);
	uint8_t readCart(uint16_t addr) const;
	void writeCart(uint16_t addr, uint8_t val);
	std::string getTitle() const;
	MBC getMBC() const;
	bool hasBattery() const;
	bool hasExternalRam() const;
	void initExtRam();
	uint8_t readRam(uint16_t addr) const;
	void writeRam(uint16_t addr, uint8_t val);
	
	void mbc1WriteRom(uint16_t addr, uint8_t val);
	void mbc2WriteRom(uint16_t addr, uint8_t val);
	void mbc3WriteRom(uint16_t addr, uint8_t val);
	void mbc5WriteRom(uint16_t addr, uint8_t val);
	void writeRamHelper(uint16_t addr, uint8_t val);
	uint8_t mbc3ReadRam(uint16_t addr) const;
	void mbc3WriteRam(uint16_t addr, uint8_t val);
	uint8_t readRamHelper(uint16_t addr) const;
};