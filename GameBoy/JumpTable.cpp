#include "CPU.h"
#include <bitset>

void CPU::executeOpCode(uint8_t opcode)
{
	switch (opcode) {
		// messy switch case block, i do not want a 500 row switch case though
		/*
		case 0x0: opcode_nop(); break; case 0x1: reg16_load(registerBC); break;
		case 0x2: mmu_load8(registerBC.reg, registerAF.hi); break; case 0x3: incr_reg(registerBC.reg); break;
		case 0x4: incr_reg(registerBC.hi); break;  case 0x5: decr_reg(registerBC.hi); break;
		case 0x6: load8_imm(registerBC.hi); break; case		: rlc_reg8(registerAF.hi); break;
		case 0x8: load_SP(); break; case 0x9: add_16(registerHL.reg, registerBC.reg); break;
		case 0xA: opcode_load8(registerBC.reg, registerAF.hi); break; case 0xB: decr_reg(registerBC.reg); break;
		case 0xC: incr_reg(registerBC.lo); break; case 0xD: decr_reg(registerBC.lo); break;
		case 0xE: load8_imm(registerBC.lo); break; case 0xF: rrc_reg8(registerAF.hi);

						 case 0x11: reg16_load(registerDE); break;
		case 0x12: mmu_load8(registerDE.reg, registerAF.hi); break; case 0x13: incr_reg(registerDE.reg); break;
		case 0x14: incr_reg(registerDE.hi); break; case 0x15: decr_reg(registerDE.hi); break;
		case 0x16: load8_imm(registerDE.hi); break; case 0x17: rl_reg8(registerAF.hi); break;
		case 0x18: jump_n(); break; case 0x19: add_16(registerHL.reg, registerBC.reg); break;
		case 0x1A: opcode_load8(registerDE.reg, registerAF.hi); break;  case 0x1B: decr_reg(registerDE.reg); break;
		case 0x1C: incr_reg(registerDE.lo); break; case 0x1D: decr_reg(registerDE.lo); break;
		case 0x1E: load8_imm(registerDE.lo); break; case 0x1F: rra_reg8(registerAF.hi); break;

		case 0x20: jump_notzero(); break; case 0x21: reg16_load(registerHL); break;
		case 0x22: mmu_ldi(registerHL.reg, registerAF.hi); break; case 0x23: incr_reg(registerHL.reg); break;
		case 0x24: incr_reg(registerHL.hi); break; case 0x25: decr_reg(registerHL.hi); break;
		case 0x26: load8_imm(registerHL.hi); break; case 0x27: opcode_bcd(registerAF.hi); break;
		case 0x28: jump_zero(); break; case 0x29: add_16(registerHL.reg, registerHL.reg);
		case 0x2A: opcode_ldi8(registerHL.reg, registerAF.hi); break; case 0x2B: decr_reg(registerHL.reg); break;
		*/

		/* By algorithmically decoding which bits hold the operation, target and source registers
		   we can group instructions together, creating more readable and reusable code
		*/
	
	// MISC

	case 0x0: opcode_nop(); break;
	case 0x20: jump_notzero(); break;
	case 0x30: jump_notcarry(); break;

	// LOADS

	// LD (nn), SP
	case 0x08: load_SP(); break;

	// LD REG16, nn
	case 0x01: case 0x11: case 0x21: case 0x31:
	{
		uint8_t target = opcode >> 2;
		uint16_t *pointerToTarget = get16BitRegister(target);
		reg16_load(*pointerToTarget);
		break;
	}

	// LD (REG16), REG8
	case 0x02: mmu_load8(registerBC.reg, registerAF.hi); break;
	case 0x12: mmu_load8(registerDE.reg, registerAF.hi); break;
	case 0x22: mmu_ldi(registerHL.reg, registerAF.hi); break;
	case 0x32: mmu_ldd(registerHL.reg, registerAF.hi); break;
	case 0x36: mmu_imm(registerHL.reg); break;

	case 0x70: case 0x71: case 0x72: case 0x73: case 0x75: case 0x77:
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
	case 0x2A: opcode_ldi8(registerHL.reg, registerAF.hi);
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
		uint8_t pointerToSource = *get8BitRegister(target);
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
	case 0xE2: mmu_load8(0xFF00 + registerBC.lo, registerAF.hi); break;

	case 0xEA: load_nnReg8(registerAF.hi); break;
	case 0xF8: opcode_ldhl();
	case 0xF9: sp.reg = registerHL.reg; break;
	case 0xFA: registerAF.hi = Memory.readMemory(this->pc); pc++; break;



	// INCR AND DEC
	// INC REG16
	case 0x03: case 0x13: case 0x23: case 0x33:
	{
		uint8_t target = opcode >> 2;
		uint16_t *pointerToTarget = get16BitRegister(target); 
		incr_reg(*pointerToTarget);
		break;
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



	// ADD AND SUBSTRACT

	// ADD 16
	case 0x9: add_16(registerHL.reg, registerBC.reg); break;
	case 0x19: add_16(registerHL.reg, registerDE.reg); break;
	case 0x29: add_16(registerHL.reg, registerDE.reg); break;
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
	case 0xD6: sub_n(registerAF.hi);

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
	
	// XOR REG8
	case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF:
	{
		uint8_t target = opcode & 0b00000111;
		uint8_t *pointerToRegister = get8BitRegister(target);
		xor_reg8(*pointerToRegister, registerAF.hi);
		break;
	}

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
		opcode_CP(*pointerToRegister, registerAF.hi);
		break;
	}



	// ROTATES
	
	// RLC REG8
	case 0x7: rlc_reg8(registerAF.hi); break;

	// RRC REG8
	case 0xF: rrc_reg8(registerAF.hi); break;

	default: {

		cout << "UNIMPLEMENTED INSTRUCTION: " << hex << static_cast<int>(opcode) << endl;
	}

	}

}