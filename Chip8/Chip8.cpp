#include "Chip8.h"
#include <vector>
#include <string>
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

}

void Chip8::InitialiseCpu()
{
	// set program counter to 0x200 as thats where programs start in memory
	this->pc = 0x200;
	this->opcode = 0x0000;
	this->I = 0x0000;
	this->sp = 0x0000;
	this->drawFlag = false;
	this->executing = true;

	for (int i = 0; i < 80; i++)
	{
		memory[i] = chipFontset[i];
	}
}

void Chip8::LoadGame(std::string File)
{
	// open File in binary mode
	std::ifstream file(File, std::ios::in);

	// Return and provide message if the file fails to open
	if (!file.is_open()) { std::cout << "Failed To Open ROM!"; return; }

	char byte;
	// copy from file to memory
	int index = 0;
	while (file >> byte && !file.eof())
	{
		this->memory[index + 0x200] = byte;
		index++;
	}

	file.close();
}

// emulate a cpu cycle using the Fetch Decode Execute cycle
void Chip8::EmulateCycle()
{
	// Fetch
	
	// Opcodes in CHIP8 are made from 2 bytes
	// bitwise shift by 8 bits to make space for the second instruction
	// bitwise OR the current and next byte in memory to merge and form an instruction
	this->opcode = memory[pc] << 8 | memory[pc + 1];

	std::cout << "PC: ";
	printf("%X", opcode);
	std::cout << std::endl;

	pc += 2;

	// Decode & Execute
	switch (this->opcode & 0xF000)
	{

		// SYS address
		case (0x0000):
		{
			switch (this->opcode & 0x000F)
			{
				// CLS
				case (0x0000):
				{
					break;
				}
				// RET
				case (0x000E):
				{
					this->pc = stack[sp];
					stack[sp] = NULL;
					sp--;
					break;
				}
			}
			break;
		}
		// JMP adress
		case (0x1000):
		{
			this->pc = this->opcode & 0x0FFF;
			std::cout << "JMP " << (this->opcode & 0x0FFF) << std::endl;
			break;
		}
		// CALL adress
		case (0x2000):
		{	
			this->stack[this->sp] = pc;
			this->pc = this->opcode & 0x0FFF;
			this->sp++;
			break;
		}
		// SE Vx, byte
		// skip next instruction if Vx == kk
		case (0x3000):
		{
			if ((v[this->opcode & 0x0F00]) == (this->opcode & 0x00FF))
			{
				// skip next instruction
				pc += 2;
			}
			break;
		}
		// SNE Vx, byte
		// skip next instruction if Vx != kk
		case (0x4000):
		{
			if ((v[this->opcode & 0x0F00]) != (this->opcode & 0x00FF))
			{
				// skip next instruction
				pc += 2;
			}
			break;
		}
		// SE Vx, Vy
		// skip next instruction if Vx == Vy
		case (0x5000):
		{
			if ((v[this->opcode & 0x0F00]) == (v[this->opcode & 0x00F0]))
			{
				// skip next instruction
				pc += 2;
			}
			break;
		}
		// LD vx, byte
		// Set Vx = kk, (0x6xkk)
		case (0x6000):
		{
			this->v[(this->opcode & 0x0F00)] = (this->opcode & 0x00FF);
			break;
		}
		// ADD Vx, byte
		case (0x7000):
		{
			v[this->opcode & 0x0F00] += this->opcode & 0x00FF;
			break;
		}
		// ADD Vy, byte
		case (0x8000):
		{
			switch (this->opcode & 0x000F)
			{
				// SET Vx, Vx | Vy
				case (0x0001):
				{
					char Vx = this->opcode & 0x0F00;
					char Vy = this->opcode & 0x00F0;
					v[Vx] = Vx | Vy;
					break;
				}
			}
			v[this->opcode & 0x0F00] += this->opcode & 0x00FF;
			break;
		}
		case (0x9000):
		{
			if (v[this->opcode & 0x0F00] != v[this->opcode & 0x00F0])
			{
				this->pc += 2;
			}
			break;
		}
		// SET I, nnn
		case (0xA000):
		{
			this->I = (this->opcode & 0x0FFF);
			break;
		}
		// SET RND Vx, AND kk
		case (0xC000):
		{
			srand(time(0));
			v[this->opcode & 0x0F00] = ((rand() + 1) % 255) & (this->opcode & 0x00FF);
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

			this->drawFlag = true;
			break;
		}
		case (0xE000):
		{
			switch (this->opcode * 0x000F)
			{
				case (0x000E):
				{
					if (key[this->opcode & 0x0F00]) { this->pc += 2; }
					break;
				}
				case (0x0001):
				{
					if (key[this->opcode & 0x0F00]) { this->pc += 2; }
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
					v[this->opcode & 0x0F00] = this->delayTimer;
					break;
				}
				// Wait for key press and store value of key in Vx
				case(0x000A):
				{
					char key = WaitForKey();
					v[this->opcode & 0x0F00] = key;
					break;
				}
				// LD DT, VX
				case(0x0015):
				{
					this->delayTimer = v[this->opcode & 0x0F00];
					break;
				}
				// LD ST, VX
				case(0x0018):
				{
					this->soundTimer = v[this->opcode & 0x0F00];
					break;
				}
				// ADD I, Vx
				case(0x001E):
				{
					this->I += v[this->opcode & 0x0F00];
					break;
				}
				// LD FONT, Vx
				case(0x0029):
				{
					this->I = this->v[((this->opcode & 0x0F00) * 5)];
					break;
				}
				// LD BCD, Vx
				case(0x0033):
				{
					this->memory[this->I] = this->v[(this->opcode & 0x0F00) >> 8] / 100;
					this->memory[this->I + 1] = (this->v[(this->opcode & 0x0F00) >> 8] / 10) % 10;
					this->memory[this->I + 2] = (this->v[(this->opcode & 0x0F00) >> 8] % 100) % 10;
					break;
				}
				// LD [i], Vx
				case(0x0055):
				{
					for (int index = 0; index < 16; index++)
					{
						this->memory[this->I + index] = this->v[index];
					}
					break;
				}
				// LD Vx, [i]
				case(0x0065):
				{
					for (int index = 0; index < 16; index++)
					{
						this->v[index] = this->memory[this->I + index];
					}
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
	
}

void Chip8::OutputScreen()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			// screen buffer is made of 1's and 0's so cast to bool for ease of use
			bool value = (int)screenBuffer[x + (y * 64)];
			if (value)
			{
				SetConsoleTextAttribute(hConsole, 0x00FF);
				std::cout << value;
			}
			else
			{
				SetConsoleTextAttribute(hConsole, 0x0000);
				std::cout << value;
			}
			
		}
		std::cout << std::endl;
	}
	SetConsoleTextAttribute(hConsole, 0x000F);
	this->drawFlag = false;
}