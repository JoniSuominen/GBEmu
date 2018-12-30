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
		case 0x6: load8_imm(registerBC.hi); break; case 0x7: rlc_reg8(registerAF.hi); break;
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

	// LD reg8, n
	case 0x6: case 0x16: case 0x26: case 0xE: case 0x1E: case 0x2E: case 0x3E:
		std::bitset<8> x(registerDE.hi);
		registerBC.hi = 0xFF;
		cout << x << endl;

		uint8_t target = opcode >> 3;
		std::bitset<8> xy(target);
		cout << xy << endl;
		uint8_t *pointer = getRegister(target);
		load8_imm(*pointer);
		std::bitset<8> z(*pointer);
		cout << z << endl;
		std::bitset<8> y(registerDE.hi);
		cout << y << endl;

				

		




	}
}