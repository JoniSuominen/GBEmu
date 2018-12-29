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

int CPU::getBit(int bit, uint8_t reg)
{
	return reg >> bit;
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

void CPU::write16ToMemory(Register reg)
{
	uint16_t nn = readTwoBytes();
	this->pc += 2;
	Memory.writeMemory(nn, reg.lo);
	nn++;
	Memory.writeMemory(nn, reg.hi);
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

void CPU::init()
{
	pc = 0x100;
	registerAF.reg = 0x01B0;
	registerBC.reg = 0x0013;
	registerDE.reg = 0x00D8;
	registerHL.reg = 0x014D;
	sp = 0xFFFE;
	Memory.init();

}


void CPU::bitreset(int flag) {
	this->registerAF.reg &= ~(1U << flag);
}