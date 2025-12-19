#ifndef FILE_H
#define FILE_H

#include <stdio.h> // Nécessaire pour FILE*
#include "avl.h"

int estEgal(const char* s1, const char* s2);

void charger(char* chemin, pStation* racine, char* mode);

#endif
