#include "CPU.h"
// BIT n, reg
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

// SWAP B
void CPU::ext_swap(uint8_t & reg)
{
	uint8_t lower = (reg & 0x0F) << 4;
	uint8_t higher = reg >> 4;
	reg = (lower & higher);
	cycles += 8;
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

// RL C
void CPU::ext_rl(uint16_t pointer)
{
	uint8_t address = Memory.readMemory(pointer);
	rl_reg8(address);
	Memory.writeMemory(pointer, address);
	cycles += 8;
}

// RR C
void CPU::ext_rr(uint16_t pointer)
{
	uint8_t address = Memory.readMemory(pointer);
	rr_reg8(address);
	Memory.writeMemory(pointer, address);
	cycles += 8;
}

//SLA
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

	address = set_bit(address, msb);
}

void CPU::ext_sra(uint8_t & address)
{
	int msb = address >> 7;
	address >>= 1;
	bitset(FLAG_C, msb);
	bitreset(FLAG_N);
	bitreset(FLAG_H);
	if (address == 0) {
		bitset(FLAG_Z);
	}

	address = set_bit(address, msb);
}

