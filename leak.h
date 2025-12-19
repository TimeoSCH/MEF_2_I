#ifndef LEAKS_H
#define LEAKS_H

#include <stdio.h> // Nécessaire pour FILE*

void traiter_fuites(char* chemin, char* station_id, FILE* f_out);

#endif
