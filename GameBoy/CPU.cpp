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
	case 0x0: opcode_nop();


	}
}

void CPU::bitset(int flag)
{
	this->registerAF.reg |= (1U << flag);
}

void CPU::bitset(int flag, int value)
{
	uint16_t bit = value;
	bitreset(flag);	
	this->registerAF.reg |= (bit << flag);
}

int CPU::getBit(int flag)
{	
	return this->registerAF.reg >> flag;
}

uint16_t CPU::readFromStack()
{
	uint16_t address = (Memory.readMemory(this->sp) << 8) | (Memory.readMemory(this->sp - 1));
	this->sp -= 2;
	return address;
}

void CPU::bitreset(int flag) {
	this->registerAF.reg &= ~(1U << flag);
}