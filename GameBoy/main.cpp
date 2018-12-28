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

	cout << 0xFFFF + 0xFFFF << endl;
	int i = 0b1111;
	i &= ~(1UL << 1);
	cout << bitset<4>(i) << endl;
	system("pause");
}