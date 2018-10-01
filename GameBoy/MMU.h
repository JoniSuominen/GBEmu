#include <iostream>
#pragma once
class MMU {
	private:
		char bios = 1;
		uint8_t bios[0x100]; // 256 bytes
		uint8_t rom[0x8000]; // 32768 bytes
		uint8_t wram[0x2000]; // 8192 bytes
		uint8_t eram[0x2000]; // 8192 bytes
		uint8_t zram[0x80]; // 128 bytes


	public:
	uint8_t readMemory(uint16_t address);
};
