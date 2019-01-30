#include <unordered_map>
#include <stdlib.h>
#include <iostream>
#include <bitset>
#include <fstream>
#define CPU_H
#include "MMU.h"
using namespace std;

class CPU
{
private:
	MMU Memory;
		typedef union Register
		{
			// 2 byte datatype for storing hi and lo bits
			uint16_t reg;
			struct
			{
				uint8_t lo;
				uint8_t hi;

			};
		}Register;
	Register registerAF, registerBC, registerDE, registerHL;
	Register sp;
	uint16_t pc;
	uint8_t m, t;


	const int FLAG_Z = 7;
	const int FLAG_N = 6;
	const int FLAG_H = 5;
	const int FLAG_C = 4;

	const int TIMA = 0xFF05;
	const int TMA = 0xFF06;
	const int TAC = 0xFF07;

	bool IME = true;

	const int DIVIDER = 0xFF04;

	int dividerRegister = 0;


	bool interruptsEnabled;


	const int CLOCKSPEED = 4194304;
	const int MAX_CYCLES = CLOCKSPEED / 60;

	int TACValue = 4096;
	int timerCounter = CLOCKSPEED / TACValue;
	int cycles = 0;


public:
	void start();
	void cycle();
	void executeOpCode(uint8_t opcode);
	void bitset(int flag);
	void bitreset(int flag);
	void bitset(int flag, int value);
	int getBit(int flag);
	int getBit(int bit, uint8_t reg);
	uint16_t readFromStack();
	void writeToStack(uint16_t data);
	void writeRegToMemory(Register reg);
	uint16_t jump16();
	uint16_t readTwoBytes();
	void init();
	uint8_t *get8BitRegister(uint8_t bits);
	uint16_t *get16BitRegister(uint8_t bits);
	void updateTimers(int cycles);
	int getNewClockFreq();
	void setInterrupt(int bit);
	void handleInterrupts();
	void executeInterrupt(int bit);
	int set_bit(int reg, int bit);
	int reset_bit(int reg, int bit);


	/* opcodes */

	// MISC	
	void opcode_nop();
	void opcode_stop();
	void opcode_cpl(uint8_t & value);
	void opcode_bcd(uint8_t & value);
	void opcode_scf();
	void opcode_CP(uint8_t reg1, uint8_t reg2);
	void opcode_CPmmu(uint8_t reg1, uint16_t pointer);	
	void opcode_popMmu(uint16_t &reg);
	void enable_interrupts();
	void opcode_di();
	void opcode_ei();
	void opcode_resetIME();

	// CALLS
	void call_false(int flag);
	void call_true(int flag);
	void call_nn();

	// PUSH
	void push_reg16(uint16_t reg);

	// JUMPS
	void jump_n();
	void jump_zero();
	void jump_carry();
	void jump_notzero();
	void jump_notcarry();
	void jump_abs();
	void jump_absFalse(int flag);
	void jump_absTrue(int flag);
	void jump_mmu(uint16_t reg);



	// LOAD
	void mmu_ldi(uint16_t address, uint8_t &data);
	void mmu_ldd(uint16_t address, uint8_t &data);
	void mmu_load8(uint16_t address, uint8_t data);
	void load8_imm(uint8_t &reg);
	void reg16_load(uint16_t &reg);
	void reg8_load(uint8_t & address);
	void opcode_load8(uint16_t address, uint8_t &destination);
	void opcode_ldi8(uint16_t address, uint8_t &destination);
	void opcode_ldd8(uint16_t address, uint8_t &destination);
	void opcode_mmucopy8(uint16_t mmulocation, uint8_t data);
	void ld_reg8(uint8_t loc, uint8_t data);
	void ldh_imm(uint8_t reg);
	void ldh_a(uint8_t &reg);
	void load_SP();
	void load_nnReg8(uint8_t reg);
	void opcode_ldhl();
	void load_SP_HL();
	void copy_reg8(uint8_t &destination, uint8_t source);
	void mmu_imm(uint16_t address);

	// ADD
	void add_16(uint16_t &destination, uint16_t source);
	void add_8(uint8_t value, uint8_t &destination);
	void add_mmu(uint16_t value, uint8_t &dest);
	void adc_reg8(uint8_t source, uint8_t &destination);
	void adc_imm(uint16_t pointer, uint8_t &destination);
	void adc_n(uint8_t &destination);
	void add_n(uint8_t &destination);
	void add_signedToSP();
	// SUBSTRACT
	void sub_reg8(uint8_t value, uint8_t &destination);
	void sub_mmu(uint16_t value, uint8_t &destination);
	void sbc_imm(uint16_t pointer, uint8_t &destination);
	void sub_n(uint8_t &destination);
	void sbc_n(uint8_t &destination);
	void sbc_reg8(uint8_t pointer, uint8_t &destination);

	// INCR
	void incr_reg(uint16_t &address);
	void incr_reg(uint8_t &address);
	void incp_reg(int16_t address);

	// AND, XOR, OR
	void and_reg8(uint8_t value, uint8_t &destination);
	void and_n(uint8_t &destination);
	void and_mmu(uint16_t value, uint8_t &destination);

	void xor_reg8(uint8_t value, uint8_t &destination);
	void xor_mmu(uint16_t value, uint8_t &destination);
	void xor_n(uint8_t &destination);

	void or_reg8(uint8_t value, uint8_t &destination);
	void or_mmu(uint16_t pointer, uint8_t &destination);
	void or_n(uint8_t &destination);

	// DECR
	void decr_reg(uint8_t &address);
	void decr_reg(uint16_t &address);
	void decp_reg(uint16_t value);

	// RESTARTS
	void restart(uint8_t n);

	// ROTATE
	void rlc_reg8(uint8_t &address);
	void rrc_reg8(uint8_t &address);
	void rl_reg8(uint8_t &address);
	void rr_reg8(uint8_t &address);
	void ext_rlc(uint16_t pointer);
	void ext_rrc(uint16_t pointer);
	void ext_rl(uint16_t pointer);
	void ext_rr(uint16_t pointer);

	// SHIFTS
	void ext_sla(uint8_t &address);
	void ext_sra(uint8_t &address);

	// RETURNS
	void opcode_ret();
	void opcode_retFalse(int flag);
	void opcode_retTrue(int Flag);
	

	// EXTENDED OPCODE MAP
	void test_bit(int bit, uint16_t reg);
	void ext_swap(uint8_t &reg);



};