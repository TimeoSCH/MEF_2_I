#ifndef USINE_H
#define USINE_H

#include <stdint.h>

typedef struct {
    uint32_t id;
    uint32_t captage;
    uint32_t acheminement;
    uint32_t traitement;
} Usine;

Usine *create_usine(uint32_t id);
void update_usine(Usine *u, uint32_t cap, uint32_t ach, uint32_t trt);

#endif
