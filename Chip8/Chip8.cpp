#include "Chip8.h"
#include <vector>
#include <iostream>
#include <Windows.h>

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
	this->executing = true;

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

	printf("%X", opcode);
	std::cout << std::endl;

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
					break;
				}
				// RET
				case (0x00EE):
				{
					this->pc = this->stack.top();
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
			
			this->stack.push(this->pc);
			this->pc = this->opcode & 0x0FFF;
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
			if ((v[this->opcode & 0x0F00]) == v[this->opcode & 0x00F0])
			{
				// skip next instruction
				pc += 2;
			}
			break;
		}
		// LD vx, byte
		// Set Vx = kk, (0x6xkkg)
		case (0x6000):
		{
			this->v[this->opcode & 0x0F00] = this->opcode & 0x00FF;
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
			this->I = this->opcode & 0x0FFF;
			break;
		}
		// SET RND Vx, AND kk
		case (0xC000):
		{
			srand(time(0));
			v[this->opcode & 0x0F00] = ((rand() + 1) % 255) & this->opcode & 0x00FF;
			break;
		}
		// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
		case (0xD000):
		{
			unsigned short x = v[(opcode & 0x0F00) >> 8];
			unsigned short y = v[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;

			v[0xF] = 0;
			for (int yline = 0; yline < height; yline++)
			{
				pixel = memory[I + yline];
				for (int xline = 0; xline < 8; xline++)
				{
					if ((pixel & (0x80 >> xline)) != 0)
					{
						if (screenBuffer[(x + xline + ((y + yline) * 64))] == 1)
						{
							v[0xF] = 1;
							screenBuffer[(x + xline + ((y + yline) * 64))] ^= 1;
						}
						
					}
				}
			}
		
			this->drawFlag = true;
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
					// Not Implimented until Input
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
	pc += 2;
}

void Chip8::OutputScreen()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (screenBuffer[y * x] == 1)
			{
				SetConsoleTextAttribute(hConsole, 0x00FF);
				std::cout << screenBuffer[y * x];
			}
			if (screenBuffer[y * x] == 0)
			{
				SetConsoleTextAttribute(hConsole, 0x0000);
				std::cout << screenBuffer[y * x];
			}
		}
		std::cout << std::endl;
	}
	SetConsoleTextAttribute(hConsole, 0x000F);
	this->drawFlag = false;
}