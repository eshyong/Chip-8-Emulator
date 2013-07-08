#include <GLUT.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"

#define WINDOW_LENGTH 640
#define WINDOW_HEIGHT 320

// chip to emulate
chip8 chip;

// method to draw a single pixel on the screen
void draw_square(float x_coord, float y_coord);

// update and render logic, called by glutDisplayFunc
void render();

// idling function for animation, called by glutIdleFunc
void idle();

// initializes GL 2D mode and other settings
void initGL();

// handles user keyboard input
void handle_key_press(unsigned char key, int x, int y);

// handles key release
void handle_key_release(unsigned char key, int x, int y);

/*	SUMMARY:
*	Main emulation loop. Loads ROM, executes it, and draws to screen.
*	PARAMETERS:
*	argv: number of command line arguments
*	argc[]: array of command line arguments
*	RETURN VALUE: usually 0
*/
int main(int argc, char *argv[]) {
	// seed random variable for use in emulation
	srand(time(NULL));

	// initialize chip memory
	initialize(&chip);

	// initialize GLUT
	glutInit(&argc, argv);

	// initialize display and window
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_LENGTH, WINDOW_HEIGHT);
	glutCreateWindow("Eric's GLUT Emulator");

	// initialize orthographic 2D view, among other things
	initGL();

	// GLUT draw function
	glutDisplayFunc(render);

	// GLUT idle function, causes screen to redraw
	glutIdleFunc(idle);

	// handles key presses and releases
	glutKeyboardFunc(handle_key_press);
	glutKeyboardUpFunc(handle_key_release);

	// main loop, all events processed here
	glutMainLoop();

	// // TODO: delete debugging loop
	// char c;
	// int i, j = 0;
	// initialize(&chip);

	// for (i = 0; i < 1000; i++) {
	// 	emulate_cycle(&chip);
	// }

	// while (c != 'x') {
	// 	scanf("%c", &c);
	// 	if (c == '\n') {
	// 		emulate_cycle(&chip);
	// 		for (i = 0; i < NUM_REGISTERS; i++) {
	// 			printf("V%x: %d\n", i, chip.V[i]);
	// 		}
	// 		if (chip.draw_flag) {
	// 			draw_screen(&chip);
	// 		}
	// 		printf("I: %d\n", chip.I);
	// 		for (i = 0; i < 6; i++) {
	// 			printf("I + %d: %d\n", i, chip.memory[chip.I + i]);
	// 		}
	// 		printf("Stack pointer: %d\n", chip.sptr);
	// 		printf("Stack[%d]: %x\n", chip.sptr, chip.stack[chip.sptr]);
	// 		printf("Program counter: %x\n", chip.pctr);
	// 		printf("Keys pressed: %c\n", chip.key);
	// 		j++;
	// 	}
	// }

	// printf("Instructions executed: %d\n", j);

	return 0;
}

/*	SUMMARY:
*	Handles all keypresses and passes them to the emulator.
*	PARAMETERS: 
*	key: the key that is pressed.
*	x: syntax required by GLUT
*	y: syntax required by GLUT
*	RETURN VALUE: none
*/
void handle_key_press(unsigned char key, int x, int y) {
	// key input is stored by chip
	// left side of keyboard (QWER) is mapped to hex layout:
	// |1|2|3|C|		=>		|1|2|3|4|
	// |4|5|6|D|		=>		|Q|W|E|R|
	// |7|8|9|E|		=>		|A|S|D|F|
	// |A|0|B|F|		=>		|Z|X|C|V|
	unsigned char input;
	switch (key) {
		case 27:
			// ASCII representation of ESC key
			// allows user to exit application
			printf("Exiting emulator...\n");
			exit(0);
		case '1':
			// only certain key values are passed to the chip
			input = 0x0001;
			break;
		case '2':
			input = 0x0002;
			break;
		case '3':
			input = 0x0003;
			break;
		case '4':
			input = 0x000C;
			break;
		case 'q':
			input = 0x0000;
			break;
		case 'w':
			input = 0x0004;
			break;
		case 'e':
			input = 0x0000;
			break;
		case 'r':
			input = 0x000D;
			break;
		case 'a':
			input = 0x0005;
			break;
		case 's':
			input = 0x0007;
			break;
		case 'd':
			input = 0x0006;
			break;
		case 'f':
			input = 0x000E;
			break;
		case 'z':
			input = 0x000A;
			break;
		case 'x':
			input = 0x0000;
			break;
		case 'c':
			input = 0x000B;
			break;
		case 'v':
			input = 0x000F;
			break;
		default:
			break;
	}
	handle_input(&chip, input);
}

/*	SUMMARY:
*	Tells emulator if any keys are released. Gives a default char value of 255.
*	PARAMETERS: 
*	key: the key that is pressed.
*	x: syntax required by GLUT
*	y: syntax required by GLUT
*	RETURN VALUE: none
*/
void handle_key_release(unsigned char key, int x, int y) {
	handle_input(&chip, 0x00FF);
}

/*	SUMMARY:
*	Draws a square. Represents a single pixel on the original Chip 8,
*	but is scaled to a 640 x 320 display.
*	PARAMETERS:
*	x_coord: x coordinate of the square
*	y_coord: y coordinate of the square
*	RETURN VALUE: none
*/
void draw_square(float x_coord, float y_coord) {
	// draws a blue 10 x 10 square with the coordinates passed
	glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex2f(x_coord, y_coord);
		glVertex2f(x_coord + 10, y_coord);
		glVertex2f(x_coord + 10, y_coord + 10);
		glVertex2f(x_coord, y_coord + 10);
	glEnd();
}

/*	SUMMARY:
*	GLUT render function to draw the display. Also emulates one
*	cycle of the chip.
*	PARAMETERS: none
*	RETURN VALUE: none
*/
void render() {
	// clears screen
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();

	// emulates one cycle of the chip
	emulate_cycle(&chip);

	// draw a pixel for each display bit
	int i, j;
	for (i = 0; i < SCREEN_WIDTH; i++) {
		for (j = 0; j < SCREEN_HEIGHT; j++) {
			if (chip.display[i][j] == 1) {
				draw_square((float)(i * 10), (float)(j * 10));
			}
		}
	}

	// swap buffers, allows for smooth animation
	glutSwapBuffers();
}

/*	SUMMARY:
*	GLUT idle function. Instructs GLUT window to redraw
*	itself.
*	PARAMETERS: none
*	RETURN VALUE: none
*/
void idle() {
	// gives the call to redraw the screen
	glutPostRedisplay();
}

/*	SUMMARY:
*	Initializes GLUT settings.
*	PARAMETERS: none
*	RETURN VALUE: none
*/
void initGL() {
	// sets up GLUT window for 2D drawing
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WINDOW_LENGTH, WINDOW_HEIGHT, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// clears screen color
	glClearColor(0.f, 0.f, 0.f, 1.f);
}
