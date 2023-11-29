#include "Chip8.h"
#include <vector>
#include <string>
#include <sstream>	
#include <iostream>
#include <Windows.h>
#include <chrono>

// just no?? what was i on making this
char Chip8::WaitForKey()
{
	while (1)
	{
		for (char key : this->KeyMap)
		{
			short KeyState = GetKeyState(VkKeyScanA(key));
			if (KeyState < 0) 
			{
				return key;
			}
		}	
	}
}

void Chip8::SetKeys()
{

	for (int i=0; i < 16; i++)
	{
		// the upper bit represents the key press state
		if (GetKeyState(this->KeyMap[i]) & 0x8000) 
		{ 
			this->key[i] = true; 
		}
		else 
		{ 
			this->key[i] = false; 
		}
	}
}

void Chip8::InitialiseCpu()
{
	// set program counter to 0x200 as thats where programs start in memory
	this->pc = 0x200;
	this->opcode = 0x0000;
	this->I = 0x0000;
	this->sp = 0x0000;
	this->drawFlag = false;
	

	for (int i = 0; i < 80; i++)
	{
		memory[i] = chipFontset[i];
	}
}

void Chip8::LoadGame(std::string File)
{
	// open File in binary mode
	std::ifstream file(File, std::ios::binary);

	// Return and provide message if the file fails to open
	if (!file.is_open()) { std::cout << "Failed To Open ROM!"; this->executing = false; return; }

	char byte{ 0x0000 };
	// copy from file to memory
	for (int i = 0x200; file.get(byte); i++)
	{
		this->memory[i] = byte;
	}
}

void Chip8::OP_00E0()
{
	this->pc += 2;

	// clear screenbuffer
	std::fill_n(this->screenBuffer, 32 * 64, 0);

	// clear console
	system("cls");
}

void Chip8::OP_00EE()
{
	this->sp--;
	this->pc = this->stack[sp];
	this->stack[sp] = 0x0200;
	this->pc += 2;
}

void Chip8::OP_1000(int nnn)
{
	this->pc = nnn;
}

void Chip8::OP_2000(int nnn)
{
	this->stack[this->sp] = this->pc;
	this->sp++;
	this->pc = nnn;
}

void Chip8::OP_3000(int x, int kk)
{
	if ((v[x]) == (kk))
	{
		// skip next instruction
		pc += 4;
	}
	else { pc += 2; }
}

void Chip8::OP_4000(int x, int kk)
{
	if (v[x] != kk)
	{
		// skip next instruction
		pc += 4;
	}
	else { pc += 2; }
}

void Chip8::OP_5000(int x, int y)
{
	if ((v[x]) == (v[y]))
	{
		// skip next instruction
		pc += 4;
	}
	else { pc += 2; }
}

void Chip8::OP_6000(int x, int kk)
{
	this->v[x] = (kk);
	pc += 2;
}

void Chip8::OP_7000(int x, int kk)
{
	v[x] += kk;
	pc += 2;
}

void Chip8::OP_8000(int x, int y)
{
	this->v[x] = this->v[y];
	this->pc += 2;
}

void Chip8::OP_8001(int x, int y)
{
	char Vx = x;
	char Vy = y;
	v[Vx] = Vx | Vy;
	pc += 2;
}

void Chip8::OP_8002(int x, int y)
{
	this->v[x] = (v[x] & v[y]);
	this->pc += 2;
}

void Chip8::OP_8003(int x, int y)
{
	this->v[x] = (v[x] ^ v[y]);
	this->pc += 2;
}

void Chip8::OP_8004(int x, int y)
{
	this->v[x] = (v[x] + v[y]);
	if (v[x] > 0xFF) { v[0xF] = 1; v[x] = 0xFF; }
	else
	{
		v[0xF] = 0;
	}
	this->pc += 2;
}

void Chip8::OP_8005(int x, int y)
{
	(v[x] > v[y]) ? v[0xF] = 1 : v[0xF] = 0;
	v[x] -= v[y];

	this->pc += 2;
}

void Chip8::OP_8006(int x)
{
	(v[x] & 0b001) ? v[0xF] = 1 : v[0xF] = 0;
	v[x] /= 2;

	this->pc += 2;
}

void Chip8::OP_8007(int y, int x)
{
	(v[y] > v[x]) ? v[0xF] = 1 : v[0xF] = 0;
	v[x] = v[y] - v[x];

	this->pc += 2;
}

void Chip8::OP_800E(int x)
{
	(v[x] & 0b0001) ? v[0xF] = 1 : v[0xF] = 0;
	v[x] *= 2;

	this->pc += 2;
}

void Chip8::OP_9000(int x, int y)
{
	if (v[x] != v[y])
	{
		this->pc += 4;
	}
	else {
		pc += 2;
	}
}

void Chip8::OP_A000(int nnn)
{
	this->I = (nnn);
	pc += 2;
}

void Chip8::OP_B000(int nnn)
{
	this->pc = (nnn + v[0]);
}

void Chip8::OP_C000(int x, int kk)
{
	srand(time(0));
	v[x] = std::abs((((rand()) % 255) + 1) & (kk));
	pc += 2;
}

void Chip8::OP_D000()
{
	unsigned short x = v[(opcode & 0x0F00) >> 8];
	unsigned short y = v[(opcode & 0x00F0) >> 4];
	unsigned short height = (opcode & 0x000F);
	unsigned short pixel;

	v[0xF] = 0;
	for (int col = 0; col < height; col++)
	{
		pixel = memory[I + col];
		for (int row = 0; row < 8; row++)
		{
			if ((pixel & (0x80 >> row)) != 0)
			{
				if (screenBuffer[x + row + ((y + col) * 64)] == 1)
				{
					v[0xF] = 1;
				}
				screenBuffer[x + row + ((y + col) * 64)] ^= 1;
			}
		}
	}
	pc += 2;
	this->drawFlag = true;

	// clear console output
	system("cls");
}

void Chip8::OP_E00E(int x)
{
	if (key[x - 2]) { this->pc += 4; }
	else { this->pc += 2; }
}

void Chip8::OP_E001(int x)
{
	if (!key[x - 2]) { this->pc += 4; }
	else { this->pc += 2; }
}

void Chip8::OP_F000()
{
	this->executing = false;
}

void Chip8::OP_F007(int x)
{
	v[x] = this->delayTimer;
	pc += 2;
}

void Chip8::OP_F00A(int x)
{
	char key = WaitForKey();
	v[x] = key;
	pc += 2;
}

void Chip8::OP_F015(int x)
{
	this->delayTimer = v[x];
	pc += 2;
}

void Chip8::OP_F018(int x)
{
	this->soundTimer = v[x];
	pc += 2;
}

void Chip8::OP_F01E(int x)
{
	this->I += v[x];
	pc += 2;
}

void Chip8::OP_F029(int x)
{
	this->I = this->v[x] * 5;
	pc += 2;
}

void Chip8::OP_F033(int x)
{
	this->memory[this->I] = this->v[(x)] / 100;
	this->memory[this->I + 1] = (this->v[(x)] / 10) % 10;
	this->memory[this->I + 2] = (this->v[(x)] % 100) % 10;
	pc += 2;
}

void Chip8::OP_F055()
{
	for (int index = 0; index < 16; index++)
	{
		this->memory[this->I + index] = this->v[index];
	}
	pc += 2;
}

void Chip8::OP_F065()
{
	for (int index = 0; index < 16; index++)
	{
		this->v[index] = this->memory[this->I + index];
	}
	pc += 2;
}

// emulate a cpu cycle using the Fetch Decode Execute cycle
void Chip8::EmulateCycle()
{
	// Fetch

	// Opcodes in CHIP8 are made from 2 bytes
	// bitwise shift by 8 bits to make space for the second instruction
	// bitwise OR the current and next byte in memory to merge and form an instruction
	this->opcode = ((memory[pc] << 8) | memory[pc + 1]);

	time_t dtStart = std::chrono::system_clock::now().time_since_epoch().count();

	if (this->Debugging)
	{
		std::cout << "PC: ";
		printf("%X", opcode);
		std::cout << " " << this->pc << " (" << (this->pc - 512) << ")";
		std::cout << std::endl;
	}


	int nnn = (this->opcode & 0x0FFF);
	int n = (this->opcode & 0x000F);
	int x = ((this->opcode & 0x0F00) >> 8);
	int y = ((this->opcode & 0x00F0) >> 4);
	int kk = (this->opcode & 0x00FF);

	// Decode & Execute
	switch (this->opcode & 0xF000)
	{
		// SYS address
	case (0x0000):
	{
		switch (this->opcode & 0x00FF)
		{
				// CLS
			case (0x00E0):
			{
				OP_00E0();
				break;
			}
			// RET
			case (0x00EE):
			{
				OP_00EE();
				break;
			}
		}
		break;
	}
	// JMP adress
	case (0x1000):
	{
		OP_1000(nnn);
		break;
	}
	// CALL adress
	case (0x2000):
	{
		OP_2000(nnn);
		break;
	}
	// SE Vx, byte
	// skip next instruction if Vx == kk
	case (0x3000):
	{
		OP_3000(x, kk);
		break;
	}
	// SNE Vx, byte
	// skip next instruction if Vx != kk
	case (0x4000):
	{
		OP_4000(x, kk);
		break;
	}
	// SE Vx, Vy
	// skip next instruction if Vx == Vy
	case (0x5000):
	{
		OP_5000(x, y);
		break;
	}
	// LD vx, byte
	// Set Vx = kk, (0x6xkk)
	case (0x6000):
	{
		OP_6000(x, kk);
		break;
	}
	// ADD Vx, byte
	case (0x7000):
	{
		OP_7000(x, kk);
		break;
	}
	
	case (0x8000):
	{
		switch (this->opcode & 0x000F)
		{
			// LD Vx, Vy
			case (0x0000):
			{
				OP_8000(x, y);
				break;
			}
			// SET OR Vx, Vy
			case (0x0001):
			{
				OP_8001(x, y);
				break;
			}
			// AND Vx, Vy 
			case (0x0002):
			{
				OP_8002(x, y);
				break;
			}
			// XOR Vx, Vy
			case (0x0003):
			{
				OP_8003(x, y);
				break;
			}
			// ADD Vx, Vy
			case (0x0004):
			{
				OP_8004(x, y);
				break;
			}
			// SUB Vx, Vy
			case (0x0005):
			{
				OP_8005(x, y);
				break;
			}
			// SHR Vx {, Vy}
			case (0x0006):
			{
				OP_8006(x);
				break;
			}
			// SUBN Vx, Vy
			case (0x0007):
			{
				OP_8007(y, x);
				break;
			}
			// SHL Vx {, Vy}
			case (0x000E):
			{
				OP_800E(x);
				break;
			}
		}
			break;
	}
	// SNE Vx, Vy
	case (0x9000):
	{
		OP_9000(x, y);
		break;
	}
	// SET I, nnn
	case (0xA000):
	{
		OP_A000(nnn);
		break;
	}
	// JP v0, addr
	// jump to nnn + v[0]
	case (0xB000):
	{
		OP_B000(nnn);
		break;
	}
	// SET RND Vx, AND kk
	case (0xC000):
	{
		OP_C000(x, kk);
		break;
	}
	// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
	case (0xD000):
	{
		OP_D000();

		break;
	}
	case (0xE000):
	{
		switch (this->opcode & 0x000F)
		{
			case (0x000E):
			{
				OP_E00E(x);
				break;
			}
			case (0x0001):
			{
				OP_E001(x);
				break;
			}
		}
		break;
	}
	case (0xF000):
	{
		switch (this->opcode & 0x00FF)
		{
			// Stop operation
		case(0x0000):
		{
			OP_F000();
			break;
		}
		// LOD Vx, DT
		case(0x0007):
		{
			OP_F007(x);
			break;
		}
		// Wait for key press and store value of key in Vx
		case(0x000A):
		{
			OP_F00A(x);
			break;
		}
		// LD DT, VX
		case(0x0015):
		{
			OP_F015(x);
			break;
		}
		// LD ST, VX
		case(0x0018):
		{
			OP_F018(x);
			break;
		}
		// ADD I, Vx
		case(0x001E):
		{
			OP_F01E(x);
			break;
		}
		// LD FONT, Vx
		case(0x0029):
		{
			OP_F029(x);
			break;
		}
		// LD BCD, Vx
		case(0x0033):
		{
			OP_F033(x);
			break;
		}
		// LD [i], Vx
		case(0x0055):
		{
			OP_F055();
			break;
		}
		// LD Vx, [i]
		case(0x0065):
		{
			OP_F065();
			break;
		}
		}
		break;
	}
		// If opcode is not implimented then stop executing
		default:
		{
			std::cout << std::endl << "Opcode Not Implimented: ";
			printf("%X", this->opcode);
			std::cout << std::endl << "First Opcode Nibble: ";
			printf("%X", this->opcode & 0xF000);
			std::cout << std::endl << "Next Instruction: ";
			printf("%X", memory[pc]); printf("%X", memory[pc + 1]);


			this->executing = false;
			break;
		}
	}

	// take away the current epoch in ms from the start
	this->deltaTime = std::chrono::system_clock::now().time_since_epoch().count() - dtStart;

	// add deltatime to the tick timer
	this->delayTickTimer += deltaTime;
	// if its == 16.67 (60HZ) then decrement timers
	if (this->delayTickTimer >= 16.67)
	{
		this->delayTimer != 0 ? this->delayTimer -= 1 : NULL;
		this->soundTimer != 0 ? this->soundTimer -= 1  : NULL;

		if (this->soundTimer > 0)
		{
			Buzzer();
		}

		this->delayTickTimer = 0;
	}
}



void Chip8::OutputScreen()
{	
	std::stringstream Output;
	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			for (int xMult = 0; xMult < this->ScreenMagnifierX; xMult++)
			{
				bool pixel = (int)this->screenBuffer[x + (y * 64)];
				if (pixel)
				{
					Output << (char)219;
				}
				else
				{
					Output << " ";
				}
			}
		}
		Output << std::endl;
	}

	std::cout << Output.str();

	this->drawFlag = false;
}

void Chip8::Buzzer()
{
	
}
