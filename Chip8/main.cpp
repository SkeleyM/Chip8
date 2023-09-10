#include <iostream>
#include <Windows.h>
#include "Chip8.h"

int main(int argc, char** argv)
{
	//if (argc == 1) { std::cout << "Provide A ROM Path!"; return 0; }

	Chip8 chip8;
	//chip8.LoadGame(argv[1]);
	chip8.LoadGame("C:/Users/mason/Desktop/opcode.ch8");


	chip8.Debugging = true;

	chip8.InitialiseCpu();
	while (chip8.executing)
	{ 
		chip8.SetKeys();

		if (GetKeyState(VK_LEFT) < 0) { chip8.StepExecute = true; }
		else { chip8.StepExecute = false;  }
		if (chip8.StepExecute) { std::cin.get(); }

		chip8.EmulateCycle();

		
		if (chip8.drawFlag)
		{
			chip8.OutputScreen();
		}	
		
		chip8.DebugLog();
		chip8.OutputKeyStates();
	}
	

	return 0;
}