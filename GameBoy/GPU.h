#pragma once
#include "MMU.h"
#include "CPU.h"

class GPU {
private:
	MMU mem;
	CPU cpu;
	GPU(CPU &processor, MMU &memory);
	void update(int cycles);
	void setStatus();
	bool LCDStatus();
	int scanlineClock = 0;
	int mode = 2;
};