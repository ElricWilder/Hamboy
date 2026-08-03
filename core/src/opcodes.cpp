#include "cpu.h"
#include "utils.h"
#include <cstdint>
#include <iostream>
#include <stdexcept>

std::array<Cpu::Opcode, 256> Cpu::opcodes{};

struct OpcodeInitializer {
	OpcodeInitializer() {
		// Initialize all opcode table entries with unimplemented
		//Cpu::opcodes.fill(&Cpu::op_unimplemented);

		Cpu::opcodes[0x00] = &Cpu::nop_00;
		Cpu::opcodes[0x01] = &Cpu::ld_01;
		Cpu::opcodes[0x02] = &Cpu::ld_02;
		Cpu::opcodes[0x03] = &Cpu::inc_03;
		Cpu::opcodes[0x04] = &Cpu::inc_04;
		Cpu::opcodes[0x05] = &Cpu::dec_05;
		Cpu::opcodes[0x06] = &Cpu::ld_06;
		Cpu::opcodes[0x07] = &Cpu::rlca_07;
		Cpu::opcodes[0x08] = &Cpu::ld_08;
		Cpu::opcodes[0x09] = &Cpu::add_09;
		Cpu::opcodes[0x0A] = &Cpu::ld_0A;
		Cpu::opcodes[0x0B] = &Cpu::dec_0B;
		Cpu::opcodes[0x0C] = &Cpu::inc_0C;
		Cpu::opcodes[0x0D] = &Cpu::dec_0D;
		Cpu::opcodes[0x0E] = &Cpu::ld_0E;
		Cpu::opcodes[0x0F] = &Cpu::rrca_0F;

		Cpu::opcodes[0x10] = &Cpu::stop_10;
		Cpu::opcodes[0x11] = &Cpu::ld_11;
		Cpu::opcodes[0x12] = &Cpu::ld_12;
		Cpu::opcodes[0x13] = &Cpu::inc_13;
		Cpu::opcodes[0x14] = &Cpu::inc_14;
		Cpu::opcodes[0x15] = &Cpu::dec_15;
		Cpu::opcodes[0x16] = &Cpu::ld_16;
		Cpu::opcodes[0x17] = &Cpu::rla_17;
		Cpu::opcodes[0x18] = &Cpu::jr_18;
		Cpu::opcodes[0x19] = &Cpu::add_19;
		Cpu::opcodes[0x1A] = &Cpu::ld_1A;
		Cpu::opcodes[0x1B] = &Cpu::dec_1B;
		Cpu::opcodes[0x1C] = &Cpu::inc_1C;
		Cpu::opcodes[0x1D] = &Cpu::dec_1D;
		Cpu::opcodes[0x1E] = &Cpu::ld_1E;
		Cpu::opcodes[0x1F] = &Cpu::rra_1F;

		Cpu::opcodes[0x20] = &Cpu::jr_20;
		Cpu::opcodes[0x21] = &Cpu::ld_21;
		Cpu::opcodes[0x22] = &Cpu::ld_22;
		Cpu::opcodes[0x23] = &Cpu::inc_23;
		Cpu::opcodes[0x24] = &Cpu::inc_24;
		Cpu::opcodes[0x25] = &Cpu::dec_25;
		Cpu::opcodes[0x26] = &Cpu::ld_26;
		Cpu::opcodes[0x27] = &Cpu::daa_27;
		Cpu::opcodes[0x28] = &Cpu::jr_28;
		Cpu::opcodes[0x29] = &Cpu::add_29;
		Cpu::opcodes[0x2A] = &Cpu::ld_2A;
		Cpu::opcodes[0x2B] = &Cpu::dec_2B;
		Cpu::opcodes[0x2C] = &Cpu::inc_2C;
		Cpu::opcodes[0x2D] = &Cpu::dec_2D;
		Cpu::opcodes[0x2E] = &Cpu::ld_2E;
		Cpu::opcodes[0x2F] = &Cpu::cpl_2F;

		Cpu::opcodes[0x30] = &Cpu::jr_30;;
		Cpu::opcodes[0x31] = &Cpu::ld_31;
		Cpu::opcodes[0x32] = &Cpu::ld_32;
		Cpu::opcodes[0x33] = &Cpu::inc_33;
		Cpu::opcodes[0x34] = &Cpu::inc_34;
		Cpu::opcodes[0x35] = &Cpu::dec_35;
		Cpu::opcodes[0x36] = &Cpu::ld_36;
		Cpu::opcodes[0x37] = &Cpu::scf_37;
		Cpu::opcodes[0x38] = &Cpu::jr_38;
		Cpu::opcodes[0x39] = &Cpu::add_39;
		Cpu::opcodes[0x3A] = &Cpu::ld_3A;
		Cpu::opcodes[0x3B] = &Cpu::dec_3B;
		Cpu::opcodes[0x3C] = &Cpu::inc_3C;
		Cpu::opcodes[0x3D] = &Cpu::dec_3D;
		Cpu::opcodes[0x3E] = &Cpu::ld_3E;
		Cpu::opcodes[0x3F] = &Cpu::ccf_3F;

		Cpu::opcodes[0x40] = &Cpu::ld_40;
		Cpu::opcodes[0x41] = &Cpu::ld_41;
		Cpu::opcodes[0x42] = &Cpu::ld_42;
		Cpu::opcodes[0x43] = &Cpu::ld_43;
		Cpu::opcodes[0x44] = &Cpu::ld_44;
		Cpu::opcodes[0x45] = &Cpu::ld_45;
		Cpu::opcodes[0x46] = &Cpu::ld_46;
		Cpu::opcodes[0x47] = &Cpu::ld_47;
		Cpu::opcodes[0x48] = &Cpu::ld_48;
		Cpu::opcodes[0x49] = &Cpu::ld_49;
		Cpu::opcodes[0x4A] = &Cpu::ld_4A;
		Cpu::opcodes[0x4B] = &Cpu::ld_4B;
		Cpu::opcodes[0x4C] = &Cpu::ld_4C;
		Cpu::opcodes[0x4D] = &Cpu::ld_4D;
		Cpu::opcodes[0x4E] = &Cpu::ld_4E;
		Cpu::opcodes[0x4F] = &Cpu::ld_4F;

		Cpu::opcodes[0x50] = &Cpu::ld_50;
		Cpu::opcodes[0x51] = &Cpu::ld_51;
		Cpu::opcodes[0x52] = &Cpu::ld_52;
		Cpu::opcodes[0x53] = &Cpu::ld_53;
		Cpu::opcodes[0x54] = &Cpu::ld_54;
		Cpu::opcodes[0x55] = &Cpu::ld_55;
		Cpu::opcodes[0x56] = &Cpu::ld_56;
		Cpu::opcodes[0x57] = &Cpu::ld_57;
		Cpu::opcodes[0x58] = &Cpu::ld_58;
		Cpu::opcodes[0x59] = &Cpu::ld_59;
		Cpu::opcodes[0x5A] = &Cpu::ld_5A;
		Cpu::opcodes[0x5B] = &Cpu::ld_5B;
		Cpu::opcodes[0x5C] = &Cpu::ld_5C;
		Cpu::opcodes[0x5D] = &Cpu::ld_5D;
		Cpu::opcodes[0x5E] = &Cpu::ld_5E;
		Cpu::opcodes[0x5F] = &Cpu::ld_5F;

		Cpu::opcodes[0x60] = &Cpu::ld_60;
		Cpu::opcodes[0x61] = &Cpu::ld_61;
		Cpu::opcodes[0x62] = &Cpu::ld_62;
		Cpu::opcodes[0x63] = &Cpu::ld_63;
		Cpu::opcodes[0x64] = &Cpu::ld_64;
		Cpu::opcodes[0x65] = &Cpu::ld_65;
		Cpu::opcodes[0x66] = &Cpu::ld_66;
		Cpu::opcodes[0x67] = &Cpu::ld_67;
		Cpu::opcodes[0x68] = &Cpu::ld_68;
		Cpu::opcodes[0x69] = &Cpu::ld_69;
		Cpu::opcodes[0x6A] = &Cpu::ld_6A;
		Cpu::opcodes[0x6B] = &Cpu::ld_6B;
		Cpu::opcodes[0x6C] = &Cpu::ld_6C;
		Cpu::opcodes[0x6D] = &Cpu::ld_6D;
		Cpu::opcodes[0x6E] = &Cpu::ld_6E;
		Cpu::opcodes[0x6F] = &Cpu::ld_6F;

		Cpu::opcodes[0x70] = &Cpu::ld_70;
		Cpu::opcodes[0x71] = &Cpu::ld_71;
		Cpu::opcodes[0x72] = &Cpu::ld_72;
		Cpu::opcodes[0x73] = &Cpu::ld_73;
		Cpu::opcodes[0x74] = &Cpu::ld_74;
		Cpu::opcodes[0x75] = &Cpu::ld_75;
		Cpu::opcodes[0x76] = &Cpu::halt_76;
		Cpu::opcodes[0x77] = &Cpu::ld_77;
		Cpu::opcodes[0x78] = &Cpu::ld_78;
		Cpu::opcodes[0x79] = &Cpu::ld_79;
		Cpu::opcodes[0x7A] = &Cpu::ld_7A;
		Cpu::opcodes[0x7B] = &Cpu::ld_7B;
		Cpu::opcodes[0x7C] = &Cpu::ld_7C;
		Cpu::opcodes[0x7D] = &Cpu::ld_7D;
		Cpu::opcodes[0x7E] = &Cpu::ld_7E;
		Cpu::opcodes[0x7F] = &Cpu::ld_7F;

		Cpu::opcodes[0x80] = &Cpu::add_80;
		Cpu::opcodes[0x81] = &Cpu::add_81;
		Cpu::opcodes[0x82] = &Cpu::add_82;
		Cpu::opcodes[0x83] = &Cpu::add_83;
		Cpu::opcodes[0x84] = &Cpu::add_84;
		Cpu::opcodes[0x85] = &Cpu::add_85;
		Cpu::opcodes[0x86] = &Cpu::add_86;
		Cpu::opcodes[0x87] = &Cpu::add_87;
		Cpu::opcodes[0x88] = &Cpu::adc_88;
		Cpu::opcodes[0x89] = &Cpu::adc_89;
		Cpu::opcodes[0x8A] = &Cpu::adc_8A;
		Cpu::opcodes[0x8B] = &Cpu::adc_8B;
		Cpu::opcodes[0x8C] = &Cpu::adc_8C;
		Cpu::opcodes[0x8D] = &Cpu::adc_8D;
		Cpu::opcodes[0x8E] = &Cpu::adc_8E;
		Cpu::opcodes[0x8F] = &Cpu::adc_8F;

		Cpu::opcodes[0x90] = &Cpu::sub_90;
		Cpu::opcodes[0x91] = &Cpu::sub_91;
		Cpu::opcodes[0x92] = &Cpu::sub_92;
		Cpu::opcodes[0x93] = &Cpu::sub_93;
		Cpu::opcodes[0x94] = &Cpu::sub_94;
		Cpu::opcodes[0x95] = &Cpu::sub_95;
		Cpu::opcodes[0x96] = &Cpu::sub_96;
		Cpu::opcodes[0x97] = &Cpu::sub_97;
		Cpu::opcodes[0x98] = &Cpu::sbc_98;
		Cpu::opcodes[0x99] = &Cpu::sbc_99;
		Cpu::opcodes[0x9A] = &Cpu::sbc_9A;
		Cpu::opcodes[0x9B] = &Cpu::sbc_9B;
		Cpu::opcodes[0x9C] = &Cpu::sbc_9C;
		Cpu::opcodes[0x9D] = &Cpu::sbc_9D;
		Cpu::opcodes[0x9E] = &Cpu::sbc_9E;
		Cpu::opcodes[0x9F] = &Cpu::sbc_9F;

		Cpu::opcodes[0xA0] = &Cpu::and_A0;
		Cpu::opcodes[0xA1] = &Cpu::and_A1;
		Cpu::opcodes[0xA2] = &Cpu::and_A2;
		Cpu::opcodes[0xA3] = &Cpu::and_A3;
		Cpu::opcodes[0xA4] = &Cpu::and_A4;
		Cpu::opcodes[0xA5] = &Cpu::and_A5;
		Cpu::opcodes[0xA6] = &Cpu::and_A6;
		Cpu::opcodes[0xA7] = &Cpu::and_A7;
		Cpu::opcodes[0xA8] = &Cpu::xor_A8;
		Cpu::opcodes[0xA9] = &Cpu::xor_A9;
		Cpu::opcodes[0xAA] = &Cpu::xor_AA;
		Cpu::opcodes[0xAB] = &Cpu::xor_AB;
		Cpu::opcodes[0xAC] = &Cpu::xor_AC;
		Cpu::opcodes[0xAD] = &Cpu::xor_AD;
		Cpu::opcodes[0xAE] = &Cpu::xor_AE;
		Cpu::opcodes[0xAF] = &Cpu::xor_AF;

		Cpu::opcodes[0xB0] = &Cpu::or_B0;
		Cpu::opcodes[0xB1] = &Cpu::or_B1;
		Cpu::opcodes[0xB2] = &Cpu::or_B2;
		Cpu::opcodes[0xB3] = &Cpu::or_B3;
		Cpu::opcodes[0xB4] = &Cpu::or_B4;
		Cpu::opcodes[0xB5] = &Cpu::or_B5;
		Cpu::opcodes[0xB6] = &Cpu::or_B6;
		Cpu::opcodes[0xB7] = &Cpu::or_B7;
		Cpu::opcodes[0xB8] = &Cpu::cp_B8;
		Cpu::opcodes[0xB9] = &Cpu::cp_B9;
		Cpu::opcodes[0xBA] = &Cpu::cp_BA;
		Cpu::opcodes[0xBB] = &Cpu::cp_BB;
		Cpu::opcodes[0xBC] = &Cpu::cp_BC;
		Cpu::opcodes[0xBD] = &Cpu::cp_BD;
		Cpu::opcodes[0xBE] = &Cpu::cp_BE;
		Cpu::opcodes[0xBF] = &Cpu::cp_BF;

		Cpu::opcodes[0xC0] = &Cpu::ret_C0;
		Cpu::opcodes[0xC1] = &Cpu::pop_C1;
		Cpu::opcodes[0xC2] = &Cpu::jp_C2;
		Cpu::opcodes[0xC3] = &Cpu::jp_C3;
		Cpu::opcodes[0xC4] = &Cpu::call_C4;
		Cpu::opcodes[0xC5] = &Cpu::push_C5;
		Cpu::opcodes[0xC6] = &Cpu::add_C6;
		Cpu::opcodes[0xC7] = &Cpu::rst_C7;
		Cpu::opcodes[0xC8] = &Cpu::ret_C8;
		Cpu::opcodes[0xC9] = &Cpu::ret_C9;
		Cpu::opcodes[0xCA] = &Cpu::jp_CA;
		Cpu::opcodes[0xCB] = &Cpu::prefix_CB;
		Cpu::opcodes[0xCC] = &Cpu::call_CC;
		Cpu::opcodes[0xCD] = &Cpu::call_CD;
		Cpu::opcodes[0xCE] = &Cpu::adc_CE;
		Cpu::opcodes[0xCF] = &Cpu::rst_CF;

		Cpu::opcodes[0xD0] = &Cpu::ret_D0;
		Cpu::opcodes[0xD1] = &Cpu::pop_D1;
		Cpu::opcodes[0xD2] = &Cpu::jp_D2;
		Cpu::opcodes[0xD3] = &Cpu::invalid;
		Cpu::opcodes[0xD4] = &Cpu::call_D4;
		Cpu::opcodes[0xD5] = &Cpu::push_D5;
		Cpu::opcodes[0xD6] = &Cpu::sub_D6;
		Cpu::opcodes[0xD7] = &Cpu::rst_D7;
		Cpu::opcodes[0xD8] = &Cpu::ret_D8;
		Cpu::opcodes[0xD9] = &Cpu::reti_D9;
		Cpu::opcodes[0xDA] = &Cpu::jp_DA;
		Cpu::opcodes[0xDB] = &Cpu::invalid;
		Cpu::opcodes[0xDC] = &Cpu::call_DC;
		Cpu::opcodes[0xDD] = &Cpu::invalid;
		Cpu::opcodes[0xDE] = &Cpu::sbc_DE;
		Cpu::opcodes[0xDF] = &Cpu::rst_DF;

		Cpu::opcodes[0xE0] = &Cpu::ld_E0;
		Cpu::opcodes[0xE1] = &Cpu::pop_E1;
		Cpu::opcodes[0xE2] = &Cpu::ld_E2;
		Cpu::opcodes[0xE3] = &Cpu::invalid;
		Cpu::opcodes[0xE4] = &Cpu::invalid;
		Cpu::opcodes[0xE5] = &Cpu::push_E5;
		Cpu::opcodes[0xE6] = &Cpu::and_E6;
		Cpu::opcodes[0xE7] = &Cpu::rst_E7;
		Cpu::opcodes[0xE8] = &Cpu::add_E8;
		Cpu::opcodes[0xE9] = &Cpu::jp_E9;
		Cpu::opcodes[0xEA] = &Cpu::ld_EA;
		Cpu::opcodes[0xEB] = &Cpu::invalid;
		Cpu::opcodes[0xEC] = &Cpu::invalid;
		Cpu::opcodes[0xED] = &Cpu::invalid;
		Cpu::opcodes[0xEE] = &Cpu::xor_EE;
		Cpu::opcodes[0xEF] = &Cpu::rst_EF;

		Cpu::opcodes[0xF0] = &Cpu::ld_F0;
		Cpu::opcodes[0xF1] = &Cpu::pop_F1;
		Cpu::opcodes[0xF2] = &Cpu::ld_F2;
		Cpu::opcodes[0xF3] = &Cpu::di_F3;
		Cpu::opcodes[0xF4] = &Cpu::invalid;
		Cpu::opcodes[0xF5] = &Cpu::push_F5;
		Cpu::opcodes[0xF6] = &Cpu::or_F6;
		Cpu::opcodes[0xF7] = &Cpu::rst_F7;
		Cpu::opcodes[0xF8] = &Cpu::ld_F8;
		Cpu::opcodes[0xF9] = &Cpu::ld_F9;
		Cpu::opcodes[0xFA] = &Cpu::ld_FA;
		Cpu::opcodes[0xFB] = &Cpu::ei_FB;
		Cpu::opcodes[0xFC] = &Cpu::invalid;
		Cpu::opcodes[0xFD] = &Cpu::invalid;
		Cpu::opcodes[0xFE] = &Cpu::cp_FE;
		Cpu::opcodes[0xFF] = &Cpu::rst_FF;

	}
} opcode_initializer;

uint8_t Cpu::op_unimplemented() {
	std::cout << "Opcode not yet implemented\n";
	return 0;
}

uint8_t Cpu::invalid() {
	throw std::runtime_error("Invalid opcode");
	return 0;
}

///// Row 0 /////

// NOP
uint8_t Cpu::nop_00() {
	// Do nothing;
	return 1;
}

// LD BC,u16
uint8_t Cpu::ld_01() {
	uint16_t val = fetch16();
	set_r16(Regs16::BC, val);
	return 3;
}

// LD (BC),A
uint8_t Cpu::ld_02() {
	uint8_t val = get_r8(Regs8::A);
	uint16_t addr = get_r16(Regs16::BC);
	writeRAM(addr, val);
	return 2;
}

// INC BC
uint8_t Cpu::inc_03() {
	inc_r16(Regs16::BC);
	return 2;
}

// INC B
uint8_t Cpu::inc_04() {
	inc_r8(Regs8::B);
	return 1;
}

// DEC B
uint8_t Cpu::dec_05() {
	dec_r8(Regs8::B);
	return 1;
}

// LD B,u8
uint8_t Cpu::ld_06() {
	uint8_t val = fetch8();
	set_r8(Regs8::B, val);
	return 2;
}

// RLCA
uint8_t Cpu::rlca_07() {
	rotate_left(Regs8::A, false);
	setFlag(Flags::Z, false);
	return 1;
}

// LD (u16),SP
uint8_t Cpu::ld_08() {
	uint16_t addr = fetch16();
	uint16_t val = get_r16(Regs16::SP);
	writeRAM(addr, getLowByte(val));
	writeRAM(addr + 1, getHighByte(val));
	return 5;
}

// ADD HL,BC
uint8_t Cpu::add_09() {
	add_r16(Regs16::HL, Regs16::BC);
	return 2;
}

// LD A,(BC)
uint8_t Cpu::ld_0A() {
	uint16_t addr = get_r16(Regs16::BC);
	uint8_t val = readRAM(addr);
	set_r8(Regs8::A, val);
	return 2;
}

// DEC BC
uint8_t Cpu::dec_0B() {
	dec_r16(Regs16::BC);
	return 2;
}

// INC C
uint8_t Cpu::inc_0C() {
	inc_r8(Regs8::C);
	return 1;
}

// DEC C
uint8_t Cpu::dec_0D() {
	dec_r8(Regs8::C);
	return 1;
}

// LD C,u8
uint8_t Cpu::ld_0E() {
	uint8_t val = fetch8();
	set_r8(Regs8::C, val);
	return 2;
}

// RRCA
uint8_t Cpu::rrca_0F() {
	rotate_right(Regs8::A, false);
	setFlag(Flags::Z, false);
	return 1;
}

///// Row 1 /////


// STOP
uint8_t Cpu::stop_10() {
	// Unimplemented
	return 1;
}

// LD DE,u16
uint8_t Cpu::ld_11() {
	uint16_t val = fetch16();
	set_r16(Regs16::DE, val);
	return 3;
}

// LD (DE),A
uint8_t Cpu::ld_12() {
	uint8_t val = get_r8(Regs8::A);
	uint16_t addr = get_r16(Regs16::DE);
	writeRAM(addr, val);
	return 2;
}

// INC DE
uint8_t Cpu::inc_13() {
	inc_r16(Regs16::DE);
	return 2;
}

// INC D
uint8_t Cpu::inc_14() {
	inc_r8(Regs8::D);
	return 1;
}

// DEC D
uint8_t Cpu::dec_15() {
	dec_r8(Regs8::D);
	return 1;
}

// LD D,u8
uint8_t Cpu::ld_16() {
	uint8_t val = fetch8();
	set_r8(Regs8::D, val);
	return 2;
}

// RLA
uint8_t Cpu::rla_17() {
	rotate_left(Regs8::A, true);
	setFlag(Flags::Z, false);
	return 1;
}

// JR i8
uint8_t Cpu::jr_18() {
	int8_t offset = int8_t(fetch8());

	setPC(uint16_t(getPC() + offset));
	return 3;
}

// ADD HL,DE
uint8_t Cpu::add_19() {
	add_r16(Regs16::HL, Regs16::DE);
	return 2;
}

// LD A,(DE)
uint8_t Cpu::ld_1A() {
	uint16_t addr = get_r16(Regs16::DE);
	uint8_t val = readRAM(addr);
	set_r8(Regs8::A, val);
	return 2;
}

// DEC DE
uint8_t Cpu::dec_1B() {
	dec_r16(Regs16::DE);
	return 2;
}

// INC E
uint8_t Cpu::inc_1C() {
	inc_r8(Regs8::E);
	return 1;
}

// DEC E
uint8_t Cpu::dec_1D() {
	dec_r8(Regs8::E);
	return 1;
}

// LD E,u8
uint8_t Cpu::ld_1E() {
	uint8_t val = fetch8();
	set_r8(Regs8::E, val);
	return 2;
}

// RRA
uint8_t Cpu::rra_1F() {
	rotate_right(Regs8::A, true);
	setFlag(Flags::Z, false);
	return 1;
}

///// Row 2 /////

// JR NZ,i8
uint8_t Cpu::jr_20() {
	int8_t offset = int8_t(fetch8());
	uint16_t pc = getPC();

	if (!getFlag(Flags::Z)) {
		setPC(uint16_t(pc + offset));
		return 3;
	}
	else {
		return 2;
	}
}

// LD HL,u16
uint8_t Cpu::ld_21() {
	uint16_t val = fetch16();
	set_r16(Regs16::HL, val);
	return 3;
}

// LD (HL+),A
uint8_t Cpu::ld_22() {
	uint8_t val = get_r8(Regs8::A);
	uint16_t addr = get_r16(Regs16::HL);
	writeRAM(addr, val);
	set_r16(Regs16::HL, addr + 1);
	return 2;
}

// INC HL
uint8_t Cpu::inc_23() {
	inc_r16(Regs16::HL);
	return 2;
}

// INC H
uint8_t Cpu::inc_24() {
	inc_r8(Regs8::H);
	return 1;
}

// DEC H
uint8_t Cpu::dec_25() {
	dec_r8(Regs8::H);
	return 1;
}

// LD H,u8
uint8_t Cpu::ld_26() {
	uint8_t val = fetch8();
	set_r8(Regs8::H, val);
	return 2;
}

// DAA
uint8_t Cpu::daa_27() {
	uint8_t a = get_r8(Regs8::A);
	uint8_t correction = 0;
	bool carry = false;

	if (!getFlag(Flags::N)) {
		if (getFlag(Flags::H) || (a & 0x0F) > 9) {
			correction |= 0x06;
		}
		if (getFlag(Flags::C) || a > 0x99) {
			correction |= 0x60;
			carry = true;
		}
		a += correction;
	}
	else {
		if (getFlag(Flags::H)) {
			correction |= 0x06;
		}
		if (getFlag(Flags::C)) {
			correction |= 0x60;
			carry = true;
		}
		a -= correction;
	}

	set_r8(Regs8::A, a);
	setFlag(Flags::Z, a == 0);
	setFlag(Flags::H, false);
	setFlag(Flags::C, carry);
	return 1;
}

// JR Z,i8
uint8_t Cpu::jr_28() {
	int8_t offset = int8_t(fetch8());
	uint16_t pc = getPC();

	if (getFlag(Flags::Z)) {
		setPC(pc + offset);
		return 3;
	}
	else {
		return 2;
	}
}

// ADD HL,HL
uint8_t Cpu::add_29() {
	add_r16(Regs16::HL, Regs16::HL);
	return 2;
}

// LD A,(HL+)
uint8_t Cpu::ld_2A() {
	uint16_t addr = get_r16(Regs16::HL);
	uint8_t val = readRAM(addr);
	set_r8(Regs8::A, val);
	set_r16(Regs16::HL, addr + 1);
	return 2;
}

// DEC HL
uint8_t Cpu::dec_2B() {
	dec_r16(Regs16::HL);
	return 2;
}

// INC L
uint8_t Cpu::inc_2C() {
	inc_r8(Regs8::L);
	return 1;
}

// DEC L
uint8_t Cpu::dec_2D() {
	dec_r8(Regs8::L);
	return 1;
}

// LD L,u8
uint8_t Cpu::ld_2E() {
	uint8_t val = fetch8();
	set_r8(Regs8::L, val);
	return 2;
}

uint8_t Cpu::cpl_2F() {
	uint8_t a = get_r8(Regs8::A);
	set_r8(Regs8::A, ~a);
	setFlag(Flags::N, true);
	setFlag(Flags::H, true);
	return 1;
}

///// Row 3 /////

// JR NC,i8
uint8_t Cpu::jr_30() {
	int8_t offset = int8_t(fetch8());
	uint16_t pc = getPC();

	if (!getFlag(Flags::C)) {
		setPC(pc + offset);
		return 3;
	}
	else {
		return 2;
	}
}

// LD SP,u16
uint8_t Cpu::ld_31() {
	uint16_t val = fetch16();
	set_r16(Regs16::SP, val);
	return 3;
}

// LD (HL-),A
uint8_t Cpu::ld_32() {
	uint8_t val = get_r8(Regs8::A);
	uint16_t addr = get_r16(Regs16::HL);
	writeRAM(addr, val);
	set_r16(Regs16::HL, addr - 1);
	return 2;
}

// INC SP
uint8_t Cpu::inc_33() {
	inc_r16(Regs16::SP);
	return 2;
}

// INC (HL)
uint8_t Cpu::inc_34() {
	inc_r8(Regs8::HL);
	return 3;
}

// DEC (HL)
uint8_t Cpu::dec_35() {
	dec_r8(Regs8::HL);
	return 3;
}

// LD (HL),u8
uint8_t Cpu::ld_36() {
	uint8_t val = fetch8();
	set_r8(Regs8::HL, val);
	return 3;
}

// SCF
uint8_t Cpu::scf_37() {
	setFlag(Flags::N, false);
	setFlag(Flags::H, false);
	setFlag(Flags::C, true);
	return 1;
}

// JR C,i8
uint8_t Cpu::jr_38() {
	int8_t offset = int8_t(fetch8());
	uint16_t pc = getPC();

	if (getFlag(Flags::C)) {
		setPC(pc + offset);
		return 3;
	}
	else {
		return 2;
	}
}

// ADD HL,SP
uint8_t Cpu::add_39() {
	add_r16(Regs16::HL, Regs16::SP);
	return 2;
}

// LD A,(HL-)
uint8_t Cpu::ld_3A() {
	uint16_t addr = get_r16(Regs16::HL);
	uint8_t val = readRAM(addr);
	set_r8(Regs8::A, val);
	set_r16(Regs16::HL, addr - 1);
	return 2;
}

// DEC SP
uint8_t Cpu::dec_3B() {
	dec_r16(Regs16::SP);
	return 2;
}

// INC A
uint8_t Cpu::inc_3C() {
	inc_r8(Regs8::A);
	return 1;
}

// DEC A
uint8_t Cpu::dec_3D() {
	dec_r8(Regs8::A);
	return 1;
}

// LD A,u8
uint8_t Cpu::ld_3E() {
	uint8_t val = fetch8();
	set_r8(Regs8::A, val);
	return 2;
}

// CCF
uint8_t Cpu::ccf_3F() {
	bool c = getFlag(Flags::C);
	setFlag(Flags::N, false);
	setFlag(Flags::H, false);
	setFlag(Flags::C, !c);
	return 1;
}

///// Row 4 /////

// LD B,B
uint8_t Cpu::ld_40() {
	// Does nothing
	return 1;
}

// LD B,C
uint8_t Cpu::ld_41() {
	uint8_t val = get_r8(Regs8::C);
	set_r8(Regs8::B, val);
	return 1;
}

// LD B,D
uint8_t Cpu::ld_42() {
	uint8_t val = get_r8(Regs8::D);
	set_r8(Regs8::B, val);
	return 1;
}

// LD B,E
uint8_t Cpu::ld_43() {
	uint8_t val = get_r8(Regs8::E);
	set_r8(Regs8::B, val);
	return 1;
}

// LD B,H
uint8_t Cpu::ld_44() {
	uint8_t val = get_r8(Regs8::H);
	set_r8(Regs8::B, val);
	return 1;
}

// LD B,L
uint8_t Cpu::ld_45() {
	uint8_t val = get_r8(Regs8::L);
	set_r8(Regs8::B, val);
	return 1;
}

// LD B,(HL)
uint8_t Cpu::ld_46() {
	uint8_t val = get_r8(Regs8::HL);
	set_r8(Regs8::B, val);
	return 2;
}

// LD B,A
uint8_t Cpu::ld_47() {
	uint8_t val = get_r8(Regs8::A);
	set_r8(Regs8::B, val);
	return 1;
}

// LD C,B
uint8_t Cpu::ld_48() {
	uint8_t val = get_r8(Regs8::B);
	set_r8(Regs8::C, val);
	return 1;
}

// LD C,C
uint8_t Cpu::ld_49() {
	// Does nothing
	return 1;
}

// LD C,D
uint8_t Cpu::ld_4A() {
	uint8_t val = get_r8(Regs8::D);
	set_r8(Regs8::C, val);
	return 1;
}

// LD C,E
uint8_t Cpu::ld_4B() {
	uint8_t val = get_r8(Regs8::E);
	set_r8(Regs8::C, val);
	return 1;
}

// LD C,H
uint8_t Cpu::ld_4C() {
	uint8_t val = get_r8(Regs8::H);
	set_r8(Regs8::C, val);
	return 1;
}

// LD C,L
uint8_t Cpu::ld_4D() {
	uint8_t val = get_r8(Regs8::L);
	set_r8(Regs8::C, val);
	return 1;
}

// LD C,(HL)
uint8_t Cpu::ld_4E() {
	uint8_t val = get_r8(Regs8::HL);
	set_r8(Regs8::C, val);
	return 2;
}

// LD C,A
uint8_t Cpu::ld_4F() {
	uint8_t val = get_r8(Regs8::A);
	set_r8(Regs8::C, val);
	return 1;
}

///// Row 5 /////

// LD D,B
uint8_t Cpu::ld_50() {
	uint8_t val = get_r8(Regs8::B);
	set_r8(Regs8::D, val);
	return 1;
}

// LD D,C
uint8_t Cpu::ld_51() {
	uint8_t val = get_r8(Regs8::C);
	set_r8(Regs8::D, val);
	return 1;
}

// LD D,D
uint8_t Cpu::ld_52() {
	// Does nothing
	return 1;
}

// LD D,E
uint8_t Cpu::ld_53() {
	uint8_t val = get_r8(Regs8::E);
	set_r8(Regs8::D, val);
	return 1;
}

// LD D,H
uint8_t Cpu::ld_54() {
	uint8_t val = get_r8(Regs8::H);
	set_r8(Regs8::D, val);
	return 1;
}

// LD D,L
uint8_t Cpu::ld_55() {
	uint8_t val = get_r8(Regs8::L);
	set_r8(Regs8::D, val);
	return 1;
}

// LD D,(HL)
uint8_t Cpu::ld_56() {
	uint8_t val = get_r8(Regs8::HL);
	set_r8(Regs8::D, val);
	return 2;
}

// LD D,A
uint8_t Cpu::ld_57() {
	uint8_t val = get_r8(Regs8::A);
	set_r8(Regs8::D, val);
	return 1;
}

// LD E,B
uint8_t Cpu::ld_58() {
	uint8_t val = get_r8(Regs8::B);
	set_r8(Regs8::E, val);
	return 1;
}

// LD E,C
uint8_t Cpu::ld_59() {
	uint8_t val = get_r8(Regs8::C);
	set_r8(Regs8::E, val);
	return 1;
}

// LD E,D
uint8_t Cpu::ld_5A() {
	uint8_t val = get_r8(Regs8::D);
	set_r8(Regs8::E, val);
	return 1;
}

// LD E,E
uint8_t Cpu::ld_5B() {
	// Do nothing
	return 1;
}

// LD E,H
uint8_t Cpu::ld_5C() {
	uint8_t val = get_r8(Regs8::H);
	set_r8(Regs8::E, val);
	return 1;
}

// LD E,L
uint8_t Cpu::ld_5D() {
	uint8_t val = get_r8(Regs8::L);
	set_r8(Regs8::E, val);
	return 1;
}

// LD E,(HL)
uint8_t Cpu::ld_5E() {
	uint8_t val = get_r8(Regs8::HL);
	set_r8(Regs8::E, val);
	return 2;
}

// LD E,A
uint8_t Cpu::ld_5F() {
	uint8_t val = get_r8(Regs8::A);
	set_r8(Regs8::E, val);
	return 1;
}

///// Row 6 /////

// LD H,B
uint8_t Cpu::ld_60() {
	uint8_t val = get_r8(Regs8::B);
	set_r8(Regs8::H, val);
	return 1;
}

// LD H,C
uint8_t Cpu::ld_61() {
	uint8_t val = get_r8(Regs8::C);
	set_r8(Regs8::H, val);
	return 1;
}

// LD H,D
uint8_t Cpu::ld_62() {
	uint8_t val = get_r8(Regs8::D);
	set_r8(Regs8::H, val);
	return 1;
}

// LD H,E
uint8_t Cpu::ld_63() {
	uint8_t val = get_r8(Regs8::E);
	set_r8(Regs8::H, val);
	return 1;
}

// LD H,H
uint8_t Cpu::ld_64() {
	// Does nothing
	return 1;
}

// LD H,L
uint8_t Cpu::ld_65() {
	uint8_t val = get_r8(Regs8::L);
	set_r8(Regs8::H, val);
	return 1;
}

// LD H,(HL)
uint8_t Cpu::ld_66() {
	uint8_t val = get_r8(Regs8::HL);
	set_r8(Regs8::H, val);
	return 2;
}

// LD H,A
uint8_t Cpu::ld_67() {
	uint8_t val = get_r8(Regs8::A);
	set_r8(Regs8::H, val);
	return 1;
}

// LD L,B
uint8_t Cpu::ld_68() {
	uint8_t val = get_r8(Regs8::B);
	set_r8(Regs8::L, val);
	return 1;
}

// LD L,C
uint8_t Cpu::ld_69() {
	uint8_t val = get_r8(Regs8::C);
	set_r8(Regs8::L, val);
	return 1;
}

// LD L,D
uint8_t Cpu::ld_6A() {
	uint8_t val = get_r8(Regs8::D);
	set_r8(Regs8::L, val);
	return 1;
}

// LD L,E
uint8_t Cpu::ld_6B() {
	uint8_t val = get_r8(Regs8::E);
	set_r8(Regs8::L, val);
	return 1;
}

// LD L,H
uint8_t Cpu::ld_6C() {
	uint8_t val = get_r8(Regs8::H);
	set_r8(Regs8::L, val);
	return 1;
}

// LD L,L
uint8_t Cpu::ld_6D() {
	// Do nothing
	return 1;
}

// LD L,(HL)
uint8_t Cpu::ld_6E() {
	uint8_t val = get_r8(Regs8::HL);
	set_r8(Regs8::L, val);
	return 2;
}

// LD L,A
uint8_t Cpu::ld_6F() {
	uint8_t val = get_r8(Regs8::A);
	set_r8(Regs8::L, val);
	return 1;
}

///// Row 7 /////

// LD (HL),B
uint8_t Cpu::ld_70() {
	uint8_t val = get_r8(Regs8::B);
	set_r8(Regs8::HL, val);
	return 2;
}

// LD (HL),C
uint8_t Cpu::ld_71() {
	uint8_t val = get_r8(Regs8::C);
	set_r8(Regs8::HL, val);
	return 2;
}

// LD (HL),D
uint8_t Cpu::ld_72() {
	uint8_t val = get_r8(Regs8::D);
	set_r8(Regs8::HL, val);
	return 2;
}

// LD (HL),E
uint8_t Cpu::ld_73() {
	uint8_t val = get_r8(Regs8::E);
	set_r8(Regs8::HL, val);
	return 2;
}

// LD (HL),H
uint8_t Cpu::ld_74() {
	uint8_t val = get_r8(Regs8::H);
	set_r8(Regs8::HL, val);
	return 2;
}

// LD (HL),L
uint8_t Cpu::ld_75() {
	uint8_t val = get_r8(Regs8::L);
	set_r8(Regs8::HL, val);
	return 2;
}

// HALT
uint8_t Cpu::halt_76() {
	set_halted(true);
	return 1;
}

// LD (HL),A
uint8_t Cpu::ld_77() {
	uint8_t val = get_r8(Regs8::A);
	set_r8(Regs8::HL, val);
	return 2;
}

// LD A,B
uint8_t Cpu::ld_78() {
	uint8_t val = get_r8(Regs8::B);
	set_r8(Regs8::A, val);
	return 1;
}

// LD A,C
uint8_t Cpu::ld_79() {
	uint8_t val = get_r8(Regs8::C);
	set_r8(Regs8::A, val);
	return 1;
}

// LD A,D
uint8_t Cpu::ld_7A() {
	uint8_t val = get_r8(Regs8::D);
	set_r8(Regs8::A, val);
	return 1;
}

// LD A,E
uint8_t Cpu::ld_7B() {
	uint8_t val = get_r8(Regs8::E);
	set_r8(Regs8::A, val);
	return 1;
}

// LD A,H
uint8_t Cpu::ld_7C() {
	uint8_t val = get_r8(Regs8::H);
	set_r8(Regs8::A, val);
	return 1;
}

// LD A,L
uint8_t Cpu::ld_7D() {
	uint8_t val = get_r8(Regs8::L);
	set_r8(Regs8::A, val);
	return 1;
}

// LD A,(HL)
uint8_t Cpu::ld_7E() {
	uint8_t val = get_r8(Regs8::HL);
	set_r8(Regs8::A, val);
	return 2;
}

// LD A,A
uint8_t Cpu::ld_7F() {
	// Do nothing
	return 1;
}

///// Row 8 /////

// ADD A,B
uint8_t Cpu::add_80() {
	uint8_t val = get_r8(Regs8::B);
	add_a_u8(val, false);
	return 1;
}

// ADD A,C
uint8_t Cpu::add_81() {
	uint8_t val = get_r8(Regs8::C);
	add_a_u8(val, false);
	return 1;
}

// ADD A,D
uint8_t Cpu::add_82() {
	uint8_t val = get_r8(Regs8::D);
	add_a_u8(val, false);
	return 1;
}

// ADD A,E
uint8_t Cpu::add_83() {
	uint8_t val = get_r8(Regs8::E);
	add_a_u8(val, false);
	return 1;
}

// ADD A,H
uint8_t Cpu::add_84() {
	uint8_t val = get_r8(Regs8::H);
	add_a_u8(val, false);
	return 1;
}

// ADD A,L
uint8_t Cpu::add_85() {
	uint8_t val = get_r8(Regs8::L);
	add_a_u8(val, false);
	return 1;
}

// ADD A,(HL)
uint8_t Cpu::add_86() {
	uint8_t val = get_r8(Regs8::HL);
	add_a_u8(val, false);
	return 2;
}

// ADD A,A
uint8_t Cpu::add_87() {
	uint8_t val = get_r8(Regs8::A);
	add_a_u8(val, false);
	return 1;
}

// ADC A,B
uint8_t Cpu::adc_88() {
	uint8_t val = get_r8(Regs8::B);
	add_a_u8(val, true);
	return 1;
}

// ADC A,C
uint8_t Cpu::adc_89() {
	uint8_t val = get_r8(Regs8::C);
	add_a_u8(val, true);
	return 1;
}

// ADC A,D
uint8_t Cpu::adc_8A() {
	uint8_t val = get_r8(Regs8::D);
	add_a_u8(val, true);
	return 1;
}

// ADC A,E
uint8_t Cpu::adc_8B() {
	uint8_t val = get_r8(Regs8::E);
	add_a_u8(val, true);
	return 1;
}

// ADC A,H
uint8_t Cpu::adc_8C() {
	uint8_t val = get_r8(Regs8::H);
	add_a_u8(val, true);
	return 1;
}

// ADC A,L
uint8_t Cpu::adc_8D() {
	uint8_t val = get_r8(Regs8::L);
	add_a_u8(val, true);
	return 1;
}

// ADC A,(HL)
uint8_t Cpu::adc_8E() {
	uint8_t val = get_r8(Regs8::HL);
	add_a_u8(val, true);
	return 2;
}

// ADC A,A
uint8_t Cpu::adc_8F() {
	uint8_t val = get_r8(Regs8::A);
	add_a_u8(val, true);
	return 1;
}

///// ROW 9 /////

// SUB A,B
uint8_t Cpu::sub_90() {
	uint8_t val = get_r8(Regs8::B);
	sub_a_u8(val, false);
	return 1;
}

// SUB A,C
uint8_t Cpu::sub_91() {
	uint8_t val = get_r8(Regs8::C);
	sub_a_u8(val, false);
	return 1;
}

// SUB A,D
uint8_t Cpu::sub_92() {
	uint8_t val = get_r8(Regs8::D);
	sub_a_u8(val, false);
	return 1;
}

// SUB A,E
uint8_t Cpu::sub_93() {
	uint8_t val = get_r8(Regs8::E);
	sub_a_u8(val, false);
	return 1;
}

// SUB A,H
uint8_t Cpu::sub_94() {
	uint8_t val = get_r8(Regs8::H);
	sub_a_u8(val, false);
	return 1;
}

// SUB A,L
uint8_t Cpu::sub_95() {
	uint8_t val = get_r8(Regs8::L);
	sub_a_u8(val, false);
	return 1;
}

// SUB A,(HL)
uint8_t Cpu::sub_96() {
	uint8_t val = get_r8(Regs8::HL);
	sub_a_u8(val, false);
	return 2;
}

// SUB A,A
uint8_t Cpu::sub_97() {
	uint8_t val = get_r8(Regs8::A);
	sub_a_u8(val, false);
	return 1;
}

// SBC A,B
uint8_t Cpu::sbc_98() {
	uint8_t val = get_r8(Regs8::B);
	sub_a_u8(val, true);
	return 1;
}

// SBC A,C
uint8_t Cpu::sbc_99() {
	uint8_t val = get_r8(Regs8::C);
	sub_a_u8(val, true);
	return 1;
}

// SBC A,D
uint8_t Cpu::sbc_9A() {
	uint8_t val = get_r8(Regs8::D);
	sub_a_u8(val, true);
	return 1;
}

// SBC A,E
uint8_t Cpu::sbc_9B() {
	uint8_t val = get_r8(Regs8::E);
	sub_a_u8(val, true);
	return 1;
}

// SBC A,H
uint8_t Cpu::sbc_9C() {
	uint8_t val = get_r8(Regs8::H);
	sub_a_u8(val, true);
	return 1;
}

// SBC A,L
uint8_t Cpu::sbc_9D() {
	uint8_t val = get_r8(Regs8::L);
	sub_a_u8(val, true);
	return 1;
}

// SBC A,(HL)
uint8_t Cpu::sbc_9E() {
	uint8_t val = get_r8(Regs8::HL);
	sub_a_u8(val, true);
	return 2;
}

// SBC A,A
uint8_t Cpu::sbc_9F() {
	uint8_t val = get_r8(Regs8::A);
	sub_a_u8(val, true);
	return 1;
}

///// Row A /////

// AND A,B
uint8_t Cpu::and_A0() {
	uint8_t val = get_r8(Regs8::B);
	and_a_u8(val);
	return 1;
}

// AND A,C
uint8_t Cpu::and_A1() {
	uint8_t val = get_r8(Regs8::C);
	and_a_u8(val);
	return 1;
}

// AND A,D
uint8_t Cpu::and_A2() {
	uint8_t val = get_r8(Regs8::D);
	and_a_u8(val);
	return 1;
}

// AND A,E
uint8_t Cpu::and_A3() {
	uint8_t val = get_r8(Regs8::E);
	and_a_u8(val);
	return 1;
}

// AND A,H
uint8_t Cpu::and_A4() {
	uint8_t val = get_r8(Regs8::H);
	and_a_u8(val);
	return 1;
}

// AND A,L
uint8_t Cpu::and_A5() {
	uint8_t val = get_r8(Regs8::L);
	and_a_u8(val);
	return 1;
}

// AND A,(HL)
uint8_t Cpu::and_A6() {
	uint8_t val = get_r8(Regs8::HL);
	and_a_u8(val);
	return 2;
}

// AND A,A
uint8_t Cpu::and_A7() {
	uint8_t val = get_r8(Regs8::A);
	and_a_u8(val);
	return 1;
}

// XOR A,B
uint8_t Cpu::xor_A8() {
	uint8_t val = get_r8(Regs8::B);
	xor_a_u8(val);
	return 1;
}

// XOR A,C
uint8_t Cpu::xor_A9() {
	uint8_t val = get_r8(Regs8::C);
	xor_a_u8(val);
	return 1;
}

// XOR A,D
uint8_t Cpu::xor_AA() {
	uint8_t val = get_r8(Regs8::D);
	xor_a_u8(val);
	return 1;
}

// XOR A,E
uint8_t Cpu::xor_AB() {
	uint8_t val = get_r8(Regs8::E);
	xor_a_u8(val);
	return 1;
}

// XOR A,H
uint8_t Cpu::xor_AC() {
	uint8_t val = get_r8(Regs8::H);
	xor_a_u8(val);
	return 1;
}

// XOR A,L
uint8_t Cpu::xor_AD() {
	uint8_t val = get_r8(Regs8::L);
	xor_a_u8(val);
	return 1;
}

// XOR A,(HL)
uint8_t Cpu::xor_AE() {
	uint8_t val = get_r8(Regs8::HL);
	xor_a_u8(val);
	return 2;
}

// XOR A,A
uint8_t Cpu::xor_AF() {
	uint8_t val = get_r8(Regs8::A);
	xor_a_u8(val);
	return 1;
}

///// Row B /////

// OR A,B
uint8_t Cpu::or_B0() {
	uint8_t val = get_r8(Regs8::B);
	or_a_u8(val);
	return 1;
}

// OR A,C
uint8_t Cpu::or_B1() {
	uint8_t val = get_r8(Regs8::C);
	or_a_u8(val);
	return 1;
}

// OR A,D
uint8_t Cpu::or_B2() {
	uint8_t val = get_r8(Regs8::D);
	or_a_u8(val);
	return 1;
}

// OR A,E
uint8_t Cpu::or_B3() {
	uint8_t val = get_r8(Regs8::E);
	or_a_u8(val);
	return 1;
}

// OR A,H
uint8_t Cpu::or_B4() {
	uint8_t val = get_r8(Regs8::H);
	or_a_u8(val);
	return 1;
}

// OR A,L
uint8_t Cpu::or_B5() {
	uint8_t val = get_r8(Regs8::L);
	or_a_u8(val);
	return 1;
}

// OR A,(HL)
uint8_t Cpu::or_B6() {
	uint8_t val = get_r8(Regs8::HL);
	or_a_u8(val);
	return 2;
}

// OR A,A
uint8_t Cpu::or_B7() {
	uint8_t val = get_r8(Regs8::A);
	or_a_u8(val);
	return 1;
}

// CP A,B
uint8_t Cpu::cp_B8() {
	uint8_t val = get_r8(Regs8::B);
	cp_a_u8(val);
	return 1;
}

// CP A,C
uint8_t Cpu::cp_B9() {
	uint8_t val = get_r8(Regs8::C);
	cp_a_u8(val);
	return 1;
}

// CP A,D
uint8_t Cpu::cp_BA() {
	uint8_t val = get_r8(Regs8::D);
	cp_a_u8(val);
	return 1;
}

// CP A,E
uint8_t Cpu::cp_BB() {
	uint8_t val = get_r8(Regs8::E);
	cp_a_u8(val);
	return 1;
}

// CP A,H
uint8_t Cpu::cp_BC() {
	uint8_t val = get_r8(Regs8::H);
	cp_a_u8(val);
	return 1;
}

// CP A,L
uint8_t Cpu::cp_BD() {
	uint8_t val = get_r8(Regs8::L);
	cp_a_u8(val);
	return 1;
}

// CP A,(HL)
uint8_t Cpu::cp_BE() {
	uint8_t val = get_r8(Regs8::HL);
	cp_a_u8(val);
	return 2;
}

// CP A,A
uint8_t Cpu::cp_BF() {
	uint8_t val = get_r8(Regs8::A);
	cp_a_u8(val);
	return 1;
}

///// Row C /////

// RET NZ
uint8_t Cpu::ret_C0() {
	if (!getFlag(Flags::Z)) {
		uint16_t addr = pop();
		setPC(addr);
		return 5;
	}
	else {
		return 2;
	}
}

// POP BC
uint8_t Cpu::pop_C1() {
	uint16_t val = pop();
	set_r16(Regs16::BC, val);
	return 3;
}

// JP NZ,u16
uint8_t Cpu::jp_C2() {
	uint16_t addr = fetch16();
	if (!getFlag(Flags::Z)) {
		setPC(addr);
		return 4;
	}
	else {
		return 3;
	}
}

// JP u16
uint8_t Cpu::jp_C3() {
	uint16_t addr = fetch16();
	setPC(addr);
	return 4;
}

// CALL NZ,u16
uint8_t Cpu::call_C4() {
	uint16_t addr = fetch16();

	if (!getFlag(Flags::Z)) {
		push(getPC());
		setPC(addr);
		return 6;
	}
	else {
		return 3;
	}
}

// PUSH BC
uint8_t Cpu::push_C5() {
	uint16_t val = get_r16(Regs16::BC);
	push(val);
	return 4;
}

// ADD A,u8
uint8_t Cpu::add_C6() {
	uint8_t val = fetch8();
	add_a_u8(val, false);
	return 2;
}

// RST 00
uint8_t Cpu::rst_C7() {
	push(getPC());
	setPC(0x0000);
	return 4;
}

// RET Z
uint8_t Cpu::ret_C8() {
	if (getFlag(Flags::Z)) {
		uint16_t addr = pop();
		setPC(addr);
		return 5;
	}
	else {
		return 2;
	}
}

// RET 
uint8_t Cpu::ret_C9() {
	uint16_t addr = pop();
	setPC(addr);
	return 4;
}

// JP Z,u16
uint8_t Cpu::jp_CA() {
	uint16_t addr = fetch16();
	if (getFlag(Flags::Z)) {
		setPC(addr);
		return 4;
	}
	else {
		return 3;
	}
}

uint8_t Cpu::prefix_CB() {
	uint8_t cb_index = fetch8();
	execute_cb(cb_index);
	return 1;
}

// CALL Z,u16
uint8_t Cpu::call_CC() {
	uint16_t addr = fetch16();

	if (getFlag(Flags::Z)) {
		push(getPC());
		setPC(addr);
		return 6;
	}
	else {
		return 3;
	}
}

// CALL u16
uint8_t Cpu::call_CD() {
	uint16_t addr = fetch16();
	push(getPC());
	setPC(addr);
	return 6;
}

// ADC A,u8
uint8_t Cpu::adc_CE() {
	uint8_t val = fetch8();
	add_a_u8(val, true);
	return 2;
}

// RST 08
uint8_t Cpu::rst_CF() {
	push(getPC());
	setPC(0x0008);
	return 4;
}

///// Row D /////

// RET NC
uint8_t Cpu::ret_D0() {
	if (!getFlag(Flags::C)) {
		uint16_t addr = pop();
		setPC(addr);
		return 5;
	}
	else {
		return 2;
	}
}

// POP DE
uint8_t Cpu::pop_D1() {
	uint16_t val = pop();
	set_r16(Regs16::DE, val);
	return 3;
}

// JP NC,u16
uint8_t Cpu::jp_D2() {
	uint16_t addr = fetch16();
	if (!getFlag(Flags::C)) {
		setPC(addr);
		return 4;
	}
	else {
		return 3;
	}
}

// CALL NC,u16
uint8_t Cpu::call_D4() {
	uint16_t addr = fetch16();

	if (!getFlag(Flags::C)) {
		push(getPC());
		setPC(addr);
		return 6;
	}
	else {
		return 3;
	}
}

// PUSH DE
uint8_t Cpu::push_D5() {
	uint16_t val = get_r16(Regs16::DE);
	push(val);
	return 4;
}
 
// SUB A,u8
uint8_t Cpu::sub_D6() {
	uint8_t val = fetch8();
	sub_a_u8(val, false);
	return 2;
}

// RST 10
uint8_t Cpu::rst_D7() {
	push(getPC());
	setPC(0x0010);
	return 4;
}

// RET C
uint8_t Cpu::ret_D8() {
	if (getFlag(Flags::C)) {
		uint16_t addr = pop();
		setPC(addr);
		return 5;
	}
	else {
		return 2;
	}
}

// RETI
uint8_t Cpu::reti_D9() {
	uint16_t addr = pop();
	setPC(addr);
	set_irq_scheduled(true);
	return 4;
}

// JP C,u16
uint8_t Cpu::jp_DA() {
	uint16_t addr = fetch16();
	if (getFlag(Flags::C)) {
		setPC(addr);
		return 4;
	}
	else {
		return 3;
	}
}

// CALL C,u16
uint8_t Cpu::call_DC() {
	uint16_t addr = fetch16();

	if (getFlag(Flags::C)) {
		push(getPC());
		setPC(addr);
		return 6;
	}
	else {
		return 3;
	}
}

// SBC A,u8
uint8_t Cpu::sbc_DE() {
	uint8_t val = fetch8();
	sub_a_u8(val, true);
	return 2;
}

// RST 18
uint8_t Cpu::rst_DF() {
	push(getPC());
	setPC(0x0018);
	return 4;
}

///// Row E /////

// LD (FF00+u8),A
uint8_t Cpu::ld_E0() {
	uint16_t offset = fetch8();
	uint16_t addr = 0xFF00 + offset;
	uint8_t val = get_r8(Regs8::A);
	writeRAM(addr, val);
	return 3;
}

// POP HL
uint8_t Cpu::pop_E1() {
	uint16_t val = pop();
	set_r16(Regs16::HL, val);
	return 3;
}

// LD (FF00+C),A
uint8_t Cpu::ld_E2() {
	uint16_t addr = 0xFF00 + get_r8(Regs8::C);
	uint8_t val = get_r8(Regs8::A);
	writeRAM(addr, val);
	return 2;
}

// PUSH HL
uint8_t Cpu::push_E5() {
	uint16_t val = get_r16(Regs16::HL);
	push(val);
	return 4;
}

// AND A,u8
uint8_t Cpu::and_E6() {
	uint8_t val = fetch8();
	and_a_u8(val);
	return 2;
}

// RST 20
uint8_t Cpu::rst_E7() {
	push(getPC());
	setPC(0x0020);
	return 4;
}

// ADD SP,i8
uint8_t Cpu::add_E8() {
	uint8_t imm = int8_t(fetch8());
	uint16_t sp = get_r16(Regs16::SP);

	uint8_t lowSP = sp & 0xFF;
	uint8_t imm8 = uint8_t(imm);

	bool halfCarry = ((lowSP & 0xF) + (imm8 & 0xF)) > 0xF;
	bool carry = (lowSP + imm8) > 0xFF;

	uint16_t result = sp + imm;

	setFlag(Flags::Z, false);
	setFlag(Flags::N, false);
	setFlag(Flags::H, halfCarry);
	setFlag(Flags::C, carry);

	set_r16(Regs16::SP, result);
	return 4;
}

// JP HL
uint8_t Cpu::jp_E9() {
	uint16_t addr = get_r16(Regs16::HL);
	setPC(addr);
	return 1;
}

// LD (u16),A
uint8_t Cpu::ld_EA() {
	uint16_t addr = fetch16();
	uint8_t val = get_r8(Regs8::A);
	writeRAM(addr, val);
	return 4;
}

// XOR A,u8
uint8_t Cpu::xor_EE() {
	uint8_t val = fetch8();
	xor_a_u8(val);
	return 2;
}

// RST 28
uint8_t Cpu::rst_EF() {
	push(getPC());
	setPC(0x0028);
	return 4;
}

///// Row F /////

// LD A,(FF00+u8)
uint8_t Cpu::ld_F0() {
	uint16_t addr = 0xFF00 + fetch8();
	uint8_t val = readRAM(addr);
	set_r8(Regs8::A, val);
	return 3;
}

// POP AF
uint8_t Cpu::pop_F1() {
	uint16_t val = pop();
	set_r16(Regs16::AF, val);
	return 3;
}

// LD A,(FF00+C)
uint8_t Cpu::ld_F2() {
	uint16_t addr = 0xFF00 + get_r8(Regs8::C);
	uint8_t val = readRAM(addr);
	set_r8(Regs8::A, val);
	return 2;
}

// DI
uint8_t Cpu::di_F3() {
	set_irq(false);
	set_irq_scheduled(false);
	return 1;
}

// PUSH AF
uint8_t Cpu::push_F5() {
	uint16_t val = get_r16(Regs16::AF);
	push(val);
	return 4;
}

// OR A,u8
uint8_t Cpu::or_F6() {
	uint8_t val = fetch8();
	or_a_u8(val);
	return 2;
}

// RST F7
uint8_t Cpu::rst_F7() {
	push(getPC());
	setPC(0x0030);
	return 4;
}

// LD HL,SP+i8
uint8_t Cpu::ld_F8() {
	int8_t offset = static_cast<int8_t>(fetch8());
	uint16_t sp = get_r16(Regs16::SP);
	bool setC = check_c_carry_u8(getLowByte(sp), getLowByte(offset));
	bool setH = check_h_carry_u8(getLowByte(sp), getLowByte(offset));
	
	set_r16(Regs16::HL, offset + sp);
	setFlag(Flags::Z, false);
	setFlag(Flags::N, false);
	setFlag(Flags::C, setC);
	setFlag(Flags::H, setH);

	return 3;
}

// LD SP,HL
uint8_t Cpu::ld_F9() {
	uint16_t val = get_r16(Regs16::HL);
	set_r16(Regs16::SP, val);
	return 2;
}

// LD A,(u16)
uint8_t Cpu::ld_FA() {
	uint16_t addr = fetch16();
	uint8_t val = readRAM(addr);
	set_r8(Regs8::A, val);
	return 4;
}

// EI
uint8_t Cpu::ei_FB() {
	set_irq_scheduled(true);
	return 1;
}

// CP A,u8
uint8_t Cpu::cp_FE() {
	uint8_t val = fetch8();
	cp_a_u8(val);
	return 2;
}

// RST 38
uint8_t Cpu::rst_FF() {
	push(getPC());
	setPC(0x0038);
	return 4;
}