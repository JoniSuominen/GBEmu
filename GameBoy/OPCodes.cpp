#include "CPU.h"


void CPU::loadMMU_reg8(uint16_t address, uint8_t data )
{
	Memory.writeMemory(address, data);
}
void CPU::opcode_nop() {
	cout << "no opertation" << endl;
}

void CPU::loadReg_reg16(Register reg)
{
	reg.lo = Memory.readMemory(this->pc);
	reg.hi = Memory.readMemory(this->pc+1);
}

void CPU::incr_reg16(uint16_t * address)
{
	*address += 1;
}
