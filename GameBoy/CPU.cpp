#include "CPU.h"

using namespace std;


CPU::CPU() {

	clock = {
		{
			'm', 0
		},
		{
			't', 0
		}
	};
}
void CPU::fetchOpcode() {
	uint8_t opcode = Memory.readMemory(this->pc);
	this->pc++;
	executeOpCode(opcode);
}


void CPU::executeOpCode(uint8_t opcode)
{
	switch (opcode) {
	case 0x00: opcode_nop();
	case 0x01: 

	}

}


