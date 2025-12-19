#ifndef FILE_H
#define FILE_H

#include <stdio.h> // Nécessaire pour FILE*
#include "avl.h"

// Charge le fichier CSV dans l'arbre AVL selon le mode choisi (histo)
void charger(char* chemin, pStation* racine, char* mode);

// Calcule les fuites pour une station précise (leaks)
void traiter_fuites(char* chemin, char* station_id, FILE* f_out);

#endif
