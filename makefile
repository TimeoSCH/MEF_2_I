CC = gcc
CFLAGS = -Wall -Wextra -std=c99
EXEC = c-wire
SRC = main.c avl.c file.c
OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c avl.h file.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC) *.o

.PHONY: all clean
