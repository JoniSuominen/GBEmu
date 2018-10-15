#include <iostream>
#include "CPU.h"

using namespace std;

int main() {
	int *pointer = new int;
	*pointer = 5;
	cout << *pointer << endl;
	cout << " hei" << endl;
	CPU h;	
	h.executeOpCode(0x0);
	system("pause");
}