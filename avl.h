#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// formules de rotation
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

//structure d'un avl
typedef struct Station {        
    char id_str[50];
    long capacite;
    long conso;
    int equilibre; 
    struct Station *fg;
    struct Station *fd;
} Station;

typedef Station* pStation;

// Fonctions principales
pStation creerStation(char* code, long cap, long conso);
pStation inserer(pStation a, char* code, long cap, long flux);

// Parcours et mémoire
void infixe(pStation a, FILE* fs);
void liberer(pStation a);

#endif
