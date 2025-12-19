CC = gcc
CFLAGS = -Wall -Wextra -g

# Nom de l'exécutable final
EXEC = c-wire

# Liste des fichiers sources (main + tous les modules)
SRC = main.c avl.c file.c leak.c
OBJ = $(SRC:.c=.o)

# Règle par défaut
all: $(EXEC)

# Édition des liens pour créer l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# --- Règles de compilation individuelles ---

# main.c dépend des 3 headers
main.o: main.c avl.h file.h leak.h
	$(CC) $(CFLAGS) -c main.c

# avl.c ne dépend que de avl.h
avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

# file.c dépend de file.h ET avl.h (car il utilise pStation)
file.o: file.c file.h avl.h
	$(CC) $(CFLAGS) -c file.c

# leak.c dépend de leak.h
leak.o: leak.c leak.h
	$(CC) $(CFLAGS) -c leak.c

# Nettoyage
clean:
	rm -f $(OBJ) $(EXEC)
