#include <unordered_map>
#include <iostream>
#include <bitset>
#define CPU_H
#include "MMU.h"
using namespace std;

class CPU
{
private:
	MMU Memory;
		union Register
		{
			// 2 byte datatype for storing hi and lo bits
			__int16 reg;
			struct
			{
				uint8_t lo;
				uint8_t hi;

			};
		};
	unordered_map<char, int> clock;
	Register registerAF, registerBC, registerDE, registerHL;
	uint16_t pc, sp;
	uint8_t m, t;
	const int FLAG_Z = 7;
	const int FLAG_N = 6;
	const int FLAG_H = 5;
	const int FLAG_C = 4;


public:
	CPU();
	void fetchOpcode();
	void executeOpCode(uint8_t opcode);
	void bitset(int flag);
	void bitreset(int flag);
	void bitset(int flag, int value);
	int getBit(int flag);

	/* opcodes */

	// MISC
	void opcode_nop();
	void opcode_stop();

	// JUMPS
	void jump_n(uint8_t n);
	void jump_zero(uint8_t n);

	// LOAD
	void mmu_load8(uint16_t address, uint8_t data);
	void mmu_load16(int16_t address, uint8_t data);
	void reg16_load(Register *reg);
	void reg8_load(uint8_t & address);
	void opcode_load8(uint16_t address, uint8_t &destination);

	// ADD
	void add_2(uint16_t &destination, uint16_t &source);

	// INCR
	void incr_reg(uint16_t &address);
	void incr_reg(uint8_t &address);

	// DECR
	void decr_reg(uint8_t &address);
	void decr_reg(uint16_t &address);

	// ROTATE
	void rlc_reg8(uint8_t &address);
	void rrc_reg8(uint8_t &address);
	void rl_reg8(int8_t &address);
	




};