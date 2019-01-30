#include "MMU.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;


uint8_t MMU::readMemory(uint16_t address) {
	// reading from from rom memory bank
	if ((address >= 0x4000) && (address <= 0x7FFF)) {
		uint16_t newAddress = address - 0x4000;
		return cartridgeMemory[newAddress + currentROMBank * 0x4000];
	}
	// reading from ram memory bank
	else if ((address >= 0xA000) && (address <= 0xBFFF)) {
		uint16_t newAddress = address - 0xA000;
		return ramBanks[newAddress + currentRAMBank * 0x2000];
	}
	else {
		return mROM[address];
	}
}

void MMU::writeMemory(uint16_t address, uint8_t data)
{
	if (address < 0x8000) {
		changeBanks(address, data);
	}
	else if (address >= 0xA000 & address < 0xC000) {
		if (enableRam) {
			uint16_t newAddress = address - 0xA000;
			ramBanks[newAddress + currentRAMBank * 0x2000] = data;
		}
	}
	else if (address >= 0xE000 && address < 0xFE00) {
		// writing to echo ram writes to working ram too	
		mROM[address] = data;
		mROM[address - 0x2000] = data;
	}
	else if (address >= 0xFEA0 && address < 0xFEFF) {
		return;
	}
	else if (address == 0xFF07) {
		uint8_t currentFreq = readMemory(0xFF07) & 0b11;
		if (data & 0b11 != currentFreq) {
			mROM[address] = data;
		}
	}
	else if (address = 0xFF04) {
			mROM[address] = 0;
	}
	else {
		mROM[address] = data;
	}

}

void MMU::incrementDivider()
{
	mROM[0xFF04]++;
}

void MMU::loadRom(const char* path)
{
	memset(cartridgeMemory, 0, sizeof(cartridgeMemory));
	cout << path << endl;
	FILE *in;
	in = fopen(path, "rb");
	fread(cartridgeMemory, 1, 0x20000, in);
	fclose(in);
	currentRAMBank = 0;
	/*
	for (int i = 0; i < (sizeof(cartridgeMemory) / sizeof(*cartridgeMemory)); i++) {
		cout << int( cartridgeMemory[i]) << endl;
	}
	*/
}

void MMU::init()
{
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
	currentROMBank = 1;
	switch (cartridgeMemory[0x147]) {
		case 1: MBC1 = true; break;
		case 2: MBC1 = true; break;
		case 3: MBC1 = true;  break;
		case 5: MBC2 = true; currentROMBank = 2;  break;
		case 6: MBC2 = true; currentROMBank = 2;  break;
	default: break;
	}
}

void MMU::changeBanks(uint16_t address, uint8_t data)
{
	if (address < 0x2000) {
		if (MBC1 || MBC2) {
			enableRamBank(address, data);
		}
	}
	else if (address >= 0x200 && address < 0x4000) {
		if (MBC1 || MBC2) {
			changeLowRamBank(data);
		}
	}
	else if (address >= 0x4000 && address <= 0x5FFF) {
		if (MBC1) {
			if (romBanking) {
				changeHiRomBank(data);
			}
			else {
				ramBankChange(data);
			}
		}
	}
	else if (address >= 0x6000 && address <= 0x7FFF) {
		if (MBC1) {
			changeRomRamMode(data);
		}
	}
}

void MMU::enableRamBank(uint16_t address, uint8_t data)
{
	if (MBC2)
	{
		if ((address & 0b1000) >> 3 == 0) return;

		uint8_t test = data & 0xF;
		if (test == 0xA) {
			enableRam = true;
		}
		else if (test = 0x0) {
			enableRam = false;
		}

	}
}

void MMU::changeLowRamBank(uint8_t data)
{
	if (MBC2) {
		currentROMBank = data & 0xF;
		if (currentROMBank == 0) {
			currentROMBank++;
		}
		return;
	}

	uint8_t lowerFive = data & 0b00011111;
	currentROMBank &= 0b11100000;
	currentROMBank |= lowerFive;
	// ROM BANK cant be 0 as that equals the bank in 0x0000 - 0x4000
	if (currentROMBank == 0) {
		currentROMBank++;
	}
}

void MMU::changeHiRomBank(uint8_t data)
{
	// in the are 0x6000 - 0x5FFF ROM bank changes are made through changing upper 5 bytes of data
	currentROMBank &= 0b00011111;


	data &= 0b11100000;
	currentROMBank |= data;
	if (currentROMBank == 0) {
		currentROMBank++;
	}

}

void MMU::ramBankChange(uint8_t data)
{
	currentRAMBank = data & 0b11;
}

void MMU::changeRomRamMode(uint8_t data)
{
	uint8_t newData = data & 0x1;

	romBanking = newData == 0 ? true : false;
	if (romBanking) {
		currentRAMBank = 0;
	}
}

