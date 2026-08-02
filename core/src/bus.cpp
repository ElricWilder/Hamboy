#include "bus.h"
#include <cstdint>
#include <iostream>
#include "wram.h"

// MEMORY MAP
//Start	End	    Description	Notes
//0000	3FFF	16 KiB ROM bank 00	From cartridge, usually a fixed bank
//4000	7FFF	16 KiB ROM Bank 01–NN	From cartridge, switchable bank via mapper(if any)
//8000	9FFF	8 KiB Video RAM(VRAM)	In CGB mode, switchable bank 0 / 1
//A000	BFFF	8 KiB External RAM	From cartridge, switchable bank if any
//C000	CFFF	4 KiB Work RAM(WRAM)
//D000	DFFF	4 KiB Work RAM(WRAM)	In CGB mode, switchable bank 1–7
//E000	FDFF	Echo RAM(mirror of C000–DDFF)	Nintendo says use of this area is prohibited.
//FE00	FE9F	Object attribute memory(OAM)
//FEA0	FEFF	Not Usable	Nintendo says use of this area is prohibited.
//FF00	FF7F	I / O Registers
//FF80	FFFE	High RAM(HRAM)
//FFFF	FFFF	Interrupt Enable register (IE)

Bus::Bus()
    : cart(), ppu(), ram{}, wram(), hram()
{
}

void Bus::loadROM(const std::vector<uint8_t>& romData) {
	cart.loadCart(romData);
}

uint8_t Bus::readRAM(uint16_t addr) const {

    if (addr >= ROM_START && addr <= ROM_STOP) {
        return cart.readCart(addr);
    }

    else if (addr >= VRAM_START && addr <= VRAM_STOP) {
        return ppu.readVRAM(addr);
    }

    else if (addr >= EXT_RAM_START && addr <= EXT_RAM_STOP) {
        return cart.readRam(addr);
    }

    else if (addr >= WRAM_START && addr <= ECHO_STOP) {
        return wram.readU8(addr);
    }

    else if (addr >= OAM_START && addr <= OAM_STOP) {
        return ppu.readOAM(addr);
    }
    
    else if (addr >= IO_START && addr <= IO_STOP) {
        return io.readU8(addr);
    }

    else if (addr >= LCD_REG_START && addr <= LCD_REG_STOP) {
        return ppu.readLcdReg(addr);
    }

    else if (addr >= HRAM_START && addr <= HRAM_STOP) {
        uint16_t relativeAddr = addr - HRAM_START;
        return hram[size_t(relativeAddr)];
    }

    else {
        return 0;
    }
}

void Bus::pressButton(Buttons button, bool pressed) {
    io.setButton(button, pressed);
}

void Bus::writeRAM(uint16_t addr, uint8_t val) {

    if (addr >= ROM_START && addr <= ROM_STOP) {
        cart.writeCart(addr, val);
    }

    else if (addr >= VRAM_START && addr <= VRAM_STOP) {
        ppu.writeVRAM(addr, val);
    }

    else if (addr >= EXT_RAM_START && addr <= EXT_RAM_STOP) {
        cart.writeRam(addr, val);
    }

    else if (addr >= WRAM_START && addr <= ECHO_STOP) {
        wram.writeU8(addr, val);
    }

    else if (addr >= OAM_START && addr <= OAM_STOP) {
        return ppu.writeOAM(addr, val);
    }

    else if (addr >= IO_START && addr <= IO_STOP) {
        return io.writeU8(addr, val);
    }

    else if (addr >= LCD_REG_START && addr <= LCD_REG_STOP) {
        if (addr == OAM_DMA) {
            dmaTransfer(val);
        }
        ppu.writeLcdReg(addr, val);
    }

    else if (addr >= HRAM_START && addr <= HRAM_STOP) {
        uint16_t relativeAddr = addr - HRAM_START;
        hram[size_t(relativeAddr)] = val;
    }

    else {
        return;
    }
}

    PpuUpdateResult Bus::updatePPU(uint8_t cycles) {
	return ppu.update(cycles);
}

std::array<uint8_t, DISPLAY_BUFFER> Bus::render() {
	return ppu.render();
}

void Bus::dmaTransfer(uint8_t val) {
    uint16_t src = uint16_t(val) << 8;
    for (int i = 0; i < 0xA0; i++) {
        uint8_t temp = readRAM(src + i);
        writeRAM(OAM_START + i, temp);
    }
}

bool Bus::updateTimer(uint8_t cycles) {
    return io.updateTimer(cycles);
}

std::string Bus::getTitle() const{
    return cart.getTitle();
}

void Bus::renderScanline() {
    ppu.renderScanline();
}