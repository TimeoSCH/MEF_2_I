CC = gcc
CFLAGS = -Wall -Wextra -g

# Nom de l'exécutable final
EXEC = c-wire

# Liste des fichiers sources
SRC = main.c avl.c file.c leak.c
OBJ = $(SRC:.c=.o)

# Règle par défaut
all: $(EXEC)

# Édition des liens
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Règles de compilation
main.o: main.c avl.h file.h leak.h
	$(CC) $(CFLAGS) -c main.c

avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

file.o: file.c file.h avl.h
	$(CC) $(CFLAGS) -c file.c

leak.o: leak.c leak.h
	$(CC) $(CFLAGS) -c leak.c

# Nettoyage
clean:
	rm -f $(OBJ) $(EXEC)
