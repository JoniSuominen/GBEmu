#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <string>
#pragma once
using namespace std;

class MMU {
	private:
		char bios_status = 1;
		uint8_t bios[0x100];
		uint8_t cartridgeMemory[0x20000];
		uint8_t mROM[65536] = { 0 };
		uint8_t ramBanks[0x8000] = { 0 };
		uint8_t keys = 0x3F;
		bool MBC1 = false;
		bool MBC2 = false;
		bool enableRam = false;	
		bool romBanking = true;
		uint8_t currentROMBank = 1;
		uint8_t currentRAMBank;
		uint8_t getJoyPadState();
		uint8_t interruptflags = 0;
	 // all the mapped memory together equals 65kB.


	public:
		void writeInterruptState(uint8_t data);
		bool timerChanged = false;
		uint8_t readMemory(uint16_t address);
		void writeMemory(uint16_t address, uint8_t data);
		void incrementDivider();
		void loadRom(const char* path);
		void init();
		void changeBanks(uint16_t address, uint8_t data);
		void enableRamBank(uint16_t address, uint8_t data);
		void changeLowRamBank(uint8_t data);
		void changeHiRomBank(uint8_t data);
		void ramBankChange(uint8_t data);
		void changeRomRamMode(uint8_t data);
		void incrementScanLine();
		void dmaTransfer(uint8_t data);
		uint8_t getKey();
		void setKey(uint8_t key);

};
