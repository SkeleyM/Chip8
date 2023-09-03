#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <stack>

class Chip8
{
private:

	unsigned short opcode{ 0x0000};

	unsigned char memory[4096]{ 0x0000 };
	unsigned char chipFontset[80] =
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	unsigned char v[16]{ 0x0000 };

	unsigned char key[16]{ 0x0000 };

	unsigned short I;

	unsigned short pc;

	unsigned char screenBuffer[64 * 32]{ 0x0000 };
	unsigned char delayTimer;
	unsigned char soundTimer;

	unsigned short stack[16]{ 0x0000 };
	unsigned short sp;

	
	void DebugLog(std::string str);

public:
	void DebugLog();

	bool Debugging{ false };
	bool StepExecute{ false };

	int ScreenMagnifierX{ 2 };
	int ScreenMagnifierY{ 2 };

	bool executing;

	bool drawFlag;
	// Input
	char WaitForKey();
	void SetKeys();

	// Emulation
	void InitialiseCpu();
	void LoadGame(std::string File);
	void EmulateCycle();
	void OutputScreen();
};

