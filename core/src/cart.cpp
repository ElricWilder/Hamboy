#include "cart.h"
#include "utils.h"
#include <algorithm>
#include <iostream>

Cart::Cart()
	: rom{},
	ram{},
	mbc(MBC::NONE),
	rtc(),
	romBank(1),
	ramBank(0),
	romMode(true),
	ramEnabled(false)
{
}


void Cart::loadCart(const std::vector<uint8_t>& romData) {
	rom = romData;
	mbc = getMBC();
	initExtRam();
}

uint8_t Cart::readCart(uint16_t addr) const {
	if (addr < ROM_BANK_SIZE) {
		return rom[addr];
	}
	else {
		uint16_t relativeAddr = addr - ROM_BANK_SIZE;
		uint16_t bankAddr = romBank * ROM_BANK_SIZE + relativeAddr;
		return rom[bankAddr];
	}
}

void Cart::writeCart(uint16_t addr, uint8_t val) {
	switch (mbc) {
	case MBC::NONE:
		return;
	case MBC::MBC1:
		mbc1WriteRom(addr, val);
		return;
	case MBC::MBC2:
		mbc2WriteRom(addr, val);
		return;
	case MBC::MBC3:
		mbc3WriteRom(addr, val);
		return;
	case MBC::MBC5:
		mbc5WriteRom(addr, val);
		return;
	default:
		return;
	}
}

std::string Cart::getTitle() const {
	std::string title(rom.begin() + TITLE_START, rom.begin() + TITLE_STOP);

	while (!title.empty() && title.back() == '\0') {
		title.pop_back();
	}

	return title;
}

Cart::MBC Cart::getMBC() const {
	uint8_t cartType = rom[CART_TYPE_ADDR];
	if (cartType == 0x00) {
		return MBC::NONE;
	}
	else if (cartType >= 0x01 && cartType <= 0x03) {
		return MBC::MBC1;
	}
	else if (cartType >= 0x05 && cartType <= 0x06) {
		return MBC::MBC2;
	}
	else if (cartType >= 0x0F && cartType <= 0x13) {
		return MBC::MBC3;
	}
	else if (cartType >= 0x19 && cartType <= 0x1E) {
		return MBC::MBC5;
	}
	else {
		return MBC::INV;
	}
}

bool Cart::hasBattery() const {
	uint8_t hasBattery[] = {
		0x03, 0x06, 0x09,
		0x0D, 0x0F, 0x10,
		0x13, 0x1B, 0x1E
	};

	uint8_t cartType = rom[CART_TYPE_ADDR];
	// check if cartType is in hasBattery and return result
	return (std::find(std::begin(hasBattery), std::end(hasBattery), cartType) != std::end(hasBattery));
}

bool Cart::hasExternalRam() const {
	uint8_t hasExtRam[] = {
		0x02, 0x03, 0x08,
		0x09, 0x0C, 0x0D,
		0x10, 0x12, 0x13,
		0x16, 0x17, 0x1A,
		0x1B, 0x1D, 0x1E
	};

	uint8_t cartType = rom[CART_TYPE_ADDR];
	return (std::find(std::begin(hasExtRam), std::end(hasExtRam), cartType) != std::end(hasExtRam));
}

void Cart::initExtRam() {
	size_t ramSizeIdx = size_t(rom[RAM_SIZE_ADDR]);
	if (hasExternalRam() && (ramSizeIdx == 0)) {
		ramSizeIdx = 1;
	}

	size_t ramSize = RAM_SIZES[ramSizeIdx] * 1024;
	ram.assign(ramSize, 0);
}

uint8_t Cart::readRam(uint16_t addr) const {
	if (!ramEnabled || ram.empty()) {
		return 0xFF;
	}

	if (mbc == MBC::NONE || mbc == MBC::MBC1 || mbc == MBC::MBC2 || mbc == MBC::MBC5) {
		readRamHelper(addr);
	}
	else if (mbc == MBC::MBC3) {
		mbc3ReadRam(addr);
	}
	else {
		return 0;
	}
}

void Cart::writeRam(uint16_t addr, uint8_t val) {
	if (!ramEnabled || ram.empty()) {
		return;
	}

	switch (mbc) {
	case MBC::NONE: {
			uint16_t relAddr = addr - EXT_RAM_START;
			ram[size_t(relAddr)] = val;
			return;
		}
	case MBC::MBC1:
		writeRamHelper(addr, val);
		return;
	case MBC::MBC2:
		writeRamHelper(addr, val);
		return;
	case MBC::MBC3:
		mbc3WriteRam(addr, val);
		return;
	case MBC::MBC5:
		writeRamHelper(addr, val);
		return;
	default:
		return;
	}
}

void Cart::mbc1WriteRom(uint16_t addr, uint8_t val) {
	if (addr >= RAM_ENABLE_START && addr <= RAM_ENABLE_STOP) {
		if (val == 0x0A) {
			ramEnabled = true;
		}
		else {
			ramEnabled = false;
		}
	}
	else if (addr >= ROM_BANK_NUM_START && addr <= ROM_BANK_NUM_STOP) {
		uint16_t bank = (val & 0x1F);
		if (bank == 0x00 || bank == 0x20 || bank == 0x40 || bank == 0x60) {
			romBank = bank + 1;
		}
		else {
			romBank = bank;
		}
	}
	else if (addr >= RAM_BANK_NUM_START && RAM_BANK_NUM_STOP) {
		uint16_t bits = (val & 0b11);

		if (romMode) {
			romBank |= (bits << 5);
		}
		else {
			ramBank = bits;
		}
	}
	else if (addr >= ROM_RAM_MODE_START && addr <= ROM_RAM_MODE_STOP) {
		if (val == 0) {
			romMode = true;
		}
		else {
			romMode = false;
		}
	}
}

void Cart::mbc2WriteRom(uint16_t addr, uint8_t val) {
	uint8_t bankSwap = get_bit8(MBC2_ROM_CONTROL_BIT, addr);
	if (bankSwap) {
		romBank = (val & 0x0F);
	}
	else {
		if (val == 0x0A) {
			ramEnabled = true;
		}
		else {
			ramEnabled = false;
		}
	}
}

void Cart::mbc3WriteRom(uint16_t addr, uint8_t val) {
	if (addr >= RAM_ENABLE_START && addr <= RAM_ENABLE_STOP) {
		if (val == 0x0A) {
			ramEnabled = true;
		}
		else {
			ramEnabled = false;
		}
	}
	else if (addr >= ROM_BANK_NUM_START && addr <= ROM_BANK_NUM_STOP) {
		if (val == 0x00) {
			romBank = 0x01;
		}
		else {
			romBank = val;
		}
	}
	else if (addr >= RAM_BANK_NUM_START && RAM_BANK_NUM_STOP) {
		ramBank = val;
	}
	else if (addr >= ROM_RAM_MODE_START && addr <= ROM_RAM_MODE_STOP) {
		rtc.writeByte(ramBank, val);
	}
	else {
		return;
	}
}

void Cart::mbc5WriteRom(uint16_t addr, uint8_t val) {
	if (addr >= RAM_ENABLE_START && addr <= RAM_ENABLE_STOP) {
		if (val == 0x0A) {
			ramEnabled = true;
		}
		else {
			ramEnabled = false;
		}
	}
	else if (addr >= ROM_BANK_LOW_START && addr <= ROM_BANK_LOW_STOP) {
		romBank &= 0xFF00;
		romBank |= uint16_t(val);
	}
	else if (addr >= ROM_BANK_HIGH_START && ROM_BANK_HIGH_STOP) {
		romBank = set_bit16(9, romBank, (val != 0));
	}
	else if (addr >= RAM_BANK_NUM_START && addr <= RAM_BANK_NUM_STOP) {
		ramBank = (val & 0x0F);
	}
	else {
		return;
	}
}

void Cart::writeRamHelper(uint16_t addr, uint8_t val) {
	if (ramEnabled) {
		uint16_t relativeAddr = addr - EXT_RAM_START;
		uint16_t ramAddr = ramBank * RAM_BANK_SIZE + relativeAddr;
		ram[ramAddr] = val;
	}
}

void Cart::mbc3WriteRam(uint16_t addr, uint8_t val) {
	if (ramBank >= 0x00 && ramBank <= 0x03) {
		writeRamHelper(addr, val);
	}
	else if (ramBank >= 0x08 && ramBank <= 0x0C) {
		if (ramEnabled) {
			rtc.writeByte(ramBank, val);
		}
	}
	else {
		return;
	}
}

uint8_t Cart::mbc3ReadRam(uint16_t addr) const{
	if (rtc.isEnabled() && (ramBank >= 0x08 && ramBank <= 0x0C)) {
		return rtc.readByte(ramBank);
	}
	else {
		return readRamHelper(addr);
	}
}

uint8_t Cart::readRamHelper(uint16_t addr) const{
	uint16_t relativeAddr = addr - EXT_RAM_START;
	uint16_t bankAddr = ramBank * RAM_BANK_SIZE + relativeAddr;
	return ram[bankAddr];
}