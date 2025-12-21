#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Station {   //structure d'un AVL
    char id_str[50];    
    long capacite;
    long conso;
    int h;
    struct Station *fg;
    struct Station *fd;
} Station;

typedef Station* pStation;

// Utilitaires
int max(int a, int b);
int hauteur(pStation a);
int equilibre(pStation a);

// Rotations
pStation rotationDroite(pStation y);
pStation rotationGauche(pStation x);
pStation doubleRotationGD(pStation a);
pStation doubleRotationDG(pStation a);

// Fonctions principales
pStation creerStation(char* code, long cap, long conso);
pStation inserer(pStation a, char* code, long cap, long flux);

// Parcours et mémoire
void infixe(pStation a, FILE* fs);
void liberer(pStation a);

#endif
