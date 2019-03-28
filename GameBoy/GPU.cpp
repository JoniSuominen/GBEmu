#include "GPU.h"
#include "CPU.h"


GPU::GPU()
{
}

void GPU::update(int cycles)
{
	setStatus();

	if (LCDStatus()) {
		scanlineClock += cycles;
	}
	else {
		return;
	}

	if (scanlineClock >= 456) {
		mem->incrementScanLine();
		uint8_t nextScanLine = mem->readMemory(0xFF44);
		scanlineClock = 0;

		if (nextScanLine == 144) {
			cpu->setInterrupt(0);
		}

		else if (nextScanLine > 153) {
			mem->writeMemory(0xFF44, 0);
		}
		else if (nextScanLine < 144) {

			drawScanline();
		}

	}
}

void GPU::setStatus()
{
	uint8_t gpuStatus = mem->readMemory(0xFF41);

	if (!LCDStatus()) {
		scanlineClock = 0;
		gpuStatus &= 252;
		mem->writeMemory(0xFF44, 0);
		gpuStatus = cpu->set_bit(gpuStatus, 0);
		mem->writeMemory(0xFF41, gpuStatus);
		return;
	}

	uint8_t currLine = mem->readMemory(0xFF44);
	uint8_t currMode = gpuStatus & 0x3;

	uint8_t mode = 0;
	bool interrupt = false;

	if (currLine >= 144) {
		mode = 1;
		gpuStatus = cpu->set_bit(gpuStatus, 0);
		gpuStatus = cpu->reset_bit(gpuStatus, 1);
		interrupt = cpu->getBit(4, gpuStatus);
	}
	else {
		if (scanlineClock <= 80) {
			mode = 2;
			gpuStatus = cpu->set_bit(gpuStatus, 1);
			gpuStatus = cpu->reset_bit(gpuStatus, 0);
			interrupt = cpu->getBit(5, gpuStatus);
		}
		else if (scanlineClock <= 252) {
			mode = 3;
			gpuStatus = cpu->set_bit(gpuStatus, 1);
			gpuStatus = cpu->set_bit(gpuStatus, 0);
		}
		else {
			mode = 0;
			gpuStatus = cpu->reset_bit(gpuStatus, 1);
			gpuStatus = cpu->reset_bit(gpuStatus, 0);
			interrupt = cpu->getBit(3, gpuStatus);
		}
	}

	if (interrupt && (mode != currMode)) {
		cpu->setInterrupt(1);
	}
	if (currLine == mem->readMemory(0xFF45))
	{
		gpuStatus = cpu->set_bit(gpuStatus, 2);
		if (cpu->getBit(6, gpuStatus)) {
			cpu->setInterrupt(1);
		}
	}
	else {
		gpuStatus = cpu->reset_bit(gpuStatus, 2);
	}
	mem->writeMemory(0xFF41, gpuStatus);


}



bool GPU::LCDStatus()
{
	uint8_t val = mem->readMemory(0xFF40);
	return cpu->getBit(7, val);
}


void GPU::renderSprites()
{
	uint8_t lcdControl = mem->readMemory(0xFF40);
	bool use8x16 = false;
	if (cpu->getBit(2, lcdControl))
		use8x16 = true;

	for (int sprite = 0; sprite < 40; sprite++) {
		uint8_t index = sprite * 4;
		uint8_t positionY = mem->readMemory(0xFE00 + index) - 16;
		uint8_t positionX = mem->readMemory(0xFe00 + index + 1) - 8;
		uint8_t tileLoc = mem->readMemory(0xFE00 + index + 2);
		uint8_t attributes = mem->readMemory(0xFe00 + index + 3);

		bool yFlip = cpu->getBit(6, attributes);
		bool xFlip = cpu->getBit(5, attributes);

		int scanLine = mem->readMemory(0xFF44);
		int height = 8;
		if (use8x16) {
			height = 16;
		}

		if ((scanLine >= positionY) && (scanLine < (positionY + height))) {
			int line = scanLine - positionY;

			// if a sprite is flipped we read data from opposite side of table 
			if (yFlip) {
				line -= height;
				line *= -1;
			}

			line *= 2;

			uint16_t address = (0x8000 + (tileLoc * 16) + line);
			uint8_t data1 = mem->readMemory(address);
			uint8_t data2 = mem->readMemory(address + 1);


			for (int tilePixel = 7; tilePixel >= 0; tilePixel--) {
				int colorbit = tilePixel;
				if (xFlip) {
					colorbit -= 7;
					colorbit *= -1;
				}

				int colorNum = cpu->getBit(colorbit, data2);
				colorNum <<= 1;
				colorNum |= cpu->getBit(colorbit, data1);

				uint16_t paletteAddress = cpu->getBit(4, attributes) ? 0xFF49 : 0xFF48;
				uint8_t color = getColor(colorNum, paletteAddress);




				int xPix = 0 - tilePixel;
				xPix += 7;

				int pixel = positionX + xPix;


				if ((scanLine < 0) || scanLine > 144 || (pixel < 0) || pixel > 40) {
					continue;
				}

				screen_data[pixel][scanLine] = color;
			}
		}
	}
}


void GPU::drawScanline()
{
	uint8_t controlByte = mem->readMemory(0xFF40);
		if (cpu->getBit(0, controlByte)) {
			renderTiles();
		}

		if (cpu->getBit(1, controlByte)) {
			renderSprites();
		}
	
}

uint8_t GPU::getColor(uint8_t pixelColor, uint16_t address)
{

	uint8_t defaultColor = 0;
	uint8_t palette = mem->readMemory(address);
	int hi = 0;
	int lo = 0;
	switch (pixelColor) {
	case 0: hi = 1; lo = 0; break;
	case 1: hi = 3; lo = 2;break;
	case 2: hi = 5; lo = 4; break;
	case 3: hi = 7; lo = 6; break;
	}

	defaultColor = cpu->getBit(hi, palette) << 1;
	
	defaultColor |= cpu->getBit(lo, palette);
	if (defaultColor == 205) {
		cout << "xd" << endl;
	}
	return defaultColor;
}

void GPU::init()
{

}



void GPU::renderTiles()
{



	uint8_t lcdControl = mem->readMemory(0xFF40);
	uint16_t tileData = cpu->getBit(4, lcdControl) ? 0x8000 : 0x8800;
	uint8_t currScanline = mem->readMemory(0xFF44);

	uint8_t scrollY = mem->readMemory(0xFF42);
	uint8_t scrollX = mem->readMemory(0xFF43);
	uint8_t windowY = mem->readMemory(0xFF4A);
	uint8_t windowX = mem->readMemory(0xFF4B) - 7;
	bool unsig = true;
	if (tileData == 0x8800) {
		unsig = false;
	}
	bool windowEnabled = false;

	// is the window enabled?f
	if (cpu->getBit(5, lcdControl)) {
		// there is no point in drawing the window if its located under the current scanline
		if (windowY <= mem->readMemory(0xFF44)) {
			windowEnabled = true;
		}

	}
		uint16_t identifierLocation;
		uint8_t positionY = 0;
		// if window is enabled we use the window background memory map per Pan doc gb docs
		if (windowEnabled) {
			identifierLocation = cpu->getBit(6, lcdControl) == 1 ? 0x9C00 : 0x9800;
			// if the window is enabled we can get the y-position of the place we need to draw via
			// window y since window y tells us distance from the area of the first place we need to draw
			positionY = mem->readMemory(0xFF44) - windowY;
		}
		else {
			identifierLocation = cpu->getBit(3, lcdControl) == 1 ? 0x9C00 : 0x9800;
			// if windows is not enabled we get the edge y-position of the area we need to draw
			// by adding current scanline and scroll-y
			positionY = mem->readMemory(0xFF44) + scrollY;
		}

		uint16_t tileRow = (((uint8_t)(positionY / 8)) * 32);

		// draw pixels horizontally
		for (int pixel = 0; pixel < 160; pixel++) {
			uint8_t positionX = pixel + scrollX;
			if (windowEnabled) {
				if (pixel >= windowX) {
					positionX = pixel - windowX;
				}
			}

			uint16_t tileCol = (positionX / 8);

			int16_t tileNumber;

			uint16_t tileAddress = identifierLocation + tileRow + tileCol;

			if (unsig) {
				tileNumber = mem->readMemory(tileAddress);
			}
			else {
				tileNumber = static_cast<int8_t>(mem->readMemory(tileAddress));
			}

			uint16_t tileLocation = tileData;

			if (unsig) {
				tileLocation += tileNumber * 16;
			}
			else {
				tileLocation += (tileNumber + 128) * 16;
			}

			// which col of tile
			uint8_t line = (positionY % 8);
			line *= 2;

			uint8_t data1 = mem->readMemory(tileLocation + line);
			uint8_t data2 = mem->readMemory(tileLocation + line + 1);

			int colorBit = positionX % 8;
			// pixel 0 is bit 7, so on
			// 0 - 7 is -7, multiplied by -1 is 7 so we can get the 7th bit of the data
			colorBit -= 7;
			colorBit *= -1;

			int colorNum = cpu->getBit(colorBit, data2);
			colorNum <<= 1;
			colorNum |= cpu->getBit(colorBit, data1);
			uint8_t color = getColor(colorNum, 0xFF47);
			int finaly = mem->readMemory(0xFF44);

			// safety check to make sure what im about 
			// to set is int the 160x144 bounds
			

			screen_data[pixel][finaly] = color;
		}
	

}

