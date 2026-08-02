#include <cstdint>
#include "cpu.h"
#include "utils.h"
#include <stdexcept>
#include <vector>
#include <optional>
#include "opcodes.h"
#include "io.h"
#include <iostream>
#include "logger.h"
#include "timer.h"

Cpu::Cpu() {
	pc = 0x0100;
	sp = 0xFFFE;
	a = 0x01;
	b = 0x00;
	c = 0x13;
	d = 0x00;
	e = 0xD8;
	f = 0xB0;
	h = 0x01;
	l = 0x4D;
	irq_enabled = false;
	irq_enabled_scheduled = false;
	halted = false;
	bus = Bus();
	totalCycles = 0;

	// Magic values
	writeRAM(0xFF10, 0x80);
	writeRAM(0xFF11, 0xBF);
	writeRAM(0xFF12, 0xF3);
	writeRAM(0xFF14, 0xBF);
	writeRAM(0xFF16, 0x3F);
	writeRAM(0xFF19, 0xBF);
	writeRAM(0xFF1A, 0x7F);
	writeRAM(0xFF1B, 0xFF);
	writeRAM(0xFF1C, 0x9F);
	writeRAM(0xFF1E, 0xBF);
	writeRAM(0xFF20, 0xFF);
	writeRAM(0xFF23, 0xBF);
	writeRAM(0xFF24, 0x77);
	writeRAM(0xFF25, 0xF3);
	writeRAM(0xFF26, 0xF1); // 0xF0 for SGB
	writeRAM(0xFF40, 0x91);
	writeRAM(0xFF47, 0xFC);
	writeRAM(0xFF48, 0xFF);
	writeRAM(0xFF49, 0xFF);

}

uint8_t Cpu::get_r8(Regs8 r) const {
	switch (r) {
		case Regs8::A: return a;
		case Regs8::B: return b;
		case Regs8::C: return c;
		case Regs8::D: return d;
		case Regs8::E: return e;
		case Regs8::F: return f;
		case Regs8::H: return h;
		case Regs8::L: return l;
		case Regs8::HL:
			uint16_t addr = get_r16(Regs16::HL);
			return readRAM(addr);
			break;
	}
	return 0;
}

uint16_t Cpu::get_r16(Regs16 r) const {
	switch (r) {
		case Regs16::AF: return merge_bytes(a, f);
		case Regs16::BC: return merge_bytes(b, c);
		case Regs16::DE: return merge_bytes(d, e);
		case Regs16::HL: return merge_bytes(h, l);
		case Regs16::SP: return sp;
	}
	return 0;
}

void Cpu::set_r8(Regs8 r, uint8_t val) {
	switch (r) {
		case Regs8::A: a = val; break;
		case Regs8::B: b = val; break;
		case Regs8::C: c = val; break;
		case Regs8::D: d = val; break;
		case Regs8::E: e = val; break;
		// The bottom 4 bits of F shall always be 0
		case Regs8::F: f = val & 0xF0; break;
		case Regs8::H: h = val; break;
		case Regs8::L: l = val; break;
		case Regs8::HL: 
			uint16_t addr = get_r16(Regs16::HL);
			writeRAM(addr, val);
			break;
	}
}

void Cpu::set_r16(Regs16 r, uint16_t val) {
	uint8_t low = getLowByte(val);
	uint8_t high = getHighByte(val);

	switch (r) {
		case Regs16::AF: {
			set_r8(Regs8::A, high);
			set_r8(Regs8::F, low);
			break;
		}
		case Regs16::BC: {
			set_r8(Regs8::B, high);
			set_r8(Regs8::C, low);
			break;
		}
		case Regs16::DE: {
			set_r8(Regs8::D, high);
			set_r8(Regs8::E, low);
			break;
		}
		case Regs16::HL: {
			set_r8(Regs8::H, high);
			set_r8(Regs8::L, low);
			break;
		}
		case Regs16::SP: sp = val; break;
	}
}

bool Cpu::getFlag(Flags f) const {
	switch (f) {
		case Flags::Z: return ((this->f & 0b1000'0000) != 0);
		case Flags::N: return ((this->f & 0b0100'0000) != 0);
		case Flags::H: return ((this->f & 0b0010'0000) != 0);
		case Flags::C: return ((this->f & 0b0001'0000) != 0);
	}
	return false;
}

void Cpu::setFlag(Flags f, bool val) {
	if (val) {
		switch (f) {
			case Flags::Z: this->f |= 0b1000'0000; break;
			case Flags::N: this->f |= 0b0100'0000; break;
			case Flags::H: this->f |= 0b0010'0000; break;
			case Flags::C: this->f |= 0b0001'0000; break;
		}
	}
	else {
		switch (f) {
			case Flags::Z: this->f &= 0b0111'0000; break;
			case Flags::N: this->f &= 0b1011'0000; break;
			case Flags::H: this->f &= 0b1101'0000; break;
			case Flags::C: this->f &= 0b1110'0000; break;
		}
	}
}

uint8_t Cpu::fetch8() {
	uint8_t val = readRAM(pc);
	pc++;
	return val;
}

uint16_t Cpu::fetch16() {
	uint8_t low = fetch8();
	uint8_t high = fetch8();
	uint16_t val = merge_bytes(high, low);
	return val;
}

uint8_t Cpu::readRAM(uint16_t addr) const {
	// std::cout << "pc is: " << std::hex<< (int)pc << "\n";
	uint8_t val = bus.readRAM(addr);
	// std::cout << "val is: " << std::hex << int(val) << "\n";
	return val;
}

void Cpu::writeRAM(uint16_t addr, uint8_t val) {
	if (addr == 0xFF43) {
		printf("SCX WRITE: %02X (PC=%04X)\n", val, pc);
	}
	bus.writeRAM(addr, val);
}

void Cpu::loadROM(const std::vector<uint8_t>& rom) {
	bus.loadROM(rom);
}

uint8_t Cpu::execute() {
	uint8_t opcode = fetch8();
	Opcode fn = opcodes[opcode];
	return (this->*fn)();
}

bool Cpu::tick() {
	bool drawTime = false;

	//printf("PC=%04X IME=%d HALT=%d\n", pc, irq_enabled, halted);

	if (irq_enabled_scheduled) {
		irq_enabled = true;
		irq_enabled_scheduled = false;
	}

	uint8_t ifReg = readRAM(IF);
	uint8_t ieReg = readRAM(IE);
	uint8_t irqFlags = ifReg & ieReg;

	if (irqFlags != 0) {
		halted = false;
	}

	uint8_t cycles = halted ? 1 : execute();

	auto ppuResult = bus.updatePPU(cycles);

	if (ppuResult.irq) {
		enableIrqType(Interrupts::Stat, true);
	}

	switch (ppuResult.lcdResult) {
	case LcdResults::RenderFrame:
		bus.renderScanline();
		enableIrqType(Interrupts::Vblank, true);
		drawTime = true;
		break;
	case LcdResults::RenderLine:
		bus.renderScanline();
		break;
	default:
		break;
	}

	bool timerIrq = bus.updateTimer(cycles);
	if (timerIrq) {
		enableIrqType(Interrupts::Timer, true);
	}

	if (irq_enabled) {
		if (auto irq = checkIrq()) {
			triggerIrq(*irq);
		}
	}

	return drawTime;
}

std::optional<Cpu::Interrupts> Cpu::checkIrq() {
	uint8_t ifReg = readRAM(IF);
	uint8_t ieReg = readRAM(IE);
	uint8_t irqFlags = ifReg & ieReg;

	if (!irq_enabled) {
		return std::nullopt;
	}

	for (int i = 0; i < 5; i++) {
		if (irqFlags & (1 << i)) {
			return IRQ_PRIORITIES[i];
		}
	}

	return std::nullopt;
}

void Cpu::enableIrqType(Cpu::Interrupts irq, bool enabled) {
	uint8_t ifReg = readRAM(IF);
	switch (irq) {
		case Interrupts::Vblank:
			ifReg = set_bit8(0, ifReg, enabled);
			break;
		case Interrupts::Stat:
			ifReg = set_bit8(1, ifReg, enabled);
			break;
		case Interrupts::Timer:
			ifReg = set_bit8(2, ifReg, enabled);
			break;
		case Interrupts::Serial:
			ifReg = set_bit8(3, ifReg, enabled);
			break;
		case Interrupts::Joypad:
			ifReg = set_bit8(4, ifReg, enabled);
			break;
		default:
			break;
	}
	
	writeRAM(IF, ifReg);
}

void Cpu::triggerIrq(Cpu::Interrupts irq) {
	halted = false;

	if (irq_enabled) {
		irq_enabled = false;
		irq_enabled_scheduled = false;

		uint16_t vector = getVector(irq);
		push(pc);
		setPC(vector);

		uint8_t ifReg(readRAM(IF));
		ifReg &= ~(1 << static_cast<int>(irq));

		writeIF_raw(ifReg);
	}
}

// Helper functions for inc/dec 16-bit opcodes
void Cpu::dec_r16(Regs16 r) {
	uint16_t val = get_r16(r);
	val--;
	set_r16(r, val);
}

void Cpu::inc_r16(Regs16 r) {
	uint16_t val = get_r16(r);
	val++;
	set_r16(r, val);
}

void Cpu::dec_r8(Regs8 r) {
	uint8_t val = get_r8(r);
	bool set_h = check_h_borrow_u8(val, 1);
	val--;

	set_r8(r, val);
	setFlag(Flags::N, true);
	setFlag(Flags::Z, val == 0);
	setFlag(Flags::H, set_h);
}

void Cpu::inc_r8(Regs8 r) {
	uint8_t val = get_r8(r);
	bool set_h = check_h_carry_u8(val, 1);
	val++;

	set_r8(r, val);
	setFlag(Flags::N, false);
	setFlag(Flags::Z, val == 0);
	setFlag(Flags::H, set_h);
}

void Cpu::and_a_u8(uint8_t val) {
	uint8_t a = get_r8(Regs8::A);
	a &= val;

	set_r8(Regs8::A, a);
	setFlag(Flags::Z, (a == 0));
	setFlag(Flags::N, false);
	setFlag(Flags::H, true);
	setFlag(Flags::C, false);
}

void Cpu::or_a_u8(uint8_t val) {
	uint8_t a = get_r8(Regs8::A);
	a |= val;

	set_r8(Regs8::A, a);
	setFlag(Flags::Z, (a == 0));
	setFlag(Flags::N, false);
	setFlag(Flags::H, false);
	setFlag(Flags::C, false);
}

void Cpu::xor_a_u8(uint8_t val) {
	uint8_t a = get_r8(Regs8::A);
	a ^= val;

	set_r8(Regs8::A, a);
	setFlag(Flags::Z, (a == 0));
	setFlag(Flags::N, false);
	setFlag(Flags::H, false);
	setFlag(Flags::C, false);
}

void Cpu::add_a_u8(uint8_t val, bool adc) {
	uint8_t carry = 0;

	// Check if this is an add or add w/ carry operation
	if (adc && getFlag(Flags::C)) {
		carry = 1;
	}

	uint8_t a = get_r8(Regs8::A);

	uint16_t sum1 = uint16_t(a) + uint16_t(val);
	uint8_t result1 = uint8_t(sum1 & 0xFF);
	bool h_check1 = check_h_carry_u8(a, val);
	bool carry1 = sum1 > 0xFF;

	uint16_t sum2 = uint16_t(result1) + uint16_t(carry);
	uint8_t result2 = uint8_t(sum2 & 0xFF);
	bool h_check2 = check_h_carry_u8(result1, carry);
	bool carry2 = sum2 > 0xFF;

	bool setH = h_check1 || h_check2;
	bool setC = carry1 || carry2;

	setFlag(Flags::N, false);
	setFlag(Flags::C, setC);
	setFlag(Flags::H, setH);
	setFlag(Flags::Z, result2 == 0);

	set_r8(Regs8::A, result2);
}

void Cpu::add_r16(Cpu::Regs16 dst_r, Cpu::Regs16 src_r) {
	uint16_t dst = get_r16(dst_r);
	uint16_t src = get_r16(src_r);

	uint32_t sum = uint32_t(dst) + uint32_t(src);
	uint16_t result = uint16_t(sum & 0xFFFF);

	bool carry = sum > 0xFFFF;
	bool setH = check_h_carry_u16(dst, src);

	setFlag(Flags::N, false);
	setFlag(Flags::H, setH);
	setFlag(Flags::C, carry);

	set_r16(dst_r, result);
}

void Cpu::sub_a_u8(uint8_t val, bool sbc) {
	uint8_t carry = 0;

	// Check if this is an add or add w/ carry operation
	if (sbc && getFlag(Flags::C)) {
		carry = 1;
	}

	uint8_t a = get_r8(Regs8::A);

	uint16_t diff = uint16_t(a) - uint16_t(val) - uint16_t(carry);
	uint8_t result = uint8_t(diff & 0xFF);

	bool half = ((a & 0xF) < ((val & 0xF) + carry));
	bool borrow = a < (val + carry);

	setFlag(Flags::N, true);
	setFlag(Flags::C, borrow);
	setFlag(Flags::H, half);
	setFlag(Flags::Z, result == 0);

	set_r8(Regs8::A, result);
}

void Cpu::cp_a_u8(uint8_t val) {
	uint8_t a = get_r8(Regs8::A);
	bool setH = check_h_borrow_u8(a, val);

	setFlag(Flags::N, true);
	setFlag(Flags::C, a < val);
	setFlag(Flags::H, setH);
	setFlag(Flags::Z, a == val);
}

uint16_t Cpu::pop() {

	uint8_t low = readRAM(sp);
	sp++;
	uint8_t high = readRAM(sp);
	sp++;
	return merge_bytes(high, low);

}

void Cpu::push(uint16_t val) {
	uint8_t high = getHighByte(val);
	uint8_t low = getLowByte(val);

	sp--;
	writeRAM(sp, high);
	sp--;
	writeRAM(sp, low);
}

uint16_t Cpu::getPC() {
	return pc;
}

void Cpu::setPC(uint16_t val) {
	pc = val;
}

bool Cpu::getBit(Regs8 r, uint8_t bit) {
	uint8_t val = get_r8(r);
	return get_bit8(bit, val);
}

void Cpu::setBit(Regs8 r, uint8_t bit, bool set) {
	uint8_t val = get_r8(r);
	val = set_bit8(bit, val, set);
	set_r8(r, val);
}

Cpu::Regs8 Cpu::get_cb_reg(uint8_t op) {
	switch (op & 0b111) {
	case 0:
		return Cpu::Regs8::B;
	case 1:
		return Cpu::Regs8::C;
	case 2:
		return Cpu::Regs8::D;
	case 3:
		return Cpu::Regs8::E;
	case 4:
		return Cpu::Regs8::H;
	case 5:
		return Cpu::Regs8::L;
	case 6:
		return Cpu::Regs8::HL;
	case 7:
		return Cpu::Regs8::A;
	}
	throw std::runtime_error("Invalid CB register");
}

uint8_t Cpu::execute_cb(uint8_t op) {
	// 0x00-0x07 -> RLC
	// 0x08-0x0F -> RRC
	// 0x10-0x17 -> RL
	// 0x18-0x1F -> RR
	// 0x20-0x27 -> SLA
	// 0x28-0x2F -> SRA
	// 0x30-0x37 -> SWAP
	// 0x38-0x3F -> SRL
	// 0x40-0x7F -> BIT
	// 0x80-0xBF -> RES
	// 0xC0-0xFF -> SET

	Cpu::Regs8 cb_reg = get_cb_reg(op);

	if (op <= 0x07) {
		rotate_left(cb_reg, false);
	}
	else if (op <= 0x0F) {
		rotate_right(cb_reg, false);
	}
	else if (op <= 0x17) {
		rotate_left(cb_reg, true);
	}
	else if (op <= 0x1F) {
		rotate_right(cb_reg, true);
	}
	else if (op <= 0x27) {
		shift_left(cb_reg);
	}
	else if (op <= 0x2F) {
		shift_right(cb_reg, true);
	}
	else if (op <= 0x37) {
		swap_bits(cb_reg);
	}
	else if (op <= 0x3F) {
		shift_right(cb_reg, false);
	}
	else if (op <= 0x7F) {
		uint8_t bit = (op >> 3) & 0x07;
		test_bit(cb_reg, bit);
	}
	else if (op <= 0xBF) {
		uint8_t bit = (op >> 3) & 0x07;
		write_bit(cb_reg, bit, false);
	}
	else if (op <= 0xFF) {
		uint8_t bit = (op >> 3) & 0x07;
		write_bit(cb_reg, bit, true);
	}
	else {
		throw std::runtime_error("Invalid CB opcode");
	}

	return 2;
}

void Cpu::rotate_left(Regs8 r, bool carry) {
	uint8_t val = get_r8(r);
	bool oldCarry = getFlag(Flags::C);

	bool msb = get_bit8(7, val);

	uint8_t result;

	if (carry) {
		result = (val << 1) | (oldCarry ? 0x01 : 0x00);
	}
	else {
		result = (val << 1) | (msb ? 0x01 : 0x00);
	}

	set_r8(r, result);
	setFlag(Flags::Z, result == 0);
	setFlag(Flags::N, false);
	setFlag(Flags::H, false);
	setFlag(Flags::C, msb);
}

void Cpu::rotate_right(Regs8 r, bool carry) {
	uint8_t val = get_r8(r);
	bool oldCarry = getFlag(Flags::C);
	bool lsb = get_bit8(0, val);

	uint8_t result;

	if (carry) {
		result = (val >> 1) | (oldCarry ? 0x80 : 0x00);
	}
	else {
		result = (val >> 1) | (lsb ? 0x80 : 0x00);
	}

	set_r8(r, result);
	setFlag(Flags::Z, result == 0);
	setFlag(Flags::N, false);
	setFlag(Flags::H, false);
	setFlag(Flags::C, lsb);
}

void Cpu::shift_left(Regs8 r) {
	uint8_t val = get_r8(r);
	bool msb = get_bit8(7, val);
	uint8_t newVal = val << 1;

	set_r8(r, newVal);
	setFlag(Flags::Z, newVal == 0);
	setFlag(Flags::N, false);
	setFlag(Flags::H, false);
	setFlag(Flags::C, msb);
}

void Cpu::shift_right(Regs8 r, bool arith) {
	uint8_t val = get_r8(r);
	uint8_t lsb = val & 0x01;
	uint8_t msb = val & 0x80;

	uint8_t newVal = val >> 1;

	if (arith) {
		// Preserve original MSB for SRA
		newVal |= msb;
	}

	set_r8(r, newVal);
	setFlag(Flags::Z, newVal == 0);
	setFlag(Flags::N, false);
	setFlag(Flags::H, false);
	setFlag(Flags::C, lsb);
}

void Cpu::swap_bits(Regs8 r) {
	uint8_t val = get_r8(r);
	uint8_t low = val & 0xF;
	uint8_t high = (val & 0xF0) >> 4;
	uint8_t newVal = (low << 4) | high;

	set_r8(r, newVal);
	setFlag(Flags::Z, newVal == 0);
	setFlag(Flags::N, false);
	setFlag(Flags::H, false);
	setFlag(Flags::C, false);
}

void Cpu::test_bit(Regs8 r, uint8_t bit) {
	uint8_t byte = get_r8(r);
	uint8_t val = get_bit8(bit, byte);

	setFlag(Flags::Z, !val);
	setFlag(Flags::N, false);
	setFlag(Flags::H, true);
}

void Cpu::write_bit(Regs8 r, uint8_t bit, bool set) {
	uint8_t byte = get_r8(r);
	uint8_t val = set_bit8(bit, byte, set);

	set_r8(r, val);
}

void Cpu::set_irq(bool enabled) {
	irq_enabled = enabled;
}

void Cpu::set_irq_scheduled(bool enabled) {
	irq_enabled_scheduled = enabled;
}

void Cpu::set_halted(bool isHalted) {
	halted = isHalted;
}

uint16_t Cpu::getVector(Interrupts intr) {
	switch (intr) {
		case Interrupts::Vblank: return 0x0040;
		case Interrupts::Stat: return 0x0048;
		case Interrupts::Timer: return 0x0050;
		case Interrupts::Serial: return 0x0058;
		case Interrupts::Joypad: return 0x0060;
	}
	return 0;
}

std::array<uint8_t, DISPLAY_BUFFER> Cpu::render() {
	return bus.render();
}

void Cpu::pressButton(Buttons button, bool pressed) {
	bus.pressButton(button, pressed);
	enableIrqType(Interrupts::Joypad, true);
}

std::string Cpu::getTitle() const{
	return bus.getTitle();
}

void Cpu::logState() {
	logFile << std::format("A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X} SP:{:04X} PC:{:04X} PCMEM:{:02X},{:02X},{:02X},{:02X}\n",
		get_r8(Regs8::A), get_r8(Regs8::F), get_r8(Regs8::B), get_r8(Regs8::C), get_r8(Regs8::D), get_r8(Regs8::E), get_r8(Regs8::H), get_r8(Regs8::L),
		sp,
		pc,
		readRAM(pc),
		readRAM(pc + 1),
		readRAM(pc + 2),
		readRAM(pc + 3)
	);
}

void Cpu::writeIF_raw(uint8_t val) {
	//write directly to IF register to clear interrupt, bypassing IO's F0FF address write rule
	bus.io.ram[IF - IO_START] = val;
}