#include <unordered_map>
#include <iostream>
#define CPU_H
using namespace std;

class CPU
{
private:
	union Register
	{
		// 2 byte datatype for storing hi and lo bits
		__int16 reg;
		struct
		{
			uint8_t lo;
			uint8_t hi;

		};
	};
	unordered_map<char, int> clock = {
		{
			'm', 0
		},
		{
			't', 0
		}
	};

	Register registers[4] = { RegisterBC, RegisterDE, RegisterHL, RegisterAF };
	uint16_t pc, sp;
	uint8_t m, t;

public:
	void executeOpCode();

};