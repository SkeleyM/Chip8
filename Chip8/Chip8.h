#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <stack>

class Chip8
{
private:

	unsigned short opcode{ 0x0000 };

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

	unsigned char key[16]{ false };

	unsigned short I{ 0x0000 };

	unsigned short pc{ 0x200 };

	unsigned char screenBuffer[64 * 32]{ 0x0000 };
	unsigned char delayTimer{ 0 };
	time_t delayTickTimer{ 0 };
	unsigned char soundTimer{ 0 };

	unsigned short stack[16]{ 0x0200 };
	unsigned short sp{ 0 };

	time_t deltaTime{ 0 };

	const int KeyMap[16]{
		0x31, 0x32, 0x33, 0x34,
		0x51, 0x57, 0x45, 0x52,
		0x41, 0x53, 0x44, 0x46,
		0x5A, 0x58, 0x43, 0x56
	};
	

	void Buzzer();

public:
	// Debug
	void DebugLog(std::string str);
	void DebugLog();
	void OutputKeyStates();

	bool Debugging{ false };
	bool StepExecute{ false };

	int ScreenMagnifierX{ 2 };

	bool executing{ true };

	bool drawFlag;
	// Input
	char WaitForKey();
	void SetKeys();

	// Emulation
	void InitialiseCpu();
	void LoadGame(std::string File);
	void OP_00E0();
	void OP_00EE();
	void OP_1000(int nnn);
	void OP_2000(int nnn);
	void OP_3000(int x, int kk);
	void OP_4000(int x, int kk);
	void OP_5000(int x, int y);
	void OP_6000(int x, int kk);
	void OP_7000(int x, int kk);
	void OP_8000(int x, int y);
	void OP_8001(int x, int y);
	void OP_8002(int x, int y);
	void OP_8003(int x, int y);
	void OP_8004(int x, int y);
	void OP_8005(int x, int y);
	void OP_8006(int x);
	void OP_8007(int y, int x);
	void OP_800E(int x);
	void OP_9000(int x, int y);
	void OP_A000(int nnn);
	void OP_B000(int nnn);
	void OP_C000(int x, int kk);
	void OP_D000();
	void OP_E00E(int x);
	void OP_E001(int x);

	void OP_F000();
	void OP_F007(int x);
	void OP_F00A(int x);
	void OP_F015(int x);
	void OP_F018(int x);
	void OP_F01E(int x);
	void OP_F029(int x);
	void OP_F033(int x);
	void OP_F055();
	void OP_F065();

	void EmulateCycle();
	void OutputScreen();

};

void OP_00EE();

void OP_D000();
