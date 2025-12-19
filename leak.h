#ifndef LEAK_H
#define LEAK_H

#include <stdio.h> // Nécessaire pour FILE*

void traiter_fuites(char* chemin, char* station_id, FILE* f_out);

#endif
