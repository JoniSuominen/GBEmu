#include <iostream>
#include <string>
using namespace std;

class MMU {
private:
	char bios_status = 1;
	uint8_t bios[0x100];
	uint8_t cartridgeMemory[0x20000];
	uint8_t mROM[0x10000] = { 0 };
 // all the mapped memory together equals 65kB.


	public:
		uint8_t readMemory(uint16_t address);
		void writeMemory(uint16_t address, uint8_t data);
		void loadRom(const char* path);
		void init();
};
