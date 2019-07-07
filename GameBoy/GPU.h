#pragma once
#include "MMU.h"
#include <SFML/Graphics.hpp>
#ifndef GPU_H
#define GPU_H

using namespace std;
class CPU;
class GPU {
private:

	void renderTiles();
	void setStatus();
	bool LCDStatus();
	int scanlineClock = 0;
	int mode = 2;
	void renderSprites();
	
	void drawScanline();
	uint8_t getColor(uint8_t pixelColor, uint16_t address);

public:
	void init();
	uint8_t screen_data[160][144] = {};
	MMU *mem;
	CPU *cpu;
	GPU();
	void update(int cycles);
};
#endif// CPU_H