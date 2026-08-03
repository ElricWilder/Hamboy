#include <cstdint>
#include "utils.h"


// Helper function to combine two bytes
uint16_t merge_bytes(uint8_t high, uint8_t low) {
	return (static_cast<uint16_t>(high) << 8 | low);
}

// Helper functions to get high and low bytes from a 16-bit value
uint8_t getHighByte(uint16_t val) {
	return val >> 8;
}

uint8_t getLowByte(uint16_t val) {
	return val & 0xFF;
}

// Helper functions to checck carry and borrow flags
// We check for overflow via comparison 
bool check_c_carry_u8(uint8_t left, uint8_t right) {
	return uint16_t(left) + uint16_t(right) > 0xFF;
}

bool check_c_carry_u16(uint16_t left, uint16_t right) {
	return uint32_t(left) + uint32_t(right) > 0xFFFF;
}

bool check_c_borrow_u8(uint8_t left, uint8_t right) {
	return left < right;
}

bool check_c_borrow_u16(uint16_t left, uint16_t right) {
	return left < right;
}

// Half carry flags
bool check_h_carry_u8(uint8_t left, uint8_t right) {
	return (((left & 0xF) + (right & 0xF)) & 0xF0) != 0;
}

bool check_h_carry_u16(uint16_t left, uint16_t right) {
	return (((left & 0xFFF) + (right & 0xFFF)) & 0xF000) != 0;
}

bool check_h_borrow_u8(uint8_t left, uint8_t right) {
	return (left & 0xF) < (right & 0xF);
}

bool check_h_borrow_u16(uint16_t left, uint16_t right) {
	return (left & 0x0FFF) < (right & 0x0FFF);
}

bool get_bit8(uint8_t bit, uint8_t val) {
	uint8_t mask = 0b1 << bit;
	return (val & mask) != 0;
}

bool get_bit16(uint8_t bit, uint16_t val) {
	uint16_t mask = 0b1 << bit;
	return (val & mask) != 0;
}

uint8_t set_bit8(uint8_t bit, uint8_t val, bool set) {
	uint8_t mask = 0b1 << bit;
	if (set) {
		return (val | mask);
	}
	else {
		return (val & ~mask);
	}
}

uint16_t set_bit16(uint8_t bit, uint16_t val, bool set) {
	uint16_t mask = 0b1 << bit;
	if (set) {
		return (val | mask);
	}
	else {
		return (val & ~mask);
	}
}

std::array<uint8_t, 4> unpack_u8(uint8_t val) {
	std::array<uint8_t, 4> output;

	output[0] = val & 0b00000011;
	output[1] = (val >> 2) & 0b00000011;
	output[2] = (val >> 4) & 0b00000011;
	output[3] = (val >> 6) & 0b00000011;
	return output;
}

uint8_t pack_u8(std::array<uint8_t, 4>& a) {
	uint8_t output = a[0];
	output |= a[1] << 2;
	output |= a[2] << 4;
	output |= a[3] << 6;
	return output;
}