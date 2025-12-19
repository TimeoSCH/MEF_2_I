#ifndef LEAK_H
#define LEAK_H

#include <stdio.h>


void traiter_fuites(char* chemin, char* mode, FILE* f_out);

// Fonctions utilitaires détectées dans les symboles de leak.o
int estEgal_leaks(const char* s1, const char* s2);
long chaineVersLong_leaks(const char* s);

#endif
