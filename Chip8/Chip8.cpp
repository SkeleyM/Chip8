#include "Chip8.h"
#include <vector>
#include <iostream>

void Chip8::InitialiseCpu()
{
	// set program counter to 0x200 as thats where programs start in memory
	this->pc = 0x200;
	this->opcode = 0x0000;
	this->I = 0x0000;
	this->sp = 0x0000;

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
		// CLS
		case (0x00E0):
		{
			break;
		}
		// RET
		case (0x00EE):
		{
			break;
		}
		// SYS address
		case (0x0000):
		{
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
		// SET I, nnn
		case (0xA000):
		{
			this->I = this->opcode & 0x0FFF;
			break;
		}
		// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
		case (0xD000):
		{
			std::cout << "DrawOp (not implemented)" << std::endl;
			break;
		}
		// SET RND Vx, AND kk
		case (0xC000):
		{
			srand(time(0));
			v[this->opcode & 0x0F00] = ((rand() + 1) % 255) & this->opcode & 0x00FF;
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
