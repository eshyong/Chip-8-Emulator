// define flags
#ifndef CHIP8_H
#define CHIP8_H

#define FONTSET_SIZE 80
#define MAX_FILE_SIZE 3584
#define NUM_KEYS 16
#define NUM_REGISTERS 16
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SPRITE_WIDTH 8
#define STACK_SIZE 16
#define SYSTEM_MEMORY 4096
#define F 15

// since C doesn't handle bool, typedef our own
typedef int bool;
#define FALSE 0
#define TRUE 1

/*	SUMMARY:
*	Chip 8 Virtual Machine. Can run Pong.
*/
typedef struct chip8 {
	// draw flag: tells CPU whether or not to draw
	bool draw_flag;

	// total system memory: 4 KB in total
	unsigned char memory[SYSTEM_MEMORY];

	// system registers: V0 - VE are general purpose while VF is the carry flag
	unsigned char V[NUM_REGISTERS];

	// register I: used for drawing
	unsigned short I;

	// program counter: keeps track of position in program
	unsigned short pctr;

	// graphics display: draws to screen
	unsigned char display[SCREEN_WIDTH][SCREEN_HEIGHT];

	// delay and sound timers
	unsigned char delay_timer;
	unsigned char sound_timer;

	// stack and stack pointer: used to handle subroutines
	unsigned short stack[STACK_SIZE];
	unsigned short sptr;

	// keys: to press
	unsigned char key;
} chip8;

// Chip 8 methods to emulate a ROM program
extern void initialize(chip8 *chip);

extern void emulate_cycle(chip8 *chip);

extern void draw_screen(chip8 *chip);

extern void handle_input(chip8 *chip, unsigned char key);

extern int load_file(char *file_name, unsigned char *buffer);

#endif
