Chip-8-Emulator
===============

Interpreter for the Chip 8 Virtual Machine. Can run Pong and Tetris.

Tutorials:
---------------

Mastering Chip 8 by Matthew Mikolay
http://mattmik.com/chip8.html

How to Write an Emulator (Chip-8 Interpreter) by Laurence Muller
http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

About:
--------------

Key mapping is based on hexadecimal keypad:

	|1|2|3|C|	=>	|1|2|3|4|
	|4|5|6|D|	=>	|Q|W|E|R|
	|7|8|9|E|	=>	|A|S|D|F|
	|A|0|B|F|	=>	|Z|X|C|V|

Tetris:

	4 -> ROTATE
	5 -> LEFT
	6 -> RIGHT

Pong:

	Q -> P1 UP
	1 -> P1 DOWN
	4 -> P2 UP
	R -> P2 DOWN

To-Do:
---------------

* change gameplay speed to be more playable
* add header file support for Win/Linux
* add makefiles for Win/Linux
