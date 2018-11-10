#include "CPU.h"


void CPU::mmu_load(uint16_t address, uint8_t data )
{
	Memory.writeMemory(address, data);
}
// do nothing
void CPU::opcode_nop() {
	cout << "no operation" << endl;
}

// load data from memory into 16-bit register
void CPU::reg16_load(Register *reg)
{
	reg->lo = Memory.readMemory(this->pc);
	reg->lo = Memory.readMemory(this->pc+1);
}

// load 8-bit immediate into register
void CPU::reg8_load(uint8_t *address)
{
	*address = Memory.readMemory(this->pc);
}

// increment 16-bit register by one
void CPU::incr_reg(uint16_t * address)
{
	*address += 1;
}

// increment 8-bit register by one
void CPU::incr_reg(uint8_t * address)
{
	*address += 1;
}
// increment 8-bit register by one
void CPU::decr_reg(uint8_t * address)
{
	*address -= 1;
}
/* Rotate left with carry -
   rest to be implemented later as I believe
   I can use the set bit-opcode here
*/
void CPU::rlc_reg8(uint8_t *address)
{
	uint8_t original = *address;
	*address = original << 1;
	uint8_t lsb = original >> 7;
	*address = *address | lsb;
	cout << std::bitset<8>(*address) << endl;	
}
