CC=gcc
CFLAGS=-std=c99	-c	-Wall	-I/System/Library/Frameworks/GLUT.framework/Headers
LDFLAGS=-framework OpenGL	-framework	GLUT
SOURCES=chip8.c	emulator.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=emulator

clean:
	rm	-rf	*o	emulator

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@