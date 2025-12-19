CC = gcc
CFLAGS = -Wall -Wextra -g

# Nom de l'exécutable généré
EXEC = c-wire

# Fichiers sources
SRC = main.c avl.c
OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c avl.h
	$(CC) $(CFLAGS) -c main.c

avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

clean:
	rm -f $(OBJ) $(EXEC)
