#include "CPU.h"

void CPU::test_bit(int bit, uint16_t reg)
{
	int num = getBit(bit, reg);
	if (num == 0) {
		bitset(FLAG_Z);
	}
	else {
		bitreset(FLAG_Z);
	}

	bitreset(FLAG_N);
	bitset(FLAG_H);
}