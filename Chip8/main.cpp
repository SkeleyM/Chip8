#include <iostream>
#include "Chip8.h"

int main()
{

	Chip8 chip8;
	chip8.LoadGame("C:/Users/mason/Desktop/test_opcode.ch8");

	chip8.InitialiseCpu();
	while (chip8.executing)
	{
		chip8.EmulateCycle();
	}
	

	return 0;
}