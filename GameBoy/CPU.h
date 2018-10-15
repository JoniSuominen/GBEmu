#include <unordered_map>
#include <iostream>
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


public:
	CPU();
	void fetchOpcode();
	void executeOpCode(uint8_t opcode);

	/* opcodes */

	// NOP
	void opcode_nop();

	// LOAD
	void mmu_load(uint16_t address, uint8_t data);
	void reg16_load(Register * reg);
	void reg8_load(uint8_t * address);

	// INCR
	void incr_reg(uint16_t *address);
	void incr_reg(uint8_t *address);

	// DEC
	void decr_reg(uint8_t *address);




};