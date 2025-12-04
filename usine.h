#ifndef USINE_H
#define USINE_H

#include <stdint.h>

typedef struct {
     int id;
     int captage;
     int acheminement;
     int traitement;
} Usine;

Usine *create_usine(uint32_t id);
void update_usine(Usine *u, uint32_t cap, uint32_t ach, uint32_t trt);

#endif
