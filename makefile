CC = gcc
# Compilation standard avec debug (-g) et warnings
CFLAGS = -std=c11 -Wall -Wextra -g
EXEC = c-wire
OBJ = main.o avl.o file.o leak.o

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c avl.h file.h leak.h
	$(CC) $(CFLAGS) -c main.c

avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

file.o: file.c file.h avl.h
	$(CC) $(CFLAGS) -c file.c

leak.o: leak.c leak.h
	$(CC) $(CFLAGS) -c leak.c

clean:
	rm -f $(OBJ) $(EXEC)
