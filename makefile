CC = gcc
CFLAGS = -Wall -Wextra -g

# Nom de votre exécutable final
EXEC = programme

# Liste des fichiers sources
SRC = main.c avl.c

# Liste des fichiers objets (générée automatiquement)
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
