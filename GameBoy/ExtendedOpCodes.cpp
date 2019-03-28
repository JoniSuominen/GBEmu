#include "CPU.h"

// BIT n, reg
void CPU::test_bit(int bit, uint8_t reg)
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

// BIT n, (HL)	
void CPU::test_bitMEM(int bit, uint16_t reg)
{
	uint8_t loc = Memory.readMemory(reg);
	test_bit(bit, loc);
	cycles += 8;
}


// SWAP B
void CPU::ext_swap(uint8_t & reg)
{
	uint8_t before = reg;
	uint8_t lower = (reg & 0x0F) << 4;
	uint8_t higher = reg >> 4;
	reg = (lower & higher);
	registerAF.lo = 0;
	if (reg == 0) {
		bitset(FLAG_Z);
	}
	cycles += 8;
}


// RLC, (HL)
// calls the function in regular jumptable for effeciency
void CPU::ext_rlc(uint16_t pointer) {
	uint8_t address = Memory.readMemory(pointer);
	rlc_reg8(address);
	Memory.writeMemory(pointer, address);
	cycles += 8;
}

// RRC, (HL)
// calls the function in regular jumptable for effeciency
void CPU::ext_rrc(uint16_t pointer)
{
	uint8_t address = Memory.readMemory(pointer);
	rrc_reg8(address);
	Memory.writeMemory(pointer, address);
	cycles += 8;
}

// RL C
// calls the function in regular jumptable for effeciency
void CPU::ext_rl(uint16_t pointer)
{
	uint8_t address = Memory.readMemory(pointer);
	rl_reg8(address);
	Memory.writeMemory(pointer, address);
	cycles += 8;
}

// RR C
// calls the function in regular jumptable for effeciency
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

// SRA
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

// SRL
void CPU::ext_srl(uint8_t & address)
{
	// saved to store into carry
	int lsbData = getBit(0, address);
	address >>= 1;
	registerAF.lo = 0;
	if (address == 0) bitset(FLAG_Z);
	if (lsbData != 0) bitset(FLAG_C);
	bitreset(FLAG_N);
	bitreset(FLAG_H);
	cycles += 8;
}

// SRL, (HL)
// calls the regular SRL-function for effeciency
void CPU::ext_SRLHL(uint16_t pointer) {
	uint8_t loc = Memory.readMemory(pointer);
	ext_srl(loc);
	Memory.writeMemory(pointer, loc);
	cycles += 8;
}

// SRA, (HL)
// calls the regular SRL-function for effeciency
void CPU::ext_SRAHL(uint16_t pointer) {
	uint8_t loc = Memory.readMemory(pointer);
	ext_sra(loc);
	Memory.writeMemory(pointer, loc);
	cycles += 8;
}

// SLA, (HL)
// calls the regular SRL-function for effeciency
void CPU::ext_SLAHL(uint16_t pointer) {
	uint8_t loc = Memory.readMemory(pointer);
	ext_sla(loc);
	Memory.writeMemory(pointer, loc);
	cycles += 8;
}

// RES BIT, REGISTER
void CPU::ext_reset(int bit, uint8_t & address)
{	
	address = reset_bit(address, bit);
	cycles += 8;
}

// RES BIT, (HL)
void CPU::ext_resetHL(int bit, uint16_t address)
{
	uint8_t loc = Memory.readMemory(address);
	ext_reset(bit, loc);
	Memory.writeMemory(address, loc);
	cycles += 8;
}

void CPU::ext_set(int bit, uint8_t& address)
{
	address = set_bit(address, bit);
	cycles += 8;
}


void CPU::ext_setHL(int bit, uint16_t address)
{
	uint8_t loc = Memory.readMemory(address);
	ext_set(bit, loc);
	Memory.writeMemory(address, loc);
	cycles += 8;
}




