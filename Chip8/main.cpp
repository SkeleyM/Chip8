#include <iostream>
#include "Chip8.h"

int main()
{

	Chip8 chip8;
	chip8.LoadGame("C:/Users/mason/Desktop/rngtest.ch8");

	chip8.StepExecute = true;

	chip8.InitialiseCpu();
	while (chip8.executing)
	{ 
		if (chip8.StepExecute) { std::cin.get(); }

		chip8.EmulateCycle();

		if (chip8.drawFlag)
		{
			chip8.OutputScreen();
		}	
		chip8.DebugLog();
	}
	

	return 0;
}