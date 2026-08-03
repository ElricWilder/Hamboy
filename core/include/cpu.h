#pragma once

#include <array>
#include "cstdint"
#include "bus.h"
#include <optional>

// LR35902 chip (DMG-CPU) has eight 8-bit registers A B C D E F H L
// that can be combined into 16-bit registers AF BC DE HL
// 
// Register A is used a accumulator for math operations
// 
// Register F is used for flags
// 
// Registers H and L are high/low registers
// 
// There is also a 16-bit Program Counter and Stack Pointer


struct Cpu {
	uint16_t pc;
	uint16_t sp;
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t f;
	uint8_t h;
	uint8_t l;
	bool irq_enabled;
	bool irq_enabled_scheduled;
	bool halted;
	bool haltBug;
	const uint16_t IF = 0xFF0F;
	const uint16_t IE = 0xFFFF;
	uint16_t totalCycles;

	Bus bus;

	enum class Regs8 {
		A,
		B,
		C,
		D,
		E,
		F,
		H,
		L,
		HL,
	};

	enum class Regs16 {
		AF,
		BC,
		DE,
		HL,
		SP,
	};

	enum class Flags {
		Z,
		N,
		H,
		C,
	};

	enum class Interrupts : uint8_t {
		Vblank,
		Stat,
		Timer,
		Serial,
		Joypad
	};

	const Interrupts IRQ_PRIORITIES[5] = {
	Interrupts::Vblank,
	Interrupts::Stat,
	Interrupts::Timer,
	Interrupts::Serial,
	Interrupts::Joypad
	};

	// Opcode/opcode table declarations
	using Opcode = uint8_t(Cpu::*)();
	static std::array<Opcode, 256> opcodes;

	uint8_t op_unimplemented();
	uint8_t invalid();

	uint8_t nop_00();
	uint8_t  ld_01();
	uint8_t  ld_02();
	uint8_t inc_03();
	uint8_t inc_04();
	uint8_t dec_05();
	uint8_t  ld_06();
	uint8_t rlca_07();
	uint8_t  ld_08();
	uint8_t add_09();
	uint8_t  ld_0A();
	uint8_t dec_0B();
	uint8_t inc_0C();
	uint8_t dec_0D();
	uint8_t  ld_0E();
	uint8_t rrca_0F();

	uint8_t stop_10();
	uint8_t  ld_11();
	uint8_t  ld_12();
	uint8_t inc_13();
	uint8_t inc_14();
	uint8_t dec_15();
	uint8_t  ld_16();
	uint8_t rla_17();
	uint8_t  jr_18();
	uint8_t add_19();
	uint8_t  ld_1A();
	uint8_t dec_1B();
	uint8_t inc_1C();
	uint8_t dec_1D();
	uint8_t  ld_1E();
	uint8_t rra_1F();

	uint8_t  jr_20();
	uint8_t  ld_21();
	uint8_t  ld_22();
	uint8_t inc_23();
	uint8_t inc_24();
	uint8_t dec_25();
	uint8_t  ld_26();
	uint8_t daa_27();
	uint8_t  jr_28();
	uint8_t add_29();
	uint8_t  ld_2A();
	uint8_t dec_2B();
	uint8_t inc_2C();
	uint8_t dec_2D();
	uint8_t  ld_2E();
	uint8_t cpl_2F();

	uint8_t  jr_30();
	uint8_t  ld_31();
	uint8_t  ld_32();
	uint8_t inc_33();
	uint8_t inc_34();
	uint8_t dec_35();
	uint8_t  ld_36();
	uint8_t scf_37();
	uint8_t  jr_38();
	uint8_t add_39();
	uint8_t  ld_3A();
	uint8_t dec_3B();
	uint8_t inc_3C();
	uint8_t dec_3D();
	uint8_t  ld_3E();
	uint8_t ccf_3F();

	uint8_t  ld_40();
	uint8_t  ld_41();
	uint8_t  ld_42();
	uint8_t  ld_43();
	uint8_t  ld_44();
	uint8_t  ld_45();
	uint8_t  ld_46();
	uint8_t  ld_47();
	uint8_t  ld_48();
	uint8_t  ld_49();
	uint8_t  ld_4A();
	uint8_t  ld_4B();
	uint8_t  ld_4C();
	uint8_t  ld_4D();
	uint8_t  ld_4E();
	uint8_t  ld_4F();

	uint8_t  ld_50();
	uint8_t  ld_51();
	uint8_t  ld_52();
	uint8_t  ld_53();
	uint8_t  ld_54();
	uint8_t  ld_55();
	uint8_t  ld_56();
	uint8_t  ld_57();
	uint8_t  ld_58();
	uint8_t  ld_59();
	uint8_t  ld_5A();
	uint8_t  ld_5B();
	uint8_t  ld_5C();
	uint8_t  ld_5D();
	uint8_t  ld_5E();
	uint8_t  ld_5F();

	uint8_t  ld_60();
	uint8_t  ld_61();
	uint8_t  ld_62();
	uint8_t  ld_63();
	uint8_t  ld_64();
	uint8_t  ld_65();
	uint8_t  ld_66();
	uint8_t  ld_67();
	uint8_t  ld_68();
	uint8_t  ld_69();
	uint8_t  ld_6A();
	uint8_t  ld_6B();
	uint8_t  ld_6C();
	uint8_t  ld_6D();
	uint8_t  ld_6E();
	uint8_t  ld_6F();

	uint8_t  ld_70();
	uint8_t  ld_71();
	uint8_t  ld_72();
	uint8_t  ld_73();
	uint8_t  ld_74();
	uint8_t  ld_75();
	uint8_t halt_76();
	uint8_t  ld_77();
	uint8_t  ld_78();
	uint8_t  ld_79();
	uint8_t  ld_7A();
	uint8_t  ld_7B();
	uint8_t  ld_7C();
	uint8_t  ld_7D();
	uint8_t  ld_7E();
	uint8_t  ld_7F();

	uint8_t add_80();
	uint8_t add_81();
	uint8_t add_82();
	uint8_t add_83();
	uint8_t add_84();
	uint8_t add_85();
	uint8_t add_86();
	uint8_t add_87();
	uint8_t adc_88();
	uint8_t adc_89();
	uint8_t adc_8A();
	uint8_t adc_8B();
	uint8_t adc_8C();
	uint8_t adc_8D();
	uint8_t adc_8E();
	uint8_t adc_8F();

	uint8_t sub_90();
	uint8_t sub_91();
	uint8_t sub_92();
	uint8_t sub_93();
	uint8_t sub_94();
	uint8_t sub_95();
	uint8_t sub_96();
	uint8_t sub_97();
	uint8_t sbc_98();
	uint8_t sbc_99();
	uint8_t sbc_9A();
	uint8_t sbc_9B();
	uint8_t sbc_9C();
	uint8_t sbc_9D();
	uint8_t sbc_9E();
	uint8_t sbc_9F();

	uint8_t and_A0();
	uint8_t and_A1();
	uint8_t and_A2();
	uint8_t and_A3();
	uint8_t and_A4();
	uint8_t and_A5();
	uint8_t and_A6();
	uint8_t and_A7();
	uint8_t xor_A8();
	uint8_t xor_A9();
	uint8_t xor_AA();
	uint8_t xor_AB();
	uint8_t xor_AC();
	uint8_t xor_AD();
	uint8_t xor_AE();
	uint8_t xor_AF();

	uint8_t or_B0();
	uint8_t or_B1();
	uint8_t or_B2();
	uint8_t or_B3();
	uint8_t or_B4();
	uint8_t or_B5();
	uint8_t or_B6();
	uint8_t or_B7();
	uint8_t cp_B8();
	uint8_t cp_B9();
	uint8_t cp_BA();
	uint8_t cp_BB();
	uint8_t cp_BC();
	uint8_t cp_BD();
	uint8_t cp_BE();
	uint8_t cp_BF();

	uint8_t ret_C0();
	uint8_t pop_C1();
	uint8_t jp_C2();
	uint8_t jp_C3();
	uint8_t call_C4();
	uint8_t push_C5();
	uint8_t add_C6();
	uint8_t rst_C7();
	uint8_t ret_C8();
	uint8_t ret_C9();
	uint8_t jp_CA();
	uint8_t prefix_CB();
	uint8_t call_CC();
	uint8_t call_CD();
	uint8_t adc_CE();
	uint8_t rst_CF();

	uint8_t ret_D0();
	uint8_t pop_D1();
	uint8_t jp_D2();
	uint8_t call_D4();
	uint8_t push_D5();
	uint8_t sub_D6();
	uint8_t rst_D7();
	uint8_t ret_D8();
	uint8_t reti_D9();
	uint8_t jp_DA();
	uint8_t call_DC();
	uint8_t sbc_DE();
	uint8_t rst_DF();

	uint8_t ld_E0();
	uint8_t pop_E1();
	uint8_t ld_E2();
	uint8_t push_E5();
	uint8_t and_E6();
	uint8_t rst_E7();
	uint8_t add_E8();
	uint8_t jp_E9();
	uint8_t ld_EA();
	uint8_t xor_EE();
	uint8_t rst_EF();

	uint8_t ld_F0();
	uint8_t pop_F1();
	uint8_t ld_F2();
	uint8_t di_F3();
	uint8_t push_F5();
	uint8_t or_F6();
	uint8_t rst_F7();
	uint8_t ld_F8();
	uint8_t ld_F9();
	uint8_t ld_FA();
	uint8_t ei_FB();
	uint8_t cp_FE();
	uint8_t rst_FF();

	Cpu();

	uint8_t execute();
	bool tick();

	std::optional<Cpu::Interrupts> checkIrq();
	void enableIrqType(Interrupts irq, bool enabled);
	void triggerIrq(Interrupts irq);

	uint8_t get_r8(Regs8 r) const;
	uint16_t get_r16(Regs16 r) const;

	void set_r8(Regs8 r, uint8_t val);
	void set_r16(Regs16 r, uint16_t val);

	bool getFlag(Flags f) const;
	void setFlag(Flags f, bool val);

	uint8_t fetch8();
	uint16_t fetch16();

	uint8_t readRAM(uint16_t addr) const;
	void writeRAM(uint16_t addr, uint8_t val);

	void loadROM(const std::vector<uint8_t>& rom);

	void dec_r16(Regs16 r);
	void inc_r16(Regs16 r);

	void dec_r8(Regs8 r);
	void inc_r8(Regs8 r);

	void and_a_u8(uint8_t val);
	void or_a_u8(uint8_t val);
	void xor_a_u8(uint8_t val);
	void add_a_u8(uint8_t val, bool adc);
	void add_r16(Cpu::Regs16 dst_r, Cpu::Regs16 src_r);
	void sub_a_u8(uint8_t val, bool adc);
	void cp_a_u8(uint8_t val);

	uint16_t pop();
	void push(uint16_t val);

	uint16_t getPC();
	void setPC(uint16_t val);

	bool getBit(Regs8 r, uint8_t bit);
	void setBit(Regs8 r, uint8_t bit, bool set);

	Cpu::Regs8 get_cb_reg(uint8_t op);
	uint8_t execute_cb(uint8_t op);

	void rotate_left(Regs8 r, bool carry);
	void rotate_right(Regs8 r, bool carry);

	void shift_left(Regs8 r);
	void shift_right(Regs8 r, bool arith);
	void swap_bits(Regs8 r);

	void test_bit(Regs8 r, uint8_t bit);
	void write_bit(Regs8 r, uint8_t bit, bool set);

	void set_irq(bool enabled);
	void set_irq_scheduled(bool enabled);
	void set_halted(bool halted);

	uint16_t getVector(Interrupts intr);

	std::array<uint8_t, DISPLAY_BUFFER> render();

	void pressButton(Buttons button, bool pressed);
	std::string getTitle() const;
	void logState();
	void writeIF_raw(uint8_t val);
};