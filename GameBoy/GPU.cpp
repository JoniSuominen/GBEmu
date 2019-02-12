#include "GPU.h"

GPU::GPU(CPU& processor, MMU &memory)
{
	cpu = processor;
	mem = memory;
}

void GPU::update(int cycles)
{
	if (LCDStatus() == 1) {
		scanlineClock += cycles;
	}

	if (scanlineClock >= 456) {
		mem.incrementScanLine();
		uint8_t nextScanLine = mem.readMemory(0xFF44);
		scanlineClock = 0;

		if (nextScanLine == 144) {
			cpu.setInterrupt(0);
		}

		if (nextScanLine == 153) {
			mem.writeMemory(0xFF04, 0);
		}

	}
}

void GPU::setStatus()
{
	uint8_t gpuStatus = mem.readMemory(0xFF41);

	if (!LCDStatus()) {
		scanlineClock = 0;
		mode = 1;
		gpuStatus &= 0b11111100;
		gpuStatus = cpu.set_bit(gpuStatus, 0);
		mem.writeMemory(0xFF41, gpuStatus);
		return;
	}

	uint8_t currLine = mem.readMemory(0xFF44);
	uint8_t currMode = gpuStatus & 0b11;

	uint8_t mode = 0;
	bool interrupt = false;

	if (currLine >= 144) {
		mode = 1;
		gpuStatus = cpu.set_bit(gpuStatus, 0);
		gpuStatus = cpu.reset_bit(gpuStatus, 1);
		interrupt = cpu.getBit(4, gpuStatus);
	}
	else {
		if (scanlineClock <= 80) {
			mode = 2;
			gpuStatus = cpu.set_bit(gpuStatus, 1);
			gpuStatus = cpu.reset_bit(gpuStatus, 0);
			interrupt = cpu.getBit(5, gpuStatus);
		}
		else if (scanlineClock <= 170) {
			mode = 3;
			gpuStatus = cpu.set_bit(gpuStatus, 1);
			gpuStatus = cpu.set_bit(gpuStatus, 0);
		}
		else {
			mode = 0;
			gpuStatus = cpu.reset_bit(gpuStatus, 1);
			gpuStatus = cpu.reset_bit(gpuStatus, 0);
			interrupt = cpu.getBit(3, gpuStatus);
		}
	}

	if (interrupt && (mode != currMode)) {
		cpu.setInterrupt(1);
	}
	if (currLine == mem.readMemory(0xFF45))
	{
		gpuStatus = cpu.set_bit(gpuStatus, 2);
		if (cpu.getBit(6, gpuStatus)) {
			cpu.setInterrupt(1);
		}
	}
	else {
		gpuStatus = cpu.reset_bit(gpuStatus, 2);
	}
	mem.writeMemory(0xFF41, gpuStatus);


}



bool GPU::LCDStatus()
{
	return mem.readMemory(0xFF40) >> 7 == 1;
}
