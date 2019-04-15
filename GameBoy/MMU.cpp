#include "MMU.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;


uint8_t MMU::getJoyPadState()
{

	uint8_t res = mROM[0xFF00];
	if (!((res >> 4) & 0b00000001)) {
		uint8_t bottomJoyPad = keys >> 4;
		bottomJoyPad |= 0xF0;
		res &= bottomJoyPad;

	}
	else {
		uint8_t topJoyPad = keys & 0xF;
		topJoyPad |= 0xF0;
		res &= topJoyPad;
	}
	return res;
}

void MMU::writeInterruptState(uint8_t data)
{
	interruptflags = data;
}

uint8_t MMU::readMemory(uint16_t address) {

	// reading from from rom memory bank
	if ((address >= 0x4000) && (address <= 0x7FFF)) {
		uint16_t newAddress = address - 0x4000;
		return cartridgeMemory[newAddress + (currentROMBank * 0x4000)];
	}
	else if (address == 0xFF00) {
		return getJoyPadState();
	}
	else if (address == 0xFF0F) {
		// ei toimi
		return interruptflags;
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
	if (address == 0xFF0F) {
		return;
	}
	if (address == 0xFF00) {
		 uint8_t bit4 = (data >> 4) & 0b00000001;
		 uint8_t bit5 = (data >> 5) & 0b00000001;
		 uint8_t reg = mROM[0xFF00];
		 if (bit4) {
			 reg &= ~(1U << 4);
		 }
		 else {
			 reg |= (1U << 4);
		 }
		 if (bit5) {
			 reg &= ~(1U << 5);
		 }
		 else {
			 reg |= (1U << 5);
		 }

		 mROM[0xFF00] = reg;
		 return;
	}
	if (address == 0xFF80) {
		if (data == 0x89) {
			//cout << "juu";
		}
	}
	if (address == 0xD804) {
	//	cout << "juu";
	}
	if (address == 0xFF02 && data == 0x81) {
		cout << hex << readMemory(0xFF01);
	}
	if (address < 0x8000) {
		changeBanks(address, data);
	}
	else if ((address >= 0xA000) & (address < 0xC000)) {
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
	else if (address == 0xFF07 ) {
		uint8_t freq = readMemory(0xFF07) & 0x3;
		mROM[0xFF07] = data;
		uint8_t newFreq = readMemory(0xFF07) & 0x3;
		if (freq != newFreq) {
			timerChanged = true;
		}
	}
	else if (address == 0xFF07) {
		uint8_t currentFreq = readMemory(0xFF07) & 0b11;
		if ((data & 0b11 != currentFreq)) {
			mROM[address] = data;
		}
	}
	else if (address == 0xFF04) {
			mROM[address] = 0;
	}
	else if (address == 0xFF44) {
		mROM[address] = 0;
	}
	else if (address == 0xFF46) {
		dmaTransfer(data);
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
	FILE *in2;
	in2 = fopen("D:\\GameBoy\\GBEmu\\tests\\bootrom.gb", "rb");
	//fread(cartridgeMemory, 1, 0x256, in2);
	fclose(in);
	//fclose(in2);
	memcpy(mROM, cartridgeMemory, 0x8000);

	currentRAMBank = 0;


}

void MMU::init()
{
	
	mROM[0xFF00] = 0xCF;
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
	mROM[0xFF40] = 0x00;
	mROM[0xFF42] = 0x00;
	mROM[0xFF43] = 0x00;
	mROM[0xFF44] = 0x90;
	mROM[0xFF45] = 0x00;
	mROM[0xFF47] = 0xFC;
	mROM[0xFF48] = 0xFF;
	mROM[0xFF49] = 0xFF;
	mROM[0xFF4A] = 0x00;
	mROM[0xFF4B] = 0x00;
	mROM[0xFFFF] = 0x00;
	mROM[0xFF4D] = 0x7E;
	
	// nintendo logo
	mROM[0x104] = 0xCE;
	mROM[0x105] = 0xED;
	mROM[0x106] = 0x66;
	mROM[0x107] = 0x66;
	mROM[0x108] = 0xCC;
	mROM[0x109] = 0x0D;
	mROM[0x10A] = 0x00;
	mROM[0x10B] = 0x0B;
	mROM[0x10C] = 0x03;
	mROM[0x10D] = 0x73;
	mROM[0x10E] = 0x00;
	mROM[0x10F] = 0x83;
	mROM[0x110] = 0x00;
	mROM[0x111] = 0x0C;
	mROM[0x112] = 0x00;
	mROM[0x113] = 0x0D;
	mROM[0x114] = 0x00;
	mROM[0x115] = 0x08;
	mROM[0x116] = 0x11;
	mROM[0x117] = 0x1F;
	mROM[0x118] = 0x88;
	mROM[0x119] = 0x89;
	mROM[0x11A] = 0x00;
	mROM[0x11B] = 0x0E;
	mROM[0x11C] = 0xDC;
	mROM[0x11D] = 0xCC;
	mROM[0x11E] = 0x6E;
	mROM[0x11F] = 0xE6;
	mROM[0x120] = 0xDD;
	mROM[0x121] = 0xDD;
	mROM[0x122] = 0xD9;
	mROM[0x123] = 0x99;
	mROM[0x124] = 0xBB;
	mROM[0x125] = 0xBB;
	mROM[0x126] = 0x67;
	mROM[0x127] = 0x63;
	mROM[0x128] = 0x6E;
	mROM[0x129] = 0x0E;
	mROM[0x12A] = 0xEC;
	mROM[0x12B] = 0xCC;
	mROM[0x12C] = 0xDD;
	mROM[0x12D] = 0xDC;
	mROM[0x12E] = 0x99;
	mROM[0x12F] = 0x9F;
	mROM[0x130] = 0xBB;
	mROM[0x131] = 0xB9;
	mROM[0x132] = 0x33;
	mROM[0x133] = 0x3E;
	cout <<  "Muistissa 0xFF40: " << mROM[0xFF40] << endl;
	currentROMBank = 1;
	currentRAMBank = 0;
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

	uint8_t lowerFive = data & 31;
	currentROMBank &= 224;
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

void MMU::incrementScanLine()
{
	mROM[0xFF44]++;
}

void MMU::dmaTransfer(uint8_t data)
{
	uint16_t address = data << 8;
	for (int i = 0; i < 160; i++) {
		writeMemory(0xFE00 + i, readMemory(address + i));
	}
}

uint8_t MMU::getKey()
{
	return keys;
}

void MMU::setKey(uint8_t key)
{
	keys = key;
}

