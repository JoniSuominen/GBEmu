#include "CPU.h"
#include "GPU.h"
using namespace std;



sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight) {

	// Compares the aspect ratio of the window to the aspect ratio of the view,
	// and sets the view's viewport accordingly in order to archieve a letterbox effect.
	// A new view (with a new viewport set) is returned.

	float windowRatio = windowWidth / (float)windowHeight;
	float viewRatio = view.getSize().x / (float)view.getSize().y;
	float sizeX = 1;
	float sizeY = 1;
	float posX = 0;
	float posY = 0;

	bool horizontalSpacing = true;
	if (windowRatio < viewRatio)
		horizontalSpacing = false;

	// If horizontalSpacing is true, the black bars will appear on the left and right side.
	// Otherwise, the black bars will appear on the top and bottom.

	if (horizontalSpacing) {
		sizeX = viewRatio / windowRatio;
		posX = (1 - sizeX) / 2.f;
	}

	else {
		sizeY = windowRatio / viewRatio;
		posY = (1 - sizeY) / 2.f;
	}

	view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));

	return view;
}

void CPU::start()
{
	Memory.loadRom("D:\\GameBoy\\GBEmu\\Tetris.gb");
	init();
	sf::RenderWindow window(sf::VideoMode(160, 144, 32), "GBEmu");
	sf::Clock timer;
	sf::Clock cpuTimer;
	
	
	sf::Uint8* pixels = new sf::Uint8[160 * 144 * 4	];
	sf::Image background;
	sf::Texture texture;
	sf::Sprite sprite;
	sf::View view;
	view.setSize(160, 144);
	view.setCenter(view.getSize().x / 2, view.getSize().y / 2);
	view = getLetterboxView(view, 160, 144);
	int rate = 1000 / 60;
	cout << time << endl;
	sf::Event event;
	while (window.isOpen()) {
		window.pollEvent(event);
					cycle();
				//sf::Time time = timer.getElapsedTime();
				//if (time.asMilliseconds() > rate) {
					window.clear();
					for (int x = 0; x < 160; x++) {
						for (int y = 0; y < 144; y++) {
							switch (gpu->screen_data[x][y]) {
							case 0:  pixels[(x + y * 160) * 4] = 255; pixels[(x + y * 160) * 4 + 1] = 255; pixels[(x + y * 160) * 4 + 2] = 255; pixels[(x + y * 160) * 4 + 3] = 255; break;
							case 1:  pixels[(x + y * 160) * 4] = 0xB6; pixels[(x + y * 160) * 4 + 1] = 0xB6; pixels[(x + y * 160) * 4 + 2] = 0xB6; pixels[(x + y * 160) * 4 + 3] = 255; break;
							case 2:  pixels[(x + y * 160) * 4] = 0x67; pixels[(x + y * 160) * 4 + 1] = 0x67; pixels[(x + y * 160) * 4 + 2] = 0x67; pixels[(x + y * 160) * 4 + 3] = 255; break;
							case 3:  pixels[(x + y * 160) * 4] = 0; pixels[(x + y * 160) * 4 + 1] = 0; pixels[(x + y * 160) * 4 + 2] = 0; pixels[(x + y * 160) * 4 + 3] = 255; 	break;
							default: cout << (int) gpu->screen_data[x][y]<< endl; break;
							}


						}
					}

					background.create(160, 144, pixels);
					texture.loadFromImage(background);
					sprite.setTexture(texture);
					window.setView(view);
					window.draw(sprite);
					window.display();
					if (event.type == sf::Event::KeyPressed) {
						if (event.key.code == sf::Keyboard::Z) {
							handleKeyboard(4);
						}
						else if (event.key.code == sf::Keyboard::X) {
							handleKeyboard(5);
						} 
						else if (event.key.code == sf::Keyboard::Return) {
							handleKeyboard(7);
						}
						else if (event.key.code == sf::Keyboard::Escape) {
							handleKeyboard(6);
						}
						else if (event.key.code == sf::Keyboard::Right) {
							handleKeyboard(0);
						}
						else if (event.key.code == sf::Keyboard::Left) {
							handleKeyboard(1);
						}
						else if (event.key.code == sf::Keyboard::Up) {
							handleKeyboard(2);
						}
						else if (event.key.code == sf::Keyboard::Down) {
							handleKeyboard(3);
						}
						
					}
					else if (event.type == sf::Event::KeyReleased) {
						if (event.key.code == sf::Keyboard::Z) {
							handleRelease(4);
						}
						else if (event.key.code == sf::Keyboard::X) {
							handleRelease(5);
						}
						else if (event.key.code == sf::Keyboard::Return) {
							handleRelease(7);
						}
						else if (event.key.code == sf::Keyboard::Space) {
							handleRelease(6);
						}
						else if (event.key.code == sf::Keyboard::Right) {
							handleRelease(0);
						}
						else if (event.key.code == sf::Keyboard::Left) {
							handleRelease(1);
						}
						else if (event.key.code == sf::Keyboard::Up) {
							handleRelease(2);
						}
						else if (event.key.code == sf::Keyboard::Down) {
							handleRelease(3);
						}
					}
					//timer.restart()	
		}
		
	

	
}
void CPU::handleKeyboard(int key)
{
	bool unset = false;
	keys = Memory.getKey();
	if (getBit(keys, key) == 0) {
		unset = true;
	}
	keys = reset_bit(keys, key);
	Memory.setKey(keys);
	bool button = true;
	if (key > 3) {
		button = true;
	}
	else {
		button = false;
	}
	uint8_t keyRegister = Memory.readMemory(0xFF00);

	bool reqInterupt = false;
	if (button && !getBit(5, keyRegister)) {
		reqInterupt = true;
	}

	else if (!button && !getBit(4, keyRegister)) {
		reqInterupt = true;
	}

	if (reqInterupt && !unset) {
		setInterrupt(4);
	}

}

void CPU::handleRelease(int key)
{
	keys = Memory.getKey();
	keys = set_bit(keys, key);
	Memory.setKey(keys);
}

// main emulation loop
void CPU::cycle() {
	int cyclesBefore = 0;
	timerCounter = CLOCKSPEED / 1024;
	while (this->cycles < MAX_CYCLES) {
		if (pc == 0x100) {
			registerAF.reg = 0x1180;
			registerBC.reg = 0x0;
			registerDE.reg = 0xFF56;
			registerHL.reg = 0xD;
		}


  	 		uint8_t opcode = Memory.readMemory(this->pc);
			pc++;
					executeOpCode(opcode);
		// op r, (hl) 19 kertaa!!!
		if (pc == 0x0061) {
			cout << "";
		}
		if (pc == 0x021B) {
			cout << "";
		}
		if (pc == 0x02A8) {
			cout << "";
		}
		if (pc == 0xC17F) {
			cout << "";
		}
		if (pc == 0xC092) {
			cout << "";
		}
		
		int opcodeCycles = cycles - cyclesBefore;
		if (Memory.timerChanged) {
			timerCounter = getNewClockFreq();
			Memory.timerChanged = false;
		}
		updateTimers(opcodeCycles);
	handleInterrupts();
					gpu->update(opcodeCycles);
		//gpu->update(opcodeCycles);
		cyclesBefore = cycles;
	}
	cycles = 0;
}


// set flag on
void CPU::bitset(int flag)
{
	this->registerAF.reg |= (1U << flag);
}

// set a flag to a value
void CPU::bitset(int flag, int value)
{
	uint16_t bit = value;
	bitreset(flag);	
	this->registerAF.reg |= (bit << flag);
}

// returns single flag
int CPU::getBit(int flag)
{	
	return (this->registerAF.reg >> flag) & 0b1;
}

// return a single bits value from register
int CPU::getBit(int bit, uint8_t reg)
{
	return (reg >> bit) & 0b00000001;
}

// read a 16-bit address from the stack
uint16_t CPU::readFromStack()
{
	uint16_t data1 = (Memory.readMemory(this->sp.reg));
	this->sp.reg++;
	uint16_t data2 = (Memory.readMemory(this->sp.reg));
	this->sp.reg++;
	uint16_t address = (data2 << 8) | data1;
	return address;
}

// write a 16-bit register to the stack
void CPU::writeToStack(uint16_t data)
{
	this->sp.reg--;
	uint8_t hi = data >> 8;
	uint8_t lo = data & 0xFF;
	Memory.writeMemory(this->sp.reg, hi);
	this->sp.reg--;
	Memory.writeMemory(this->sp.reg, lo);
}

// write two bytes to memory
void CPU::writeRegToMemory(Register reg)
{
	uint16_t nn = readTwoBytes();
	Memory.writeMemory(nn, reg.lo);
	nn++;
	Memory.writeMemory(nn, reg.hi);
}

// return the address you must set the program counter to in case of a jump command
uint16_t CPU::jump16()
{
	uint16_t address = ((Memory.readMemory(this->pc + 1))<< 8) | (Memory.readMemory(this->pc));
	return address;
}

// read two bytes from memory and combine them to single 16-bit integer
uint16_t CPU::readTwoBytes()
{
	uint16_t data1 = (Memory.readMemory(this->pc + 1) << 8);
	uint16_t data2 = (Memory.readMemory(this->pc));
	data1 |= data2;
	pc += 2;
	return data1;
}

// initializes the cpu (and mmu)
void CPU::init()
{
	gpu = new GPU();
	gpu->cpu = this;
	
	pc = 0x100;
	registerAF.reg = 0x01B0;
	registerBC.reg = 0x0013;
	registerDE.reg = 0x00D8;
	registerHL.reg = 0x014D;
	sp.reg = 0xFFFE;
	Memory.init();
	gpu->mem = &Memory;

}

// returns an 8-bit register based on the bit order
uint8_t * CPU::get8BitRegister(uint8_t bits)
{
	switch (bits)
	{
	case 0:   return &registerBC.hi;
	case 1: return &registerBC.lo;
		case 2: return &registerDE.hi;
		case 3: return &registerDE.lo;
		case 4: return &registerHL.hi;
		case 5: return &registerHL.lo;
		case 6: return &registerAF.lo;
		case 7: return &registerAF.hi;
		default: cout << "Not a valid register" << endl; break;
	}
	return nullptr;
	// TODO: insert return statement here
}

// Used to algorithmically decode which 16-bit register the bits point to
uint16_t * CPU::get16BitRegister(uint8_t bits)
{
	switch (bits)
	{
		case 0:  return &registerBC.reg;
		case 2:   return &sp.reg;
		case 4:  return &registerDE.reg;
		case 8: return &registerHL.reg;
		case 12:  return &registerAF.reg;
	}
	return nullptr;
}

// update the cpu's internal timers
void CPU::updateTimers(int cycles)
{
	// check whether clock is enabled - decides whether we can update TIMA

	bool clockEnabled = getBit(Memory.readMemory(TAC), 2) ? true : false;

	// update divider register
	dividerRegister += cycles;
	if (dividerRegister >= 255) {
		dividerRegister = 0;
		Memory.incrementDivider();
	}

	uint8_t currClockFreq = getNewClockFreq();
	// check if tac has changed
	if (TACValue != currClockFreq)
	{
		timerCounter = currClockFreq;
		TACValue = currClockFreq;
	}

	if (clockEnabled)
	{
		timerCounter -= cycles;
		// if we have done enough cycles to increment 
		if (timerCounter <= 0)
		{
			
			timerCounter = getNewClockFreq();
			if (Memory.readMemory(TIMA) == 255) {
				Memory.writeMemory(TIMA, Memory.readMemory(TMA));
				setInterrupt(2);
			}
			else {
				Memory.writeMemory(TIMA, Memory.readMemory(TIMA) + 1);
			}
		}
	}
}

// get the frequency TIMA is currently incrementing at
int CPU::getNewClockFreq()
{
	uint8_t bits = Memory.readMemory(TAC) & 0b11;

	switch (bits)
	{
		case 0: return 1024;
		case 1: return 16;
		case 2: return 64;
		case 3: return 256;
	}
}

// set interupt to happen
void CPU::setInterrupt(int bit)
{
	uint8_t reg = Memory.readMemory(0xFF0F);
	reg = set_bit(reg, bit);
	Memory.writeInterruptState(reg);
}

/*
	Check if interrupts have been enabled and execute enabled interrupts
	in order
*/
void CPU::handleInterrupts()
{
	if (IME) {
		uint8_t enabledBits = Memory.readMemory(0xFFFF);
		uint8_t interruptFlags = Memory.readMemory(0xFF0F);

		// mean we have some interrupt
		if (interruptFlags > 0) {
			for (int i = 0; i < 5; i++) {
				if (getBit(i, interruptFlags) == 1) {
					if (getBit(i, enabledBits) == 1) {
						executeInterrupt(i);
					}
				}
			}

		}
	}
}

// execute interrupts which were enabled
void CPU::executeInterrupt(int bit)
{
	IME = false;
	uint8_t reg = Memory.readMemory(0xFF0F);
	reg = reset_bit(reg, bit);
	Memory.writeInterruptState(reg);
	writeToStack(pc);
	switch (bit)
	{
	case 0: pc = 0x40; break;
	case 1: pc = 0x48; break;
	case 2: pc = 0x50; break;
	case 4: pc = 0x60; break;
	}
}

// Set a single bit in register
int CPU::set_bit(int reg, int bit)
{
	return reg |= (1U << bit);
}

// reset a single bit in register
int CPU::reset_bit(uint8_t &reg, int bit)
{
	return reg &= ~(1U << bit);
}

// reset a single bit in flag AF
void CPU::bitreset(int flag) {
	this->registerAF.reg &= ~(1UL << flag);
}