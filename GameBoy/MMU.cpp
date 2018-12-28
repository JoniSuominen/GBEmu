#include "MMU.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;


uint8_t MMU::readMemory(uint16_t address) {
	return rom[address];
}

void MMU::writeMemory(uint16_t address, uint8_t data)
{
	rom[address] = data;
}

void MMU::loadRom(string path)
{
	string binaryNumber;
	ifstream romPath;
	romPath.open(path);
	int counter = 0;
	if (romPath.is_open()) {
		while (getline(romPath, binaryNumber)) {
			uint8_t block;
			copy(binaryNumber.begin(), binaryNumber.end(), block);
			cartridgeMemory[counter] = block;
			counter++;
		}
	}
}

