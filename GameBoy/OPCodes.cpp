#include "CPU.h"

// Relative jump signed by immediate
// JR, N
void CPU::jump_n()
{
	int8_t signedn = int8_t(Memory.readMemory(this->pc));
	this->pc += signedn;
	this->pc++;
	cycles += 8;
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
	cycles += 12;
}
	
// JP NZ, nn etc..
void CPU::jump_absFalse(int flag)
{
	if (getBit(flag) != 0) jump_abs();
}
// JP Z, nn etc
void CPU::jump_absTrue(int flag)
{
	if (getBit(flag)) jump_abs();
}
// JP (HL)
void CPU::jump_mmu(uint16_t reg)
{
	uint8_t address = Memory.readMemory(reg);
	this->pc = address;
	cycles += 4;
}
// LDI (HL), A
void CPU::mmu_ldi(uint16_t address, uint8_t &data)
{
	mmu_load8(address, data);
	address += 1;
	
}

// LDD (HL), A
void CPU::mmu_ldd(uint16_t  address, uint8_t & data)
{
	mmu_load8(address, data);
	address -= 1;
}
	
// LD (BC), A
void CPU::mmu_load8(uint16_t address, uint8_t data )
{
	Memory.writeMemory(address, data);
	cycles += 8;
}



// LD B, n
void CPU::load8_imm(uint8_t & reg)
{
	uint8_t data = Memory.readMemory(this->pc);
	pc++;
	reg = 0x41;
}

// do nothing
void CPU::opcode_nop() {
	cycles += 4;
}

// CPL
void CPU::opcode_cpl(uint8_t & value)
{
	value = ~value;
	bitset(FLAG_N);
	bitset(FLAG_H);
	cycles += 4;
}

// SCF
void CPU::opcode_scf()
{
	bitset(FLAG_C);
	bitreset(FLAG_N);
	bitreset(FLAG_H);
	cycles += 4;
}
// CP B
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
	cycles += 4;
}

//CP (HL)
void CPU::opcode_CPmmu(uint8_t reg1, uint16_t pointer)
{
	uint8_t val2 = Memory.readMemory(pointer);
	opcode_CP(reg1, val2);
	// opcode_CP already adds 4 cycles and this one requires 8 thus we add 4 more
	cycles += 4;
}

void CPU::enable_interrupts()
{
	interruptsEnabled = true;
	cycles += 4;
}

void CPU::opcode_di()
{
	IME = false;
	cycles += 4;
}

void CPU::opcode_ei()
{
	IME = true;
	cycles += 4;
}

void CPU::opcode_resetIME()
{
	IME = true;
}

// POP BC
void CPU::call_false(int flag)
{
	if (getBit(flag) != 0) {
		call_nn();
	}
	
}

// CALL Z, nn
void CPU::call_true(int flag)
{
	if (getBit(flag) == 0) {
		call_nn();
	}
}

// CALL nn
void CPU::call_nn()
{
	uint16_t imd = readTwoBytes();
	this->pc += 2;
	writeToStack(pc);
	this->pc = readTwoBytes();
	cycles += 12;
}

// PUSH BC
void CPU::push_reg16(uint16_t reg)
{
	writeToStack(reg);
	cycles += 16;
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
	cycles += 4;
}

// load A from address pointed by (BC)
// LD A, (BC)
void CPU::opcode_load8(uint16_t address, uint8_t &destination) {
	destination = Memory.readMemory(address);
	cycles += 8;
}
// LDI A, (HL)
void CPU::opcode_ldi8(uint16_t  address, uint8_t & destination)
{
	opcode_load8(address, destination);
	address++;
}

// LDD A, (HL)
void CPU::opcode_ldd8(uint16_t  address, uint8_t & destination)
{
	opcode_load8(address, destination);
	address--;
}


// LD (BC), A
void CPU::opcode_mmucopy8(uint16_t mmulocation, uint8_t data)
{
	Memory.writeMemory(mmulocation, data);
	cycles += 8;
}
void CPU::ld_reg8(uint8_t loc, uint8_t data)
{
	mmu_load8((0xFF00 + loc), data);
}
// LDH (n), A
void CPU::ldh_imm(uint8_t reg)
{
	uint8_t n = Memory.readMemory(this->pc);
	pc++;
	mmu_load8((0xFF00 + n), reg);
	// add extra 4 cycles for total of 12
	cycles += 4;
}

// LDH A, (n)
void CPU::ldh_a(uint8_t & reg)
{
	uint8_t n = Memory.readMemory(this->pc);
	pc++;
	reg = Memory.readMemory(0xFF00 + n);
	cycles += 12;
}

// LD (nn), SP
void CPU::load_SP()
{
	writeRegToMemory(sp);
	cycles += 20;
}
// LD (nn), A
void CPU::load_nnReg8(uint8_t reg)
{
	uint16_t address = readTwoBytes();
	Memory.writeMemory(address, reg);
	cycles += 12;
}
// LDHL SP, d
void CPU::opcode_ldhl()
{
	int8_t signedInt = Memory.readMemory(this->pc);
	pc++;
	if ((this->sp.reg & 0xFF)+ signedInt > 0xFF) {
		bitset(FLAG_C);
	}

	if ((this->sp.reg & 0xF) + (signedInt & 0xF) > 0xF) {
		bitset(FLAG_H);
	}
	registerHL.reg = signedInt + sp.reg;
	cycles += 12;
}

void CPU::load_SP_HL()
{
	this->sp.reg = registerHL.reg;
	cycles += 8;
}
// LD B, n
void CPU::copy_reg8(uint8_t &destination, uint8_t source)
{
	destination = source;
}

void CPU::mmu_imm(uint16_t address)
{
	uint8_t data = Memory.readMemory(this->pc);
	pc++;
	mmu_load8(address, data);
}

// load immediate data from memory into 16-bit register
// LD BC, nn
void CPU::reg16_load(uint16_t &reg)
{
	uint16_t byteLo = Memory.readMemory(this->pc);
	uint16_t byteHi = Memory.readMemory(this->pc+1);
	uint16_t twoBytes = (byteHi << 8) | byteLo;
	reg = twoBytes;
	this->pc += 2;
	cycles += 12;
}

// LD C,n
void CPU::reg8_load(uint8_t &address)
{
	address = Memory.readMemory(this->pc);
	this->pc += 1;
	cycles += 8;
}



// ADD HL, BC
void CPU::add_16(uint16_t & destination, uint16_t source)
{
	bitreset(FLAG_Z);
	if ((destination & 0x0FFF) + (source & 0x0FFF) > 0x0FFF) {
		bitset(FLAG_H);
	}
	if (destination + source > 0xFFFF) {
		bitset(FLAG_C);
	}

	destination += source;
	cycles += 8;
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
	cycles += 4;
}

// ADD A, (HL)
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
	cycles += 8;
}
// ADC A, B
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

	destination += source + carry;
	cycles += 4;
}

// ADC A, (HL)
void CPU::adc_imm(uint16_t pointer, uint8_t & destination)
{
	uint8_t source = Memory.readMemory(pointer);
	this->adc_reg8(source, destination);
	cycles += 8;
}

// ADC A, n
void CPU::adc_n(uint8_t & destination)
{
	uint8_t data = Memory.readMemory(this->pc);
	this->pc++;
	adc_reg8(data, destination);
	// add extra 4 cyckes
	cycles += 4;
}
// ADD A, n
void CPU::add_n(uint8_t & destination)
{
	uint8_t data = Memory.readMemory(this->pc);
	this->pc++;
	add_8(data, destination);
	// add extra 4 cycles
	cycles += 4;
}
// ADD SP, d
void CPU::add_signedToSP()
{
	int8_t signedInt = Memory.readMemory(this->pc);
	pc++;
	bitreset(FLAG_Z);
	bitreset(FLAG_N);
	if ((this->sp.reg & 0x0FFF) + (signedInt & 0x0FFF) > 0x0FFF) {
		bitset(FLAG_H);
	}

	if (this->sp.reg + signedInt > 0xFFFF) {
		bitset(FLAG_C);
	}
	cycles += 16;
}

// SUB A, D
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
	cycles += 4;
}

// SUB A, (HL)
void CPU::sub_mmu(uint16_t value, uint8_t & destination)
{
	uint8_t val = Memory.readMemory(value);
	sub_reg8(val, destination);
	// add extra 4 cycles for total of 8 required due to memory access
	cycles += 4;
}

// SBC A, (HL)
void CPU::sbc_imm(uint16_t pointer, uint8_t & destination)
{
	uint8_t value = Memory.readMemory(pointer) + getBit(FLAG_C);
	sub_reg8(value, destination);
	// add extra 4 cycles for total of 8 required due to memory access
	cycles += 4;
}

// SBC A, n
void CPU::sbc_n(uint8_t & destination)
{
	uint8_t data = Memory.readMemory(this->pc);
	this->pc++;
	sbc_reg8(data, destination);
	// add extra 4 cycles for total of 8 required due to memory access
	cycles += 4;
}

// SBC A, B
void CPU::sbc_reg8(uint8_t data, uint8_t & destination)
{
	uint8_t value = data + getBit(FLAG_C);
	sub_reg8(value, destination);
}

// SUB A, n
void CPU::sub_n(uint8_t & destination)
{
	uint8_t data = Memory.readMemory(this->pc);
	this->pc++;
	sub_reg8(data, destination);
	// add extra 4 cycles for total of 8 required due to memory access
	cycles += 4;
}

// increment 16-bit register by one
// INC BC
void CPU::incr_reg(uint16_t & address)
{
	address += 1;
	cycles += 8;
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
	cycles += 4;
}
// INC (HL)
void CPU::incp_reg(int16_t address)
{
	uint8_t value = Memory.readMemory(address);
	if (value + 1 > 0xFF) {
		bitset(FLAG_C);
		return;
	}
	value++;
	Memory.writeMemory(address, value);
	cycles += 12;
}

// AND B
void CPU::and_reg8(uint8_t value, uint8_t & destination)
{
	destination &= value;
	if (destination == 0) {
		bitset(FLAG_Z);
	}

	bitreset(FLAG_N);
	bitset(FLAG_H);
	bitreset(FLAG_C);
	cycles += 4;

}

// AND, n
void CPU::and_n(uint8_t & destination)
{
	uint8_t data = Memory.readMemory(this->pc);
	pc++;
	and_reg8(data, destination);
	// add extra 4 cycles for total of 8 required due to memory access
	cycles += 4;
}

// AND (HL)
void CPU::and_mmu(uint16_t value, uint8_t & destination)
{
	uint8_t and = Memory.readMemory(value);
	this->and_reg8(and, destination);
	// add extra 4 cycles for total of 8 required due to memory access
	cycles += 4;
}

// XOR B
void CPU::xor_reg8(uint8_t value, uint8_t & destination)
{
	destination ^= value;

	if (destination == 0) {
		bitset(FLAG_Z);
	}
	bitreset(FLAG_N);
	bitreset(FLAG_H);
	bitreset(FLAG_C);
	cycles += 4;

}

// XOR (HL)
void CPU::xor_mmu(uint16_t value, uint8_t & destination)
{
	uint8_t val2 = Memory.readMemory(value);
	xor_reg8(val2, destination);
	// add extra 4 cycles for total of 8 required due to memory access
	cycles += 4;
}

// XOR, n
void CPU::xor_n(uint8_t & destination)
{
	uint8_t data = Memory.readMemory(this->pc);
	pc++;
	xor_reg8(data, destination);
	// add extra 4 cycles for total of 8 required due to memory access
	cycles += 4;
}

// OR B
void CPU::or_reg8(uint8_t value, uint8_t & destination)
{
	destination |= value;

	if (destination == 0) {
		bitset(FLAG_Z);
	}
	bitreset(FLAG_N);
	bitreset(FLAG_H);
	bitreset(FLAG_C);
	cycles += 4;
}

// OR (HL)
void CPU::or_mmu(uint16_t pointer, uint8_t & destination)
{
	uint8_t value = Memory.readMemory(pointer);
	or_reg8(value, destination);
	// add extra 4 cycles for total of 8 required due to memory access
	cycles += 4;
}

// OR, n
void CPU::or_n(uint8_t & destination)
{
	uint8_t data = Memory.readMemory(this->pc);
	pc++;
	and_reg8(data, destination);
	// add extra 4 cycles for total of 8 required due to memory access
	cycles += 4;

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
	cycles += 4;
}
// DEC BC
void CPU::decr_reg(uint16_t & address)
{
	int original = address >> 3;
	address -= 1;
	cycles += 8;

}
// DEC (HL)
void CPU::decp_reg(uint16_t value)
{
	uint8_t newValue = Memory.readMemory(value);
	if (newValue == 0) {
		bitset(FLAG_C);
		return;
	}
	newValue--;
	Memory.writeMemory(value, newValue);
	cycles += 12;
}
void CPU::restart(uint8_t n)
{
	writeToStack(this->pc);
	this->pc = n;
	cycles += 32;
}
/* Rotate left with carry
   RLC A
*/
void CPU::rlc_reg8(uint8_t &address)
{
	uint8_t original = address;
	address <<= 1;
	uint8_t lsb = original >> 7;
	address |= lsb;
	cout << std::bitset<8>(address) << endl;
	bitset(FLAG_C, lsb);
	if (address == 0) {
		bitset(FLAG_Z);
	}


	cycles += 8;
}

/* Rotate right with carry
   RRC A
*/
void CPU::rrc_reg8(uint8_t & address)
{
	uint8_t original = address;
	address = original >> 1;
	uint8_t lsb = original & 0b00000001;
	if (lsb == 1) {
		address |= (0b00000001 << 7);
	}
	else {
		address	&= ~(1U << 7);
	}

	bitset(FLAG_C, lsb);
	if (address == 0) {
		bitset(FLAG_Z);
	}
	bitreset(FLAG_N);
	bitreset(FLAG_H);

	cycles += 8;
}

// Rotate A left, documentation says this is the same exact opcode as RLC, A: unsure if this should get implemented?
// RL, A
void CPU::rl_reg8(uint8_t & address)
{
	address <<= 1;
	uint8_t bitSeven = address >> 7;
	uint8_t carry = getBit(FLAG_C);
	address |= carry;
	if (bitSeven == 1) {
		bitset(FLAG_C);
	}
	else {
		bitreset(FLAG_C);
	}
	if (address == 0) {
		bitset(FLAG_Z);
	}

	bitreset(FLAG_N);
	bitreset(FLAG_H);
	cycles += 4;
}
// RR A
void CPU::rr_reg8(uint8_t & address)
{
	uint8_t original = address;
	address >>= 1;
	uint8_t carry = getBit(FLAG_C);
	uint8_t bitZero = address |= 0x1;
	bitset(FLAG_C, bitZero);
	address |= (carry << 7);

	if (address == 0) {
		bitset(FLAG_Z);
	}
	bitreset(FLAG_N);
	bitreset(FLAG_H);
}

// RET
void CPU::opcode_ret()
{
	this->pc = readFromStack();
	cycles += 8;
}

// RET Z
void CPU::opcode_retFalse(int flag)
{
	if (getBit(flag) == 0) {
		opcode_ret();
	}
}

// RET NZ
void CPU::opcode_retTrue(int flag)
{
	if (getBit(flag)) {
		opcode_ret();
	}
}







