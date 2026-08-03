#pragma once
#include <cstdint>
#include <array>

const uint32_t SCREEN_WIDTH = 160;
const uint32_t SCREEN_HEIGHT = 144;
const uint32_t DISPLAY_BUFFER = SCREEN_WIDTH * SCREEN_HEIGHT * 4;

static constexpr std::array<std::array<uint8_t, 4>, 4> GB_PALETTE = { {
	{{255, 255, 255, 255}},
	{{128, 128, 128, 255}},
	{{64, 64, 64, 255}},
	{{0, 0, 0, 255}}
} };

struct Point {
	size_t x;
	size_t y;

	Point(size_t x, size_t y)
		: x(x), y(y) {}
};

uint16_t merge_bytes(uint8_t high, uint8_t low);

uint8_t getHighByte(uint16_t val);

uint8_t getLowByte(uint16_t val);

bool check_c_carry_u8(uint8_t left, uint8_t right);

bool check_c_carry_u16(uint16_t left, uint16_t right);

bool check_c_borrow_u8(uint8_t left, uint8_t right);

bool check_c_borrow_u16(uint16_t left, uint16_t right);

bool check_h_carry_u8(uint8_t left, uint8_t right);

bool check_h_carry_u16(uint16_t left, uint16_t right);

bool check_h_borrow_u8(uint8_t left, uint8_t right);

bool check_h_borrow_u16(uint16_t left, uint16_t right);

bool get_bit8(uint8_t bit, uint8_t val);

bool get_bit16(uint8_t bit, uint16_t val);

uint8_t set_bit8(uint8_t bit, uint8_t val, bool set);

uint16_t set_bit16(uint8_t bit, uint16_t val, bool set);

std::array<uint8_t, 4> unpack_u8(uint8_t val);

uint8_t pack_u8(std::array<uint8_t, 4>& a);

