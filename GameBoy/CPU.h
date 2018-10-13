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

<<<<<<< HEAD
	Register registers[4] = { RegisterBC, RegisterDE, RegisterHL, RegisterAF };
=======
	Register registerAF, registerBC, registerDE, registerHL;
>>>>>>> origin/master
	uint16_t pc, sp;
	uint8_t m, t;


public:
	CPU();
	void fetchOpcode();
	void executeOpCode(uint8_t opcode);
	void ld_reg_reg();
	void opcode_nop();
	void load_16bit(Register reg, uint16_t nn);


};