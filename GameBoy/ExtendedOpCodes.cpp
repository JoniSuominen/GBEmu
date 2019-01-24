#include "CPU.h"

void CPU::test_bit(int bit, uint16_t reg)
{
	int num = getBit(bit, reg);
	if (num == 0) {
		bitset(FLAG_Z);
	}
	else {
		bitreset(FLAG_Z);
	}

	bitreset(FLAG_N);
	bitset(FLAG_H);
}


// RLC, (HL)
void CPU::ext_rlc(uint16_t pointer) {
	uint8_t address = Memory.readMemory(pointer);
	rlc_reg8(address);
	Memory.writeMemory(pointer, address);
	cycles += 8;
}

// RRC, (HL)
void CPU::ext_rrc(uint16_t pointer)
{
	uint8_t address = Memory.readMemory(pointer);
	rrc_reg8(address);
	Memory.writeMemory(pointer, address);
	cycles += 8;
}

void CPU::ext_rl(uint16_t pointer)
{
	uint8_t address = Memory.readMemory(pointer);
	rl_reg8(address);
	Memory.writeMemory(pointer, address);
	cycles += 8;
}

void CPU::ext_rr(uint16_t pointer)
{
	uint8_t address = Memory.readMemory(pointer);
	rr_reg8(address);
	Memory.writeMemory(pointer, address);
	cycles += 8;
}

void CPU::ext_sla(uint8_t & address)
{
	int msb = address >> 7;
	address <<= 1;
	bitset(FLAG_C, msb);
	bitreset(FLAG_N);
	bitreset(FLAG_H);
	if (address == 0) {
		bitset(FLAG_Z);
	}

	bitset(FLAG_C, msb);
}

