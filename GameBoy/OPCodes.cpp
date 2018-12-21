#include "CPU.h"

// Relative jump signed by immediate
// JR, N
void CPU::jump_n(uint8_t n)
{
	int8_t signedn = int8_t(Memory.readMemory(this->pc));
	this->pc++;
	this->pc += n;
}

void CPU::jump_zero(uint8_t n)
{
	if (getBit(FLAG_Z) == 0) {
		jump_n(n);
	}
}
// LDI (HL), A
void CPU::mmu_ldi(uint16_t &address, uint8_t &data)
{
	mmu_load8(address, data);
	address += 1;
}
	
// LD (BC), A
void CPU::mmu_load8(uint16_t address, uint8_t data )
{
	Memory.writeMemory(address, data);
}


void CPU::mmu_load16(int16_t address, uint8_t data)
{
	Memory.writeMemory(address, data);
}

// do nothing
void CPU::opcode_nop() {
	cout << "no operation" << endl;
}

void CPU::opcode_cpl(uint8_t & value)
{
	value = ~value;
	bitset(FLAG_N);
	bitset(FLAG_H);
}

void CPU::opcode_scf()
{
	bitset(FLAG_C);
	bitreset(FLAG_N);
	bitreset(FLAG_H);
}

// DAA
void CPU::opcode_bcd(uint8_t & value)
{
	if (this->registerAF.hi >> FLAG_N == 0) {
		if (this->registerAF.hi >> FLAG_C == 1 || value > 0x99) {
			if (value + 0x6 > 0xFF) {
				bitset(FLAG_C);
			}
			value += 0x60;
			bitset(FLAG_C);
		}
		if (this->registerAF.hi >> FLAG_H == 1 || (value & 0x0f) < 0x09) {
			if (value + 0x6 > 0xFF) {
				bitset(FLAG_C);
			}
			value += 0x6;
		
		}
	}
	else {
		if (this->registerAF.hi >> FLAG_Z == 1) {
			if (value - 0x6 > 0xFF) {
				bitset(FLAG_C);
			}
			value -= 0x60;
		}
		if (this->registerAF.hi >> FLAG_H == 1) {
			if (value - 0x6 > 0xFF) {
				bitset(FLAG_C);
			}
			value -= 0x6;
		}
	}

	if (value == 0) {
		bitset(FLAG_Z);
	}

	bitreset(FLAG_H);

}

// load A from address pointed by (BC)
// LD A, (BC)
void CPU::opcode_load8(uint16_t address, uint8_t &destination) {
	destination = Memory.readMemory(address);
}

void CPU::opcode_ldi8(uint16_t & address, uint8_t & destination)
{
	opcode_load8(address, destination);
	address++;
}

// load immediate data from memory into 16-bit register
// LD BC, nn
void CPU::reg16_load(Register *reg)
{
	reg->lo = Memory.readMemory(this->pc);
	reg->hi = Memory.readMemory(this->pc+1);
}

// load 8-bit immediate into register
void CPU::reg8_load(uint8_t &address)
{
	address = Memory.readMemory(this->pc);
}



// ADD HL, BC
void CPU::add_2(uint16_t & destination, uint16_t & source)
{
	bitreset(FLAG_Z);
	if ((destination & 0x0FFF) + (source & 0x0FFF) > 0x0FFF) {
		bitset(FLAG_H);
	}
	if (destination + source > 0xFFFF) {
		bitset(FLAG_C);
	}

	destination += source;
}

// increment 16-bit register by one
// INC BC
void CPU::incr_reg(uint16_t & address)
{
	address += 1;
}

// increment 8-bit register by one
// INC B
void CPU::incr_reg(uint8_t & address)
{
	int original = address >> 2;
	address += 1;
	if (address == 0) {
		bitset(FLAG_Z);
	}

	if (original == address >> 2) {
		bitset(FLAG_H);
	}

}
void CPU::incp_reg(int16_t address)
{
	uint8_t value = Memory.readMemory(address);
	if (value + 1 > 0xFF) {
		bitset(FLAG_C);
		return;
	}
	value++;
	Memory.writeMemory(value);
}
// increment 8-bit register by one
// DEC B
void CPU::decr_reg(uint8_t & address)
{
	int original = address >> 3;
	address -= 1;

	if (address == 0) {
		bitset(FLAG_Z);
	}

	if (original == address >> 3) {
		bitset(FLAG_H);
	}
}
// DEC BC
void CPU::decr_reg(uint16_t & address)
{
	int original = address >> 3;
	address -= 1;

}
// DEC (HL)
void CPU::decp_reg(uint16_t value)
{
	uint8_t newValue = Memory.readMemory(value);
	if (newValue == 0) {
		bitset(FLAG_C);
		return;
	}
	value--;
	Memory.writeMemory(newValue);
}
/* Rotate left with carry
   RLC A
*/
void CPU::rlc_reg8(uint8_t &address)
{
	uint8_t original = address;
	address = original << 1;
	uint8_t lsb = original >> 7;
	address = address | lsb;
	cout << std::bitset<8>(address) << endl;
	bitset(FLAG_C, lsb);
	if (address == 0) {
		bitset(FLAG_Z);
	}
}

/* Rotate right with carry
   RRC A
*/
void CPU::rrc_reg8(uint8_t & address)
{
	uint8_t original = address;
	address = original >> 1;
	uint8_t lsb = original & 0b00000001;
	bitset(FLAG_C, lsb);
	if (address == 0) {
		bitset(FLAG_Z);
	}
}

// Rotate A left, documentation says this is the same exact opcode as RLC, A: unsure if this should get implemented?
// RL, A
void CPU::rl_reg8(int8_t & address)
{
}





