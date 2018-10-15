#include "MMU.h"
#include <iostream>;
using namespace std;


uint8_t MMU::readMemory(uint16_t address) {
	return rom[address];
}

void MMU::writeMemory(uint16_t address, uint8_t data)
{
	rom[address] = data;
}

