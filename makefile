CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O3 -march=native
EXEC = c-wire
OBJ = main.o avl.o file.o leaks.o

# Règle par défaut
all: $(EXEC)

# Édition de liens
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation des objets
main.o: main.c avl.h file.h leaks.h
	$(CC) $(CFLAGS) -c main.c

avl.o: avl.c avl.h
	$(CC) $(CFLAGS) -c avl.c

file.o: file.c file.h avl.h
	$(CC) $(CFLAGS) -c file.c

leaks.o: leaks.c leaks.h
	$(CC) $(CFLAGS) -c leaks.c

# Nettoyage
clean:
	rm -f $(OBJ) $(EXEC)
