#include <iostream>
#include <Windows.h>
#include "Chip8.h"

int main()
{

	Chip8 chip8;
	chip8.LoadGame("C:/Users/mason/Desktop/rngtest.ch8");


	chip8.InitialiseCpu();
	while (chip8.executing)
	{ 
		if (chip8.StepExecute) { std::cin.get(); }

		chip8.EmulateCycle();

		if (chip8.drawFlag)
		{
			chip8.OutputScreen();
		}	
		Sleep(2);
	}
	

	return 0;
}