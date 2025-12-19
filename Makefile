CC = gcc
CFLAGS = -Wall -Wextra -g

# Nom de l'exécutable final
EXEC = c-wire

# Liste de tous les fichiers sources
# IMPORTANT : On inclut file.c et leak.c car main.c les utilise
SRC = main.c avl.c file.c leak.c
OBJ = $(SRC:.c=.o)

# Règle par défaut (lance la compilation de l'exécutable)
all: $(EXEC)

# Édition des liens (création de l'exécutable)
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# --- Règles de compilation pour chaque fichier ---

# main.c dépend des trois headers
main.o: main.c avl.h file.h leak.h
	$(CC) $(CFLAGS) -c main.c

# avl.c ne dépend que de avl.h
avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

# file.c dépend souvent de avl.h (car il manipule l'arbre)
file.o: file.c file.h avl.h
	$(CC) $(CFLAGS) -c file.c

# leak.c dépend de leak.h
leak.o: leak.c leak.h
	$(CC) $(CFLAGS) -c leak.c

# Nettoyage des fichiers temporaires
clean:
	rm -f $(OBJ) $(EXEC)
