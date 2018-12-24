#include "CPU.h"

// Relative jump signed by immediate
// JR, N
void CPU::jump_n()
{
	int8_t signedn = int8_t(Memory.readMemory(this->pc));
	this->pc++;
	this->pc += signedn;
}
// JR Z, n
void CPU::jump_zero()
{
	if (getBit(FLAG_Z) == 0) {
		jump_n();
	}
}
// JR C,n
void CPU::jump_carry()
{
	if (getBit(FLAG_C) == 1) {
		jump_n();
	}
}
// JR NZ, n
void CPU::jump_notzero()
{
	if (getBit(FLAG_Z) != 0) {
		jump_n();
	}
}

// JR, NC
void CPU::jump_notcarry()
{
	if (getBit(FLAG_C) == 0) {
		jump_n();
	}
}
// JP nn
void CPU::jump_abs()
{
	uint16_t jump = jump16();
	this->pc = jump;
}
void CPU::jump_absNZ()
{
	if (getBit(FLAG_Z) != 0) jump_abs();
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

void CPU::opcode_CP(uint8_t reg1, uint8_t reg2)
{
	if (reg1 - reg2 == 0) {
		bitset(FLAG_Z);
	}
	if ((reg1 & 0x0F) > (reg2 & 0x0F)) {
		bitset(FLAG_H);
	}

	if (reg2 > reg1) {
		bitset(FLAG_C);
	}

	bitset(FLAG_N);
}

void CPU::opcode_CPmmu(uint8_t reg1, uint16_t pointer)
{
	uint8_t val2 = Memory.readMemory(pointer);
	opcode_CP(reg1, val2);
}

void CPU::opcode_popMmu(uint16_t & reg)
{
	uint16_t value = readFromStack();
}

void CPU::call_nz()
{
	if (getBit(FLAG_Z) != 0) {
		call_nn();
	}
}

void CPU::call_nn()
{
	uint16_t imd = readTwoBytes();
	this->pc += 2;
	writeToStack(pc);
	this->pc = readTwoBytes();
}

void CPU::push_reg16(uint16_t reg)
{
	writeToStack(reg);
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

void CPU::opcode_copy8(uint8_t value, uint8_t & destination)
{

}

void CPU::opcode_mmucopy16(uint16_t source, uint8_t & destination)
{
	destination = Memory.readMemory(source);
}

void CPU::opcode_mmucopy8(uint16_t mmulocation, uint8_t data)
{
	Memory.writeMemory(mmulocation, data);
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
void CPU::add_16(uint16_t & destination, uint16_t & source)
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

// ADD A, B
void CPU::add_8(uint8_t value, uint8_t & destination)
{
	uint16_t checksum = value + destination;
	if (checksum == 0) {
		bitset(FLAG_Z);
	}
	bitreset(FLAG_N);

	if ((value & 0x0F) + (destination & 0x0F) > 0x0F) {
		bitset(FLAG_H);
	}

	if (checksum > 0xFF) {
		bitset(FLAG_C);
	}

	destination += value;
}

void CPU::add_mmu(uint16_t value, uint8_t & dest)
{
	uint8_t valueToSum = Memory.readMemory(value);
	uint16_t checksum = valueToSum + dest;
	if (checksum == 0) {
		bitset(FLAG_Z);
	}
	bitreset(FLAG_N);

	if ((valueToSum & 0x0F) + (dest & 0x0F) > 0x0F) {
		bitset(FLAG_H);
	}

	if (checksum > 0xFF) {
		bitset(FLAG_C);
	}

	dest += valueToSum;
}

void CPU::adc_reg8(uint8_t source, uint8_t & destination)
{
	int carry = getBit(FLAG_C);
	if (destination + source + carry == 0) {
		bitset(FLAG_Z);
	}

	bitreset(FLAG_N);

	if ((source & 0x0F) + (destination & 0x0F) + carry > 0x0F) {
		bitset(FLAG_H);
	}

	if (source + destination + carry > 0xFF) {
		bitset(FLAG_C);
	}

	destination = source + carry;
}

void CPU::adc_imm(uint16_t pointer, uint8_t & destination)
{
	uint8_t source = Memory.readMemory(pointer);
	this->adc_reg8(source, destination);
}

void CPU::sub_reg8(uint8_t value, uint8_t & destination)
{
	if (destination - value == 0) {
		bitset(FLAG_Z);
	}
	bitset(FLAG_N);

	if ((destination & 0x0F) < (value & 0x0F)) {
		bitset(FLAG_H);
	}

	if (destination - value < 0) {
		bitset(FLAG_C);
	}
}

void CPU::sub_mmu(uint16_t value, uint8_t & destination)
{
	uint8_t value = Memory.readMemory(value);
	sub_reg8(value, destination);
}

void CPU::sbc_reg8(uint16_t pointer, uint8_t & destination)
{
	uint8_t value = Memory.readMemory(pointer) + getBit(FLAG_C);
	sub_reg8(value, destination);
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
void CPU::and_reg8(uint8_t value, uint8_t & destination)
{
	destination &= value;
	if (destination == 0) {
		bitset(FLAG_Z);
	}

	bitreset(FLAG_N);
	bitset(FLAG_H);
	bitreset(FLAG_C);

}
void CPU::and_mmu(uint16_t value, uint8_t & destination)
{
	uint8_t and = Memory.readMemory(value);
	this->and_reg8(and, destination);
}
void CPU::xor_reg8(uint8_t value, uint8_t & destination)
{
	destination ^= value;

	if (destination == 0) {
		bitset(FLAG_Z);
	}
	bitreset(FLAG_N);
	bitreset(FLAG_H);
	bitreset(FLAG_C);

}
void CPU::xor_mmu(uint16_t value, uint8_t & destination)
{
	uint8_t val2 = Memory.readMemory(value);
	xor_reg8(value, destination);
}
void CPU::or_reg8(uint8_t value, uint8_t & destination)
{
	destination |= value;

	if (destination == 0) {
		bitset(FLAG_Z);
	}
	bitreset(FLAG_N);
	bitreset(FLAG_H);
	bitreset(FLAG_C);
}
void CPU::or_mmu(uint16_t pointer, uint8_t & destination)
{
	uint8_t value = Memory.readMemory(pointer);
	or_reg8(value, destination);
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
void CPU::restart(uint8_t n)
{
	writeToStack(this->pc);
	this->pc = n;
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

void CPU::opcode_ret()
{
	this->pc = readFromStack();
}

void CPU::opcode_retNZ()
{
	if (getBit(FLAG_Z) == 0) {
		opcode_ret();
	}
}	





