#include "CPU.h"
#include <bitset>

void CPU::executeOpCode(uint8_t opcode)
{
	switch (opcode) {


		/* By algorithmically decoding which bits hold the operation, target and source registers
		   we can group instructions together, creating more readable and reusable code
		*/
	
	// MISC

	case 0x0: opcode_nop(); break;
	case 0xa6: and_mmu(registerHL.reg, registerAF.hi); break;
	case 0x20: jump_notzero(); break;
	case 0x27: opcode_bcd(registerAF.hi); break;
	case 0x18: jump_n(); break;
	case 0x38: jump_carry(); break;
	case 0xFE: opcode_CP(registerAF.hi, Memory.readMemory(this->pc)); pc++; break;
	case 0x28: jump_zero(); break;
	case 0x30: jump_notcarry(); break;
	case 0xF7: restart(0x30); break;
	case 0xF3: opcode_di(); break;
	case 0xFB: enable_interrupts(); break;
	case 0xC9: opcode_ret(); break;
	case 0x3F: opcode_ccf(); break;
	case 0xFF: restart(0x38); break;
	case 0xF1: opcode_pop(registerAF.reg); registerAF.lo &= 0xF0; break;
	case 0xCB: opcode_cb(); break;
	case 0xCD: opcode_callNN(); break;
	case 0xC4: call_false(FLAG_Z); break;
	case 0xCC: call_true(FLAG_Z); break;
	case 0xD1: opcode_pop(registerDE.reg); break;
	case 0xE9: jump_mmu(registerHL.reg); break;
	case 0xD0: opcode_retFalse(FLAG_C); break;
	case 0xC8: opcode_retTrue(FLAG_Z); break;
	case 0xB6: or_reg8(Memory.readMemory(registerHL.reg), registerAF.hi); break;
	case 0xEE:	xor_n(registerAF.hi); break;
	case 0xD8: opcode_retTrue(FLAG_C); break;
	case 0x10: opcode_stop(); break;
	case 0x2F: opcode_cpl(registerAF.hi); break;
	case 0xEF: restart(0x28); break;
	case 0xDF: restart(0x18); break;
	case 0xD9: opcode_ret(); opcode_ei(); break;
	case 0xD2: jump_absFalse(FLAG_C); break;
	case 0xD7: restart(0x10); break;
	case 0x76: opcode_halt(); break;
	case 0xC0:	opcode_retFalse(FLAG_Z); break;

	case 0xC7: restart(0x0); break;
	case 0xC2: jump_absFalse(FLAG_Z); break;
	case 0xDA: jump_absTrue(FLAG_C); break;
	case 0xD4: call_false(FLAG_C); break;
	case 0xDC: call_true(FLAG_C); break;
	case 0x37: opcode_scf(); break;
	case 0xCF: restart(0x8); break;
	case 0xE7: restart(020); break;

	case 0xBE: opcode_CP(registerAF.hi, Memory.readMemory(registerHL.reg)); cycles += 4; break;
	case 0x1f: rr_reg8(registerAF.hi); break;

	// LOADS

	// LD (nn), SP
	case 0x08: load_SP(); break;

	// LD REG16, nn
	case 0x01: {
		reg16_load(registerBC.reg);
		break;
	}
	case 0x11: {
		reg16_load(registerDE.reg);
		break;
	}
	case 0x21: {
		reg16_load(registerHL.reg);
		break;
	}
	case 0x31: {
		reg16_load(sp.reg);
		break;
	}

	// LD (REG16), REG8
	case 0x02: mmu_load8(registerBC.reg, registerAF.hi); break;
	case 0x12: mmu_load8(registerDE.reg, registerAF.hi); break;
	case 0x22: mmu_ldi(registerHL.reg, registerAF.hi); break;
	case 0x32: mmu_ldd(registerHL.reg, registerAF.hi); break;
	case 0x36: mmu_imm(registerHL.reg); break;

	case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77:
	{
		uint16_t source = (opcode & 0b00000111);
		uint8_t *pointerToSource = get8BitRegister(source);
		mmu_load8(registerHL.reg, *pointerToSource);
		break;
	}

	// LD reg8, n
	case 0x06: case 0x16: case 0x26: case 0x0E: case 0x1E: case 0x2E: case 0x3E:
	{
		
		uint8_t target = opcode >> 3;
		uint8_t *pointer = get8BitRegister(target);
		load8_imm(*pointer);
		break;
	}

	// LD REG8, (REG16)
	case 0x0A: opcode_load8(registerBC.reg, registerAF.hi); break;
	case 0x1A: opcode_load8(registerDE.reg, registerAF.hi); break;
	case 0x2A: opcode_ldi8(registerHL.reg, registerAF.hi);  break;
	case 0x3A: opcode_ldd8(registerHL.reg, registerAF.hi); break;

	case 0x4E: case 0x5E: case 0x6E: case 0x7E:
	{
		uint8_t target = (opcode & 0b00111000) >> 3;
		uint8_t *pointerToRegister = get8BitRegister(target);
		opcode_load8(registerHL.reg, *pointerToRegister);
		break;
	}

	
	// LD reg8, reg8
	case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: case 0x48: case 0x49:
	case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4F:	case 0x50: case 0x51: case 0x52: case 0x53:
	case 0x54: case 0x55: case 0x57: case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D:
	case 0x5F: case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: case 0x68:
	case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6F: case 0x78: case 0x79: case 0x7A:
	case 0x7B: case 0x7C: case 0x7D: case 0x7F:
	{
		uint8_t target = opcode >> 3;
		target &= 0b00000111;
		uint8_t source = (opcode & 0b00000111);
		uint8_t *pointerToTarget = get8BitRegister(target);
		uint8_t pointerToSource = *get8BitRegister(source);
		copy_reg8(*pointerToTarget, pointerToSource);
		break;
	}

	// LD REG8, (HL);
	case 0x46: opcode_load8(registerHL.reg, registerBC.hi); break;
	case 0x56: opcode_load8(registerHL.reg, registerDE.hi); break;
	case 0x66: opcode_load8(registerHL.reg, registerHL.hi); break;
	// LDH
	case 0xE0: ldh_imm(registerAF.hi); break;
	case 0xF0: ldh_a(registerAF.hi); break;
	case 0xE2: mmu_load8(0xFF00 + registerBC.lo, registerAF.hi);   break;

	case 0xEA: load_nnReg8(registerAF.hi); break;
	case 0xF8: opcode_ldhl(); break;
	case 0xF9: sp.reg = registerHL.reg; cycles += 8; break;
	case 0xFA: registerAF.hi = Memory.readMemory(readTwoBytes()); cycles += 16; break;
	




	case 0x03: {
		incr_reg(registerBC.reg);
		break;
	}
	case 0x13: {
		incr_reg(registerDE.reg);
		break;
	}
	case 0x23: {
		incr_reg(registerHL.reg);
		break;
	}
	case 0x33: {
		incr_reg(sp.reg); break;
	}

	case 0x34: incp_reg(registerHL.reg); break;
	// INC reg8
	case 0x4: case 0x14: case 0x24: case 0xC: case 0x1C: case 0x2C: case 0x3C:
	{
		uint8_t target = opcode >> 3;
		uint8_t *pointer = get8BitRegister(target);
		incr_reg(*pointer);
		break;
	}
	// DEC reg8
	case 0x5: case 0x15: case 0x25: case 0xD: case 0x1D: case 0x2D: case 0x3D:
	{
		uint8_t target = opcode >> 3;
		uint8_t *pointerToTarget = get8BitRegister(target);
		decr_reg(*pointerToTarget);
		break;
	}

	// DEC reg16
	case 0xB: decr_reg(registerBC.reg); break;
	case 0x1b: decr_reg(registerDE.reg); break;
	case 0x2b: decr_reg(registerHL.reg); break;
	case 0x3b: decr_reg(sp.reg); break;
	/*
	case 0xB: case 0x1B: case 0x2B: case 0x3B:
	{
		uint16_t target = opcode >> 2;
		uint16_t *pointerToTarget = get16BitRegister(target);
		cout << *pointerToTarget << endl;
		decr_reg(*pointerToTarget);
		break;
	}
	*/

	case 0x35: decp_reg(registerHL.reg); break;

	case 0xC3: jump_abs(); break;
	case 0xCA: jump_absTrue(FLAG_Z); break;

	// ADD AND SUBSTRACT

	// ADD 16
	case 0x9: add_16(registerHL.reg, registerBC.reg); break;
	case 0x19: add_16(registerHL.reg, registerDE.reg); break;
	case 0x29: add_16(registerHL.reg, registerHL.reg); break;
	case 0x39: add_16(registerHL.reg, sp.reg); break;

	// ADD 8
	case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87:
	{
		uint8_t target = opcode & 7;
		uint8_t *pointerToRegister = get8BitRegister(target);
		add_8(*pointerToRegister, registerAF.hi);
		break;
	}

	// ADD IMM

	case 0xC6: add_n(registerAF.hi); break;
	case 0xE8: add_signedToSP(); break;
	case 0x86: add_mmu(registerHL.reg, registerAF.hi); break;

	// ADC REG8
	case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8F:
	{
		uint8_t target = opcode & 7;
		uint8_t *pointerToRegister = get8BitRegister(target);
		adc_reg8(*pointerToRegister, registerAF.hi);
		break;
	}	

	// ADC IMM
	case 0x8E: adc_imm(registerHL.reg, registerAF.hi); break;
	case 0xCE: adc_n(registerAF.hi); break;

	// SUB REG8
	case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97:
	{
		uint8_t target = opcode & 7;
		uint8_t *pointerToRegister = get8BitRegister(target);
		sub_reg8(*pointerToRegister, registerAF.hi);
		break;
	}

	// SUB IMM
	case 0x96: sub_mmu(registerHL.reg, registerAF.hi); break;
	case 0xD6: sub_n(registerAF.hi); break;

	// SBC IMM

	case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9F:
	{
		uint8_t target = opcode & 7;
		uint8_t *pointerToRegister = get8BitRegister(target);
		sbc_reg8(*pointerToRegister, registerAF.hi);
		break;
	}

	case 0x9E: sbc_imm(registerHL.reg, registerAF.hi); break;
	case 0xDE: sbc_n(registerAF.hi); break;

	// AND REG8
	case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7:
	{
		uint8_t target = opcode & 0b00000111;
		uint8_t *pointerToRegister = get8BitRegister(target);
		and_reg8(*pointerToRegister, registerAF.hi);
		break;
	}
	case 0xE6: {
		uint8_t n = Memory.readMemory(this->pc);
		pc++;
		and_reg8(n, registerAF.hi);
		break;
	}
	
	// XOR REG8
	case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF:
	{
		uint8_t target = opcode & 0b00000111;
		uint8_t *pointerToRegister = get8BitRegister(target);
		xor_reg8(*pointerToRegister, registerAF.hi);
		break;
	}
	case 0xAE: xor_reg8(Memory.readMemory(registerHL.reg), registerAF.hi); break;
	case 0xf6: or_n(registerAF.hi); break;


	// OR REG8
	case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7:
	{
		uint8_t target = opcode & 0b00000111;
		uint8_t *pointerToRegister = get8BitRegister(target);
		or_reg8(*pointerToRegister, registerAF.hi);
		break;
	}
	
	// CP REG8
	case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF:
	{
		uint8_t target = opcode & 0b00000111;
		uint8_t *pointerToRegister = get8BitRegister(target);
		opcode_CP(registerAF.hi, *pointerToRegister);
		break;
	}

	// PUSH
	case 0xC5: push_reg16(registerBC.reg); break;
	case 0xF5: push_reg16(registerAF.reg); break;
	case 0xE5: push_reg16(registerHL.reg); break;
	case 0xD5: push_reg16(registerDE.reg); break;
	// POP
	case 0xC1: opcode_pop(registerBC.reg); break;
	case 0xE1: opcode_pop(registerHL.reg); break;

	// ROTATES
	
	// RLC REG8
	case 0x7: rlc_reg8(registerAF.hi); break;
	case 0x17: rl_reg8(registerAF.hi); break;

	// RRC REG8
	case 0xF: rrc_reg8(registerAF.hi); break;

	default: {
		cout << "\nUNIMPLEMENTED INSTRUCTION: " << hex << static_cast<int>(opcode) << endl;
		cout << "PC: " << pc - 1 << endl;
	}

	}

}
void CPU::extended_opcodes(uint8_t opcode) {
	//cout << "EXTENDED OP: " << hex << static_cast<int>(opcode) << endl;
	switch (opcode) {

	// EXT RLC
	case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x07: {
		uint8_t reg = (opcode & 0x7);
		uint8_t *pointerToReg = get8BitRegister(reg);
		rlc_reg8(*pointerToReg);
		if (*pointerToReg == 0) {
			bitset(FLAG_Z);
		}
		break;
	}

	// EXT RRC
	case 0x08: case 0x09: case 0xA: case 0xB: case 0xC: case 0xD:  case 0xF: {
		uint8_t reg = (opcode & 0x7);
		uint8_t *pointerToReg = get8BitRegister(reg);
		rrc_reg8(*pointerToReg);
		if (*pointerToReg == 0) {
			bitset(FLAG_Z);
		}
		break;
	}
	// EXT RL
	case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x17: {
		uint8_t reg = (opcode & 0x7);
		uint8_t *pointerToReg = get8BitRegister(reg);
		rl_reg8(*pointerToReg);
		if (*pointerToReg == 0) {
			bitset(FLAG_Z);
		}
		break;
	}
	// EXT RR
	case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D:  case 0x1F: {
		uint8_t reg = (opcode & 0x7);
		uint8_t *pointerToReg = get8BitRegister(reg);
		rr_reg8(*pointerToReg);
		if (*pointerToReg == 0) {
			bitset(FLAG_Z);
		}
		break;
	}
	// EXT SLA
	case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x27: {
		uint8_t reg = (opcode & 0x7);
		uint8_t *pointerToReg = get8BitRegister(reg);
		ext_sla(*pointerToReg);
		break;
	}
	//EXT SRA
	case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D:  case 0x2F: {
		uint8_t reg = (opcode & 0x7);
		uint8_t *pointerToReg = get8BitRegister(reg);
		ext_sra(*pointerToReg);
		break;
	}

	//EXT SRL
	case 0x38: case 0x39: case 0x3A: case 0x3B: case 0x3C: case 0x3D:  case 0x3F: {
		uint8_t reg = (opcode & 0x7);
		uint8_t *pointerToReg = get8BitRegister(reg);
		ext_srl(*pointerToReg);
		break;
	}
	// BIT n, B
	case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: case 0x48: case 0x49:
	case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4F:	case 0x50: case 0x51: case 0x52: case 0x53:
	case 0x54: case 0x55: case 0x57: case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D:
	case 0x5F: case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: case 0x68:
	case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6F: case 0x70: case 0x71:
	case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: case 0x78: case 0x79: case 0x7A:
	case 0x7B: case 0x7C: case 0x7D: case 0x7F: {
		uint8_t reg = (opcode & 0b00000111);
		uint8_t *pointerToReg = get8BitRegister(reg);
		//cout << "reg h: " << hex << static_cast<int>(registerHL.reg) << endl;
		uint8_t bit = (opcode >> 3) & 0b111;
		test_bit(bit, *pointerToReg);
		break;
	}

	case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: case 0x88: case 0x89:
	case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8F:	case 0x90: case 0x91: case 0x92: case 0x93:
	case 0x94: case 0x95: case 0x97: case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D:
	case 0x9F: case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7: case 0xA8:
	case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: case 0xB0: case 0xB1:
	case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7: case 0xB8: case 0xB9: case 0xBA:
	case 0xBB: case 0xBC: case 0xBD: case 0xBF: {
		uint8_t reg = (opcode & 0b00000111);
		uint8_t *pointerToReg = get8BitRegister(reg);
		uint8_t bit = (opcode >> 3) & 0b111;
		*pointerToReg = reset_bit(*pointerToReg, bit);
		cycles += 8;
		break;
	}

	case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC7: case 0xC8: case 0xC9:
	case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCF:	case 0xD0: case 0xD1: case 0xD2: case 0xD3:
	case 0xD4: case 0xD5: case 0xD7: case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD:
	case 0xDF: case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE7: case 0xE8:
	case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEF: case 0xF0: case 0xF1:
	case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF7: case 0xF8: case 0xF9: case 0xFA:
	case 0xFB: case 0xFC: case 0xFD: case 0xFF: {
		uint8_t reg = (opcode & 0b00000111);
		uint8_t *pointerToReg = get8BitRegister(reg);
		uint8_t bit = (opcode >> 3) & 0b111;
		*pointerToReg = set_bit(*pointerToReg, bit);
		cycles += 8;
		break;
	}

	case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x37: {
		uint8_t reg = (opcode & 0x7);
		uint8_t *pointerToReg = get8BitRegister(reg);
		ext_swap(*pointerToReg);
		break;
	}

	// RLC (HL)
	case 0x06: {
		uint8_t data = Memory.readMemory(registerHL.reg);
		rlc_reg8(data);
		if (data == 0) {
			bitset(FLAG_Z);
		}
		Memory.writeMemory(registerHL.reg, data);
		// 13 kertaa?
		cycles += 8;
		break;
	}
	 // RRC (HL)
	case 0x0E: {
		uint8_t data = Memory.readMemory(registerHL.reg);
		rrc_reg8(data);
		if (data == 0) {
			bitset(FLAG_Z);
		}
		Memory.writeMemory(registerHL.reg, data);
		cycles += 8;
		break;
	}
	// RL (HL)
	case 0x16: {
		uint8_t data = Memory.readMemory(registerHL.reg);
		rl_reg8(data);
		if (data == 0) {
			bitset(FLAG_Z);
		}
		Memory.writeMemory(registerHL.reg, data);
		cycles += 8;
		break;
	}
	// RR (HL)

	case 0x1E: {
		uint8_t data = Memory.readMemory(registerHL.reg);
		rr_reg8(data);
		if (data == 0) {
			bitset(FLAG_Z);
		}
		Memory.writeMemory(registerHL.reg, data);
		cycles += 8;
		break;
	}

	// SLA (HL)
	case 0x26: {
		ext_SLAHL(registerHL.reg);
		cycles += 8;
		break;
	}

	// SRA (HL)
	case 0x2E: {
		ext_SRAHL(registerHL.reg);
		cycles += 8;
		break;
	}

	case 0x36: {
		uint8_t data = Memory.readMemory(registerHL.reg);
		ext_swap(data);
		Memory.writeMemory(registerHL.reg, data);
		break;
	}

	case 0x3E: {
		uint8_t data = Memory.readMemory(registerHL.reg);
		ext_srl(data);
		Memory.writeMemory(registerHL.reg, data);
		break;
	}

	case 0x46:ext_testBit(Memory.readMemory(registerHL.reg), 0); break;
	case 0x4E:ext_testBit(Memory.readMemory(registerHL.reg), 1); break;
	case 0x56:ext_testBit(Memory.readMemory(registerHL.reg), 2); break;
	case 0x5E:ext_testBit(Memory.readMemory(registerHL.reg), 3); break;
	case 0x66:ext_testBit(Memory.readMemory(registerHL.reg), 4); break;
	case 0x6E:ext_testBit(Memory.readMemory(registerHL.reg), 5); break;
	case 0x76:ext_testBit(Memory.readMemory(registerHL.reg), 6); break;
	case 0x7E:ext_testBit(Memory.readMemory(registerHL.reg), 7); break;

	case 0x86: ext_resetBitHL(0); break;
	case 0x8E: ext_resetBitHL(1); break;
	case 0x96: ext_resetBitHL(2); break;
	case 0x9E: ext_resetBitHL(3); break;
	case 0xA6: ext_resetBitHL(4); break;
	case 0xAE: ext_resetBitHL(5); break;
	case 0xB6: ext_resetBitHL(6); break;
	case 0xBE: ext_resetBitHL(7); break;

	case 0xC6: ext_setBitHL(0); break;
	case 0xCE: ext_setBitHL(1); break;
	case 0xD6: ext_setBitHL(2); break;
	case 0xDE: ext_setBitHL(3); break;
	case 0xE6: ext_setBitHL(4); break;
	case 0xEE: ext_setBitHL(5); break;
	case 0xF6: ext_setBitHL(6); break;
	case 0xFE: ext_setBitHL(7); break;

	default:cout << "\nUNIMPLEMENTED EXTENDDED INSTRUCTION: " << hex << static_cast<int>(opcode) << endl; break;

	}
}
