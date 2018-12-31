#include <stdlib.h>
#include <iostream>
#include "CPU.h"

using namespace std;

int main() {
	union Register
	{
		// 2 byte datatype for storing hi and lo bits
		uint16_t reg;
		struct reg8
		{
			unsigned char lo;
			unsigned char hi;

		} reg8;
	};


	CPU cpu;
	cpu.init();
	cpu.executeOpCode(0x16);
	system("pause");

}