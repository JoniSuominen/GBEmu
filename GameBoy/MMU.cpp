#include "MMU.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;


uint8_t MMU::readMemory(uint16_t address) {
	return mROM[address];
}

void MMU::writeMemory(uint16_t address, uint8_t data)
{
	if (address < 0x8000) {
		return;
	}
	else if (address >= 0xE000 && address < 0xFE00) {
		// writing to echo ram writes to working ram too	
		mROM[address] = data;
		mROM[address - 0x2000] = data;
	}
	else if (address >= 0xFEA0 && address < 0xFEFF) {
		return;
	}
	else {
		mROM[address] = data;
	}

}

void MMU::loadRom(const char* path)
{
	memset(cartridgeMemory, 0, sizeof(cartridgeMemory));

	FILE *in;
	in = fopen(path, "rb");
	fread(cartridgeMemory, 1, 0x20000, in);
	fclose(in);

}

void MMU::init()
{
	
	mROM[0x100] = 0x01;
	mROM[0xFF05] = 0x00;
	mROM[0xFF06] = 0x00;
	mROM[0xFF07] = 0x00;
	mROM[0xFF10] = 0x80;
	mROM[0xFF11] = 0xBF;
	mROM[0xFF12] = 0xF3;
	mROM[0xFF14] = 0xBF;
	mROM[0xFF16] = 0x3F;
	mROM[0xFF17] = 0x00;
	mROM[0xFF19] = 0xBF;
	mROM[0xFF1A] = 0x7F;
	mROM[0xFF1B] = 0xFF;
	mROM[0xFF1C] = 0x9F;
	mROM[0xFF1E] = 0xBF;
	mROM[0xFF20] = 0xFF;
	mROM[0xFF21] = 0x00;
	mROM[0xFF22] = 0x00;
	mROM[0xFF23] = 0xBF;
	mROM[0xFF24] = 0x77;
	mROM[0xFF25] = 0xF3;
	mROM[0xFF26] = 0xF1;
	mROM[0xFF40] = 0x91;
	mROM[0xFF42] = 0x00;
	mROM[0xFF43] = 0x00;
	mROM[0xFF45] = 0x00;
	mROM[0xFF47] = 0xFC;
	mROM[0xFF48] = 0xFF;
	mROM[0xFF49] = 0xFF;
	mROM[0xFF4A] = 0x00;
	mROM[0xFF4B] = 0x00;
	mROM[0xFFFF] = 0x00;
}

