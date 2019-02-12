#include "CPU.h"

using namespace std;


void CPU::start()
{
	init();
	Memory.loadRom("C:\\Users\\Joni\\source\\repos\\GameBoy\\test.gb");
	cout << " XD" << endl;

	while (true) {
		cycle();
	}
}

// main emulation loop
void CPU::cycle() {
	int cyclesBefore = 0;
	timerCounter = CLOCKSPEED / 1024;
	while (this->cycles < MAX_CYCLES) {
		uint8_t opcode = int(Memory.readMemory(this->pc));
		pc++;
		executeOpCode(opcode);
		cout << hex << static_cast<int>(opcode) << endl;
		int opcodeCycles = cycles - cyclesBefore;
		updateTimers(opcodeCycles);
		handleInterrupts();

	}
	cycles = 0;
}


// set flag on
void CPU::bitset(int flag)
{
	this->registerAF.reg |= (1U << flag);
}

// set a flag to a value
void CPU::bitset(int flag, int value)
{
	uint16_t bit = value;
	bitreset(flag);	
	this->registerAF.reg |= (bit << flag);
}

// returns single flag
int CPU::getBit(int flag)
{	
	return this->registerAF.reg >> flag;
}

// return a single bits value from register
int CPU::getBit(int bit, uint8_t reg)
{
	return (reg >> bit) & 0b00000001;
}

// read a 16-bit address from the stack
uint16_t CPU::readFromStack()
{
	uint16_t address = (Memory.readMemory(this->sp.reg + 1) << 8) | (Memory.readMemory(this->sp.reg));
	this->sp.reg += 2;
	return address;
}

// write a 16-bit register to the stack
void CPU::writeToStack(uint16_t data)
{
	this->sp.reg--;
	uint8_t hi = data >> 8;
	Memory.writeMemory(this->sp.reg, hi);
	this->sp.reg--;
	uint8_t lo = data & 0xFF;
	Memory.writeMemory(this->sp.reg, lo);
	
}

// write two bytes to memory
void CPU::writeRegToMemory(Register reg)
{
	uint16_t nn = readTwoBytes();
	this->pc += 2;
	Memory.writeMemory(nn, reg.lo);
	nn++;
	Memory.writeMemory(nn, reg.hi);
}

// return the address you must set the program counter to in case of a jump command
uint16_t CPU::jump16()
{
	uint16_t address = (Memory.readMemory(this->pc) << 8) | (Memory.readMemory(this->pc + 1));
	return address;
}

// read two bytes from memory and combine them to single 16-bit integer
uint16_t CPU::readTwoBytes()
{
	uint16_t data = (Memory.readMemory(this->pc) << 8) | (Memory.readMemory(this->pc + 1));
	pc += 2;
	return data;
}

// initializes the cpu (and mmu)
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

// returns an 8-bit register based on the bit order
uint8_t * CPU::get8BitRegister(uint8_t bits)
{
	switch (bits)
	{
	case 0:  cout << "hi" << endl;  return &registerBC.hi;
		case 1: return &registerBC.lo;
		case 2: return &registerDE.hi;
		case 3: return &registerDE.lo;
		case 4: return &registerHL.hi;
		case 5: return &registerHL.lo;
		case 6: return &registerAF.lo;
		case 7: return &registerAF.hi;
		default: cout << "Not a valid register" << endl; break;
	}
	return nullptr;
	// TODO: insert return statement here
}

// Used to algorithmically decode which 16-bit register the bits point to
uint16_t * CPU::get16BitRegister(uint8_t bits)
{
	switch (bits)
	{
		case 0: cout << "bc " << endl; return &registerBC.reg;
		case 2: cout << "sp" << endl;  return &sp.reg;
		case 4: cout << "de" << endl; return &registerDE.reg;
		case 8:cout << "hl" << endl; return &registerHL.reg;
		case 12: cout << "af" << endl; return &registerAF.reg;
	}
	return nullptr;
}

// update the cpu's internal timers
void CPU::updateTimers(int cycles)
{
	// check whether clock is enabled - decides whether we can update TIMA
	uint8_t clockBit = Memory.readMemory(TAC);
	bool clockEnabled = (clockBit & 0b00000100) >> 2 == 1 ? true : false;

	// update divider register
	dividerRegister += cycles;
	if (dividerRegister >= 255) {
		dividerRegister = 0;
		Memory.incrementDivider();
	}

	uint8_t currClockFreq = getNewClockFreq();
	if (TACValue != currClockFreq)
	{
		timerCounter = currClockFreq;
		TACValue = currClockFreq;
	}

	if (clockEnabled)
	{
		timerCounter -= cycles;
		// if we have done enough cycles to increment 
		if (timerCounter <= 0)
		{
			
			timerCounter = getNewClockFreq();
			if (Memory.readMemory(TIMA) == 255) {
				Memory.writeMemory(TIMA, Memory.readMemory(TMA));
				setInterrupt(2);
			}
			else {
				Memory.writeMemory(TIMA, Memory.readMemory(TIMA) + 1);
			}
		}
	}
}

// get the frequency TIMA is currently incrementing at
int CPU::getNewClockFreq()
{
	uint8_t bits = Memory.readMemory(TAC) & 0b11;

	switch (bits)
	{
		case 0: return 4096;
		case 1: return 262144;
		case 2: return 65536;
		case 3: return 16384;
	}
}

// set interupt to happen
void CPU::setInterrupt(int bit)
{
	uint8_t reg = Memory.readMemory(0xFF0F);
	reg = set_bit(reg, bit);
	Memory.writeMemory(0xFF0F, reg);
}

/*
	Check if interrupts have been enabled and execute enabled interrupts
	in order
*/
void CPU::handleInterrupts()
{
	if (IME) {
		uint8_t enabledBits = Memory.readMemory(0xFFFF);
		uint8_t interruptFlags = Memory.readMemory(0xFF0F);

		// mean we have some interrupt
		if (interruptFlags > 0) {
			for (int i = 0; i < 5; i++) {
				if (getBit(i, interruptFlags) == 1) {
					if (getBit(i, enabledBits) == 1) {
						executeInterrupt(i);
					}
				}
			}

		}
	}
}

// execute interrupts which were enabled
void CPU::executeInterrupt(int bit)
{
	IME = false;
	uint8_t reg = Memory.readMemory(0xFF0F);
	reg = reset_bit(reg, bit);

	writeToStack(pc);
	switch (bit)
	{
	case 0: pc = 0x40; break;
	case 1: pc = 0x48; break;
	case 2: pc = 0x50; break;
	case 4: pc = 0x60; break;
	}
}

// Set a single bit in register
int CPU::set_bit(int reg, int bit)
{
	return reg |= (1U << bit);
}

// reset a single bit in register
int CPU::reset_bit(uint8_t &reg, int bit)
{
	return reg &= ~(1U << bit);
}

// reset a single bit in flag AF
void CPU::bitreset(int flag) {
	this->registerAF.reg &= ~(1U << flag);
}