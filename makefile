CC = gcc

CFLAGS = -std=c11 -Wall -Wextra -g
EXEC = c-wire
OBJ = main.o avl.o file.o leaks.o

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c avl.h file.h leaks.h
	$(CC) $(CFLAGS) -c main.c

avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

file.o: file.c file.h avl.h
	$(CC) $(CFLAGS) -c file.c

leaks.o: leaks.c leaks.h
	$(CC) $(CFLAGS) -c leaks.c

clean:
	rm -f $(OBJ) $(EXEC)
