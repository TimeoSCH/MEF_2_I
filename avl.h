#ifndef AVL_H
#define AVL_H

#include "structures.h"

// Structure du noeud AVL [cite: 200]
typedef struct Arbre {
    Station* elmt;          
    struct Arbre* fg;       
    struct Arbre* fd;       
    int equilibre;          
} Arbre;

// Prototypes
Arbre* creerArbre(Station* s);
Arbre* insertionAVL(Arbre* a, Station* s, int* h);
Station* rechercherStation(Arbre* a, char* id);
void libererAVL(Arbre* a);

#endif
