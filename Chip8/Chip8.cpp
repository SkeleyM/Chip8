#include "Chip8.h"
#include <vector>
#include <string>
#include <sstream>	
#include <iostream>
#include <Windows.h>

void Chip8::DebugLog()
{
	int index = 0;
	for (char value : v)
	{
		std::cout << " V" << index << ": " << (int)value;
		index++;
	}
	std::cout << " I:";
	printf("%X", this->I);

	std::cout << std::endl;
}

void Chip8::DebugLog(std::string str)
{
	if (this->Debugging)
	{
		std::cout << str << std::endl;
	}
}

void Chip8::OutputKeyStates()
{
	if (!this->Debugging) { return; }

	int index = 0;
	for (char Key : key)
	{
		std::cout << "K" << index << ": " << Key << " ";
		index++;
	}
	std::cout << std::endl;
}

char Chip8::WaitForKey()
{
	std::vector<char> validKeys{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'a', 'b', 'c', 'd', 'e', 'f' };

	char key;
	while (true)
	{
		bool breakLoop{ false };

		std::cin >> key;

		for (int i = 0; i < validKeys.size(); i++)
		{
			if (key == validKeys[i])
			{
				breakLoop = true;
				break;
			}
		}
		if (breakLoop) { break; }
	}

	switch (key)
	{
	case '1':
	{
		return 0x1;
	}
	case '2':
	{
		return 0x2;
	}
	case '3':
	{
		return 0x3;
	}
	case '4':
	{
		return 0x4;
	}
	case '5':
	{
		return 0x5;
	}
	case '6':
	{
		return 0x6;
	}
	case '7':
	{
		return 0x7;
	}
	case '8':
	{
		return 0x8;
	}
	case '9':
	{
		return 0x9;
	}
	case '0':
	{
		return 0x0;
	}
	case 'a':
	{
		return 0xa;
	}
	case 'b':
	{
		return 0xb;
	}
	case 'c':
	{
		return 0xc;
	}
	case 'd':
	{
		return 0xd;
	}
	case 'e':
	{
		return 0xe;
	}
	case 'f':
	{
		return 0xF;
	}
	}

}

void Chip8::SetKeys()
{
	// For Reference i do not care this is bad i cant figure out GetKeyboardState()

	// 1 2 3 C
	if (GetKeyState(0x31) < 0) { this->key[1] = true; }
	else { this->key[1] = false; }
	if (GetKeyState(0x32) < 0) { this->key[2] = true; }
	else { this->key[2] = false; }
	if (GetKeyState(0x33) < 0) { this->key[3] = true; }
	else { this->key[3] = false; }
	if (GetKeyState(0x34) < 0) { this->key[0xC] = true; }
	else { this->key[0xC] = false; }


	// 4 5 6 D
	if (GetKeyState(0x51) < 0) { this->key[4] = true; }
	else { this->key[4] = false; }
	if (GetKeyState(0x57) < 0) { this->key[5] = true; }
	else { this->key[5] = false; }
	if (GetKeyState(0x45) < 0) { this->key[6] = true; }
	else { this->key[6] = false; }
	if (GetKeyState(0x52) < 0) { this->key[0xD] = true; }
	else { this->key[0xD] = false; }

	// 7 8 9 E
	if (GetKeyState(0x41) < 0) { this->key[7] = true; }
	else { this->key[7] = false; }
	if (GetKeyState(0x53) < 0) { this->key[8] = true; }
	else { this->key[8] = false; }
	if (GetKeyState(0x44) < 0) { this->key[9] = true; }
	else { this->key[9] = false; }
	if (GetKeyState(0x46) < 0) { this->key[0xE] = true; }
	else { this->key[0xE] = false; }

	// A 0 B F
	if (GetKeyState(0x5A) < 0) { this->key[0xA] = true; }
	else { this->key[0xA] = false; }
	if (GetKeyState(0x58) < 0) { this->key[0] = true; }
	else { this->key[0] = false; }
	if (GetKeyState(0x43) < 0) { this->key[0xB] = true; }
	else { this->key[0xB] = false; }
	if (GetKeyState(0x56) < 0) { this->key[0xF] = true; }
	else { this->key[0xF] = false; }
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

// emulate a cpu cycle using the Fetch Decode Execute cycle
void Chip8::EmulateCycle()
{
	// Fetch

	// Opcodes in CHIP8 are made from 2 bytes
	// bitwise shift by 8 bits to make space for the second instruction
	// bitwise OR the current and next byte in memory to merge and form an instruction
	this->opcode = ((memory[pc] << 8) | memory[pc + 1]);

	int dtStart = time(0);

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
				pc += 2;

				// clear screenbuffer
				std::fill_n(this->screenBuffer, 32 * 64, 0);

				// clear console
				system("cls");

				break;
			}
			// RET
			case (0x00EE):
			{
				sp--;
				this->pc = stack[sp];
				stack[sp] = 0x0200;
				pc += 2;
				break;
			}
		}
		break;
	}
	// JMP adress
	case (0x1000):
	{
		this->pc = nnn;
		break;
	}
	// CALL adress
	case (0x2000):
	{
		this->stack[this->sp] = this->pc;
		this->sp++;
		this->pc = nnn;
		break;
	}
	// SE Vx, byte
	// skip next instruction if Vx == kk
	case (0x3000):
	{
		if ((v[x]) == (kk))
		{
			// skip next instruction
			pc += 4;
		}
		else { pc += 2; }
		break;
	}
	// SNE Vx, byte
	// skip next instruction if Vx != kk
	case (0x4000):
	{
		if (v[x] != kk)
		{
			// skip next instruction
			pc += 4;
		}
		else { pc += 2; }
		break;
	}
	// SE Vx, Vy
	// skip next instruction if Vx == Vy
	case (0x5000):
	{
		if ((v[x]) == (v[y]))
		{
			// skip next instruction
			pc += 4;
		}
		else { pc += 2; }
		break;
	}
	// LD vx, byte
	// Set Vx = kk, (0x6xkk)
	case (0x6000):
	{
		this->v[x] = (kk);
		pc += 2;
		break;
	}
	// ADD Vx, byte
	case (0x7000):
	{
		v[x] += kk;
		pc += 2;
		break;
	}
	
	case (0x8000):
	{
		switch (this->opcode & 0x000F)
		{
			// LD Vx, Vy
			case (0x0000):
			{
				this->v[x] = this->v[y];
				this->pc += 2;
				break;
			}
			// SET OR Vx, Vy
			case (0x0001):
			{
				char Vx = x;
				char Vy = y;
				v[Vx] = Vx | Vy;
				pc += 2;
				break;
			}
			// AND Vx, Vy 
			case (0x0002):
			{
				this->v[x] = (v[x] & v[y]);
				this->pc += 2;
				break;
			}
			// XOR Vx, Vy
			case (0x0003):
			{
				this->v[x] = (v[x] ^ v[y]);
				this->pc += 2;
				break;
			}
			// ADD Vx, Vy
			case (0x0004):
			{
				this->v[x] = (v[x] + v[y]);
				if (v[x] > 0xFF) { v[0xF] = 1; v[x] = 0xFF; }
				else
				{
					v[0xF] = 0;
				}
				this->pc += 2;
				break;
			}
			// SUB Vx, Vy
			case (0x0005):
			{
				(v[x] > v[y]) ? v[0xF] = 1 : v[0xF] = 0;
				v[x] -= v[y];

				this->pc += 2;
				break;
			}
			// SHR Vx {, Vy}
			case (0x0006):
			{
				(v[x] & 0b001) ? v[0xF] = 1 : v[0xF] = 0;
				v[x] /= 2;

				this->pc += 2;
				break;
			}
			// SUBN Vx, Vy
			case (0x0007):
			{
				(v[y] > v[x]) ? v[0xF] = 1 : v[0xF] = 0;
				v[x] = v[y] - v[x];

				this->pc += 2;
				break;
			}
			// SHL Vx {, Vy}
			case (0x000E):
			{
				(v[x] & 0b0001) ? v[0xF] = 1 : v[0xF] = 0;
				v[x] *= 2;

				this->pc += 2;
				break;
			}
		}
			break;
	}
	// SNE Vx, Vy
	case (0x9000):
	{
		if (v[x] != v[y])
		{
			this->pc += 4;
		}
		else {
			pc += 2;
		}
		break;
	}
	// SET I, nnn
	case (0xA000):
	{
		this->I = (nnn);
		pc += 2;
		break;
	}
	// JP v0, addr
	// jump to nnn + v[0]
	case (0xB000):
	{
		this->pc = (nnn + v[0]);
		break;
	}
	// SET RND Vx, AND kk
	case (0xC000):
	{
		srand(time(0));
		v[x] = std::abs((((rand()) % 255) + 1) & (kk));
		pc += 2;
		break;
	}
	// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
	case (0xD000):
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

		break;
	}
	case (0xE000):
	{
		switch (this->opcode * 0x000F)
		{
		case (0x000E):
		{
			if (key[x]) { this->pc += 4; }
			else { this->pc += 2; }
			break;
		}
		case (0x0001):
		{
			std::cout << key[x];
			if (!key[x]) { this->pc += 4; }
			else { this->pc += 2; }
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
			this->executing = false;
			std::cout << "Returning to 'CHIP-OS'";
			break;
		}
		// LOD Vx, DT
		case(0x0007):
		{
			v[x] = this->delayTimer;
			pc += 2;
			break;
		}
		// Wait for key press and store value of key in Vx
		case(0x000A):
		{
			char key = WaitForKey();
			v[x] = key;
			pc += 2;
			break;
		}
		// LD DT, VX
		case(0x0015):
		{
			this->delayTimer = v[x];
			pc += 2;
			break;
		}
		// LD ST, VX
		case(0x0018):
		{
			this->soundTimer = v[x];
			pc += 2;
			break;
		}
		// ADD I, Vx
		case(0x001E):
		{
			this->I += v[x];
			pc += 2;
			break;
		}
		// LD FONT, Vx
		case(0x0029):
		{
			this->I = this->v[x] * 5;
			pc += 2;
			break;
		}
		// LD BCD, Vx
		case(0x0033):
		{
			this->memory[this->I] = this->v[(x)] / 100;
			this->memory[this->I + 1] = (this->v[(x)] / 10) % 10;
			this->memory[this->I + 2] = (this->v[(x)] % 100) % 10;
			pc += 2;
			break;
		}
		// LD [i], Vx
		case(0x0055):
		{
			for (int index = 0; index < 16; index++)
			{
				this->memory[this->I + index] = this->v[index];
			}
			pc += 2;
			break;
		}
		// LD Vx, [i]
		case(0x0065):
		{
			for (int index = 0; index < 16; index++)
			{
				this->v[index] = this->memory[this->I + index];
			}
			pc += 2;
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

	this->deltaTime = time(0) - dtStart;

	this->delayTickTimer += deltaTime;
	if (this->delayTickTimer >= 16.67)
	{
		delayTimer != 0 ? delayTimer -= 1 : NULL;
	}
}

void Chip8::OutputScreen()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	std::stringstream OutputBlock;
	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			bool pixel = (int)this->screenBuffer[x + (y * 64)];
			OutputBlock << pixel;

			// determine weather to output block or not
			if ((bool)pixel != this->screenBuffer[x + (y * 64)] + 1)
			{
				if (pixel) { SetConsoleTextAttribute(hConsole, 0xFF); }
				else { SetConsoleTextAttribute(hConsole, 0x00); }

				std::cout << OutputBlock.str();
				OutputBlock.str("");
			}
		}

		std::cout << OutputBlock.str();
		OutputBlock.str("");

		std::cout << std::endl;
	}

	std::cout << std::flush;

	SetConsoleTextAttribute(hConsole, 0x000F);
	this->drawFlag = false;
}