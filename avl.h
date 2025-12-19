#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définition de la structure
typedef struct Station {
    int id;             // Identifiant numérique
    char id_str[50];    // Identifiant texte
    long capacite;      // Capacité
    long conso;         // Consommation 
    int h;              // Hauteur
    struct Station *fg; // Fils gauche
    struct Station *fd; // Fils droit
} Station;

typedef Station* pStation;

// --- Prototypes ---

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
// CORRECTION ICI : Ajout du 4ème argument 'long conso' pour correspondre au .c
pStation creerStation(int id, char* code, long cap, long conso);

pStation inserer(pStation a, int id, char* code, long cap, long flux);

// Parcours et mémoire
void infixe(pStation a, FILE* fs);
void liberer(pStation a);

#endif
