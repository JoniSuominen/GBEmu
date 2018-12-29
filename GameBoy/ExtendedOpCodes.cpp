#include "CPU.h"

void CPU::test_bit(int bit, uint8_t reg)
{
	int bit = getBit(bit, reg);
	if (bit == 0) {
		bitset(FLAG_Z);
	}
	else {
		bitreset(FLAG_Z);
	}

	bitreset(FLAG_N);
	bitset(FLAG_H);
}