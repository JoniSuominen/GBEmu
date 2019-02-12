#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <string>

using namespace std;

class MMU {
private:
	char bios_status = 1;
	uint8_t bios[0x100];
	uint8_t cartridgeMemory[0x20000];
	uint8_t mROM[0x10000] = { 0 };
	uint8_t ramBanks[0x8000] = { 0 };
	bool MBC1 = false;
	bool MBC2 = false;
	bool enableRam = false;
	bool romBanking = true;
	uint8_t currentROMBank;
	uint8_t currentRAMBank;
 // all the mapped memory together equals 65kB.


	public:
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

};
