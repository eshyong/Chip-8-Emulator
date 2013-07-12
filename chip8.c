#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"

// font set for rendering
const unsigned char fontset[FONTSET_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
	0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};

/*	SUMMARY:
*	Initializes the chip and loads program into memory.
*	PARAMETERS:
*	chip: pointer to the chip to emulate
*	RETURN VALUE: none
*/
void initialize(chip8 *chip) {
	// set program counter to 0x200, where program starts
	chip->pctr = 0x200;
	chip->I = 0;
	chip->sptr = 0;

	// clear system memory, stack, display, and registers
	memset(chip->memory, 0, SYSTEM_MEMORY);
	memset(chip->stack, 0, STACK_SIZE);
	memset(chip->display, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
	memset(chip->V, 0, NUM_REGISTERS);

	// load chip8 fontset into memory 
	int i;
	for (i = 0; i < FONTSET_SIZE; i++) {
		chip->memory[i] = fontset[i];
	}

	// set timers to 0
	chip->delay_timer = 0;
	chip->sound_timer = 0;

	// set key to 0, or NULL
	chip->key = 0;

	// allow user to pick game
	char *file_name;
	char c;
	int loop = 1;

	printf("Please select a game from the list (enter a number).\n1.TETRIS\n2.PONG\n3.EXIT\n");
	scanf("%c", &c);

	while (loop) {
		switch (c) {
			case '1':
				file_name = "TETRIS.bin";
				loop = 0;
				break;
			case '2':
				file_name = "PONG.bin";
				loop = 0;
				break;
			case '3':
				printf("Exiting emulator...\n");
				exit(0);
			default:
				break;
		}
	}

	// load program into memory
	if (load_file(file_name, chip->memory + 512) < 0) { exit(0); }
}

/*	SUMMARY:
*	Loads program into chip memory.
*	PARAMETERS:
*	file_name: name of file to be loaded
*	buffer: system memory which will hold program
*	RETURN VALUE: 0 if successful, -1 if file error
*/
int load_file(char *file_name, unsigned char *buffer) {
	FILE *file;
	int file_size;

	// open file stream in binary read-only mode
	file = fopen(file_name, "rb");
	if (file == NULL) { printf("File not found.\n"); return -1; }
	else {
		// find file size and load program
		fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		printf("ROM size: %d\n", file_size);
		rewind(file);

		// return error if file is too large or cannot be found
		if (file_size > MAX_FILE_SIZE) { printf("File is too large to load.\n"); return -1; }

		// read program into memory
		fread(buffer, 1, file_size, file);
		return 0;
	}
}

/*	SUMMARY:
*	Mutator function, handles key presses from the main GLUT loop.
*	PARAMETERS:
*	chip: chip to handle keys
*	key: char representation of key passed from main loop.
*	RETURN VALUE: none
*/
void handle_input(chip8 *chip, unsigned char key) {
	chip->key = key;
}

/*	SUMMARY:
*	Emulation loop. Fetches opcodes from program in memory, then decodes and executes them.
*	PARAMETERS:
*	chip: chip to be emulated
*	RETURN VALUE: none
*/
void emulate_cycle(chip8 *chip) {
	// count down delay timer before executing opcode
	if (chip->delay_timer > 0) {
		--(chip->delay_timer);
	}

	// TODO: add sounds
	if (chip->sound_timer > 0) {
		--(chip->sound_timer);
	}

	// fetch opcode
	bool jump = FALSE;
	int i, j;
	unsigned short opcode, jump_addr;
	unsigned char X, Y, NN;
	opcode = chip->memory[chip->pctr] << 8 | chip->memory[chip->pctr + 1];
	jump_addr = opcode & 0x0FFF;
	X = (opcode & 0x0F00) >> 8;
	Y = (opcode & 0x00F0) >> 4;
	NN = opcode & 0x00FF;

	// decode and execute opcode in giant opcode switch statement
	switch (opcode & 0xF000) {
		case 0x0000:
			if (opcode == 0x00E0) {
				// clear graphics display
				memset(chip->display, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
			}
			else if (opcode == 0x00EE) {
				// return from subroutine
				if (chip->stack[chip->sptr] == 0) {
					printf("Invalid return from subroutine.\n");
				}
				chip->pctr = chip->stack[chip->sptr];
				chip->stack[chip->sptr] = 0;
				if (chip->sptr > 0) {
					--(chip->sptr);
				}
			} else {
				printf("Unknown opcode: 0x%x\n", opcode);
			}
			break;
		case 0x1000:
			// jump to address NNN
			chip->pctr = jump_addr;
			jump = TRUE;
			break;
		case 0x2000:
			// call subroutine at address NNN
			if (chip->stack[chip->sptr] != 0) {
				++(chip->sptr);
			}
			chip->stack[chip->sptr] = chip->pctr;
			chip->pctr = jump_addr;
			jump = TRUE;
			break;
		case 0x3000:
			// skip next instruction if VX equals NN
			if (chip->V[X] == NN) { chip->pctr += 2; }
			break;
		case 0x4000:
			// skip next instruction if VX not equals NN
			if (chip->V[X] != NN) { chip->pctr += 2; }
			break;
		case 0x5000:
			// skip next instruction if VX equals VY
			if (chip->V[X] == chip->V[Y]) { chip->pctr += 2; }
			break;
		case 0x6000:
			// set VX to NN
			chip->V[X] = NN;
			break;
		case 0x7000:
			// add NN to VX
			chip->V[X] += NN;
			break;
		case 0x8000:
			// VX and VY operations
			switch (opcode & 0x000F) {
				case 0x0000:
					// set VX to value of VY
					chip->V[X] = chip->V[Y];
					break;
				case 0x0001:
					// set VX to value of VX OR VY
					chip->V[X] = chip->V[X] | chip->V[Y];
					break;
				case 0x0002:
					// set VX to value of VX AND VY
					chip->V[X] = chip->V[X] & chip->V[Y];
					break;
				case 0x0003:
					// set VX to value of VX XOR VY 
					chip->V[X] = chip->V[X] ^ chip->V[Y];
					break;
				case 0x0004:
					// add value of VY to VX
					// set carry flag to 1 if overflow, 0 if not
					if (chip->V[X] + chip->V[Y] > 255) {
						chip->V[F] = 1;
					} else {
						chip->V[F] = 0;
					}
					chip->V[X] += chip->V[Y];
					break;
				case 0x0005:
					// subtract value of VY from VX
					// set carry flag to 0 if borrow, 1 if not
					if (chip->V[X] < chip->V[Y]) {
						chip->V[F] = 0;
					} else {
						chip->V[F] = 1;
					}
					chip->V[X] -= chip->V[Y];
					break;
				case 0x0006:
					// shift VX right by one, store least significant bit in VF
					if (chip->V[X] % 2 == 0) {
						chip->V[F] = 0;
					} else {
						chip->V[F] = 1;
					}
					chip->V[X] = chip->V[X] >> 1;
					break;
				case 0x0007:
					// set VX to value of VY minus VX
					// set carry flag to 0 if borrow, 1 if not
					if (chip->V[Y] < chip->V[X]) {
						chip->V[F] = 0;
					}
					else {
						chip->V[F] = 1;
					}
					chip->V[X] = chip->V[Y] - chip->V[X];
					break;
				case 0x000E:
					// shift VX left by one, store most significant bit in VF
					if (chip->V[X] < 128) {
						chip->V[F] = 0;
					} else {
						chip->V[F] = 1;
					}
					chip->V[X] = chip->V[X] << 1;
					break;
				default:
					printf("Unknown opcode: 0x%x\n", opcode);
					break;
			}
			break;
		case 0x9000:
			// skip next instruction if VX not equals VY
			if (chip->V[X] != chip->V[Y]) { chip->pctr += 2; }
			break;
		case 0xA000:
			// set I to address NNN
			chip->I = opcode & 0x0FFF;
			break;
		case 0xB000:
			// jump to address NNN plus V0
			chip->pctr = jump_addr + chip->V[0];
			jump = TRUE;
			break;
		case 0xC000:
			// set VX to random number plus NN
			chip->V[X] = (rand() % 255) & (opcode & 0x00FF);
			break;
		case 0xD000: {
			// draws a sprite to the screen
			// uses coordinates stored in VX and VY, with height given by N
			int height = opcode & 0x000F;
			int x_coord = chip->V[X];
			int y_coord = chip->V[Y];
			
			// because the sprite is represented by hexadecimal numbers
			// bitwise operators are necessary to obtain each pixel
			int ands[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

			// set carry flag to 0
			chip->V[F] = 0;
			// drawing loop
			for (i = 0; i < height; i++) {
				for (j = 0; j < SPRITE_WIDTH; j++) {
					// allows sprite to wrap around screen
					if (x_coord + j == SCREEN_WIDTH) {
						x_coord = -j;
					}
					if (y_coord + i == SCREEN_HEIGHT) {
						y_coord = -i;
					}

					// set carry flag to 1 if a sprite changes from set to unset
					if (chip->display[x_coord + j][y_coord + i] == 1 &&
						((chip->memory[chip->I + i] & ands[j]) >> (8 - j - 1)) == 1) {
						chip->V[F] = 1;
					}

					// bitwise operations decode each bit of sprite and XOR with the current pixel on screen
					chip->display[x_coord + j][y_coord + i] = chip->display[x_coord + j][y_coord + i] ^ 
						((chip->memory[chip->I + i] & ands[j]) >> (8 - j - 1));
				}
				x_coord = chip->V[X];
				y_coord = chip->V[Y];
			}
			break;
		}
		case 0xE000:
			if (NN == 0x009E) {
				// skip following instruction if key in VX is pressed
				if (chip->key == chip->V[X]) {
					chip->pctr += 2;
				}
			} else if (NN == 0x00A1) {
				// skip following instruction if key in VX is not pressed
				if (chip->key != chip->V[X]) {
					chip->pctr += 2;
				}
			} else {
				printf("Unknown opcode: %x\n", opcode);
			}
			break;
		case 0xF000:
			switch (opcode & 0x00FF) {
				case 0x0007:
					// set VX to the value of the delay timer
					chip->V[X] = chip->delay_timer;
					break;
				case 0x000A:
					// await key press, then store in VX
				if (chip->key == 0) {
						return;
					} else {
						chip->V[X] = chip->key;
					}
					break;
				case 0x0015:
					// set delay timer to value of VX
					chip->delay_timer = chip->V[X];
					break;
				case 0x0018:
					// set sound timer to value of VX
					chip->sound_timer = chip->V[X];
					break;
				case 0x001E:
					// add value of VX to I
					chip->I += chip->V[X];
					break;
				case 0x0029:
					// set I to the location of the sprite for the character in VX
					// characters 0-F (in hexadecimal) are represented by a 4x5 font
					chip->I = chip->V[X] * 5;
					break;
				case 0x0033:
					// store binary encoded decimal representation of VX
					chip->memory[chip->I] = chip->V[X] / 100;
					chip->memory[chip->I + 1] = (chip->V[X] / 10) % 10;
					chip->memory[chip->I + 2] = chip->V[X] % 10;
					break;
				case 0x0055:
					// store V0 to VX in memory starting at address 
					for (i = 0; i < X + 1; i++) {
						chip->memory[chip->I + i] = chip->V[i];
					}
					chip->I = chip->I + X + 1;
					break;
				case 0x0065:
					// fill V0 to VX with stored memory starting at address I
					for (i = 0; i < X + 1; i++) {
						chip->V[i] = chip->memory[chip->I + i];
					}
					chip->I = chip->I + X + 1;
					break;
				default:
					printf("Unknown opcode: 0x%x\n", opcode);
					break;
			}
		break;
		default:
			printf("Unknown opcode: 0x%x\n", opcode);
			break;
	}

	// increment program counter if there is no memory address jump
	if (!jump) { chip->pctr += 2; }
}