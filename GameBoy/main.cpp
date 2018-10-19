#include <iostream>
#include "CPU.h"

using namespace std;

int main() {
	uint8_t *pointer = new uint8_t;
	*pointer = 0b11000001;
	CPU h;	
	h.rlc_reg8(pointer);
	system("pause");
}