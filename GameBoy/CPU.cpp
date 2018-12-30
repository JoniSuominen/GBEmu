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

void CPU::start()
{
	init();
	Memory.loadRom("C:\\Users\\jonis\\GBEmu\\test.gb");
	cycle();
}

void CPU::cycle() {
	int cyclesBefore = 0;
	while (this->cycles < MAX_CYCLES) {
		uint8_t opcode = Memory.readMemory(this->pc);
		std::bitset<8> x(opcode);
		cout << x << endl;
		pc++;
		executeOpCode(opcode);
		int opcodeCycles = cycles - cyclesBefore;
	}
	cycles = 0;
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
	uint16_t address = (Memory.readMemory(this->sp.reg + 1) << 8) | (Memory.readMemory(this->sp.reg));
	this->sp.reg += 2;
	return address;
}

void CPU::writeToStack(uint16_t data)
{
	this->sp.reg--;
	uint8_t hi = data >> 8;
	Memory.writeMemory(this->sp.reg, hi);
	this->sp.reg--;
	uint8_t lo = data & 0xFF;
	Memory.writeMemory(this->sp.reg, lo);
	
}

void CPU::writeRegToMemory(Register reg)
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
	sp.reg = 0xFFFE;
	Memory.init();

}

uint8_t * CPU::getRegister(uint8_t bits)
{
	switch (bits)
	{
	case 0:  cout << "hi" << endl;  return &registerBC.hi;
		case 1: return &registerBC.lo;
		case 2: return &registerDE.hi;
		case 3: return &registerDE.lo;
		case 4: return &registerHL.hi;
		case 5: return &registerHL.lo;
		case 7: return &registerAF.hi;
		default: cout << "Not a valid register" << endl; break;
	}
	uint8_t ret = 5000;
	return &ret;
	// TODO: insert return statement here
}


void CPU::bitreset(int flag) {
	this->registerAF.reg &= ~(1U << flag);
}