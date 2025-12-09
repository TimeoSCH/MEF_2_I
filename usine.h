#ifndef USINE_H
#define USINE_H

#include <stdint.h>

typedef struct {
     int id;
     int captage;
     int acheminement;
     int traitement;
} Usine;

Usine *create_usine(int id);
void update_usine(Usine *u, int captage, int acheminement, int traitement);

#endif
