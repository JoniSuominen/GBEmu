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

void CPU::cycle() {
	int cyclesBefore = 0;
	while (this->cycles < MAX_CYCLES) {
		uint8_t opcode = Memory.readMemory(this->pc);
		executeOpCode(opcode);
		int opcodeCycles = cycles - cyclesBefore;
	}
	cycles = 0;
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
	uint16_t address = (Memory.readMemory(this->sp + 1) << 8) | (Memory.readMemory(this->sp));
	this->sp += 2;
	return address;
}

void CPU::writeToStack(uint16_t data)
{
	this->sp--;
	uint8_t hi = data >> 8;
	Memory.writeMemory(this->sp, hi);
	this->sp--;
	uint8_t lo = data & 0xFF;
	Memory.writeMemory(this->sp, lo);
	
}

uint16_t CPU::jump16()
{
	uint16_t address = (Memory.readMemory(this->pc) << 8) | (Memory.readMemory(this->pc + 1));
	return address;
}

uint16_t CPU::readTwoBytes()
{
	uint16_t data = (Memory.readMemory(this->pc + 1) << 8) | (Memory.readMemory(this->pc + 1));
	return data;
}

void CPU::readFile(string path)
{
	ifstream inFile;
	inFile.open(path);
	if (!inFile) {
		cout << "Unable to open file" << endl;
	}
	else {
		Memo
	}
}

void CPU::bitreset(int flag) {
	this->registerAF.reg &= ~(1U << flag);
}