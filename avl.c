#include "avl.h"

#define min3(a, b, c) min(a, min(b, c))
#define max3(a, b, c) max(a, max(b, c))

//rotation simple gauche
pStation rotationGauche(pStation a) {
    pStation pivot = a->fd;
    int eq_a = a->equilibre;
    int eq_p = pivot->equilibre;
    a->fd = pivot->fg;
    pivot->fg = a;
    a->equilibre = eq_a - max(eq_p, 0) - 1;
    pivot->equilibre = min3(eq_a - 2, eq_a + eq_p - 2, eq_p - 1);

    return pivot;
}

// rotation simple droite.
pStation rotationDroite(pStation a) {
    pStation pivot = a->fg;
    int eq_a = a->equilibre;
    int eq_p = pivot->equilibre;
    a->fg = pivot->fd;
    pivot->fd = a;  
    a->equilibre = eq_a - min(eq_p, 0) + 1;
    pivot->equilibre = max3(eq_a + 2, eq_a + eq_p + 2, eq_p + 1);

    return pivot;
}

/// Fonction double rotation droite.
pStation doubleRotationD(pStation a) {
    a->fg = rotationGauche(a->fg); 
    return rotationDroite(a);      
}

//fonction double rotation gauche
pStation doubleRotationG(pStation a) {
    a->fd = rotationDroite(a->fd); 
    return rotationGauche(a);      
}

// Alloue et initialise une nouvelle station
pStation creerStation(char* code, long cap, long conso) {
    pStation p = malloc(sizeof(Station));
    if (p == NULL) exit(1);
    
    strcpy(p->id_str, code);
    p->capacite = cap;
    p->conso = conso;
    p->equilibre = 0; 
    p->fg = NULL;
    p->fd = NULL;
    
    return p;
}

// Fonction d'insertion
pStation inserer_interne(pStation a, char* code, long cap, long flux, int* h) {
    if (a == NULL) {
        *h = 1; 
        return creerStation(code, cap, flux);
    }

    int cmp = strcmp(code, a->id_str);

    if (cmp < 0) {
        // Insertion dans le sous-arbre GAUCHE
        a->fg = inserer_interne(a->fg, code, cap, flux, h);
        if (*h != 0) { 
            a->equilibre--; 
            
            if (a->equilibre == 0) {
                *h = 0; 
            } else if (a->equilibre == -1) {
                *h = 1; 
            } else { 
                if (a->fg->equilibre == 1) {
                    a = doubleRotationGD(a); 
                } else {
                    a = rotationDroite(a);   
                }
                *h = 0; 
            }
        }
    } 
    else if (cmp > 0) {
        // Insertion dans le sous-arbre droit
        a->fd = inserer_interne(a->fd, code, cap, flux, h);
        
        if (*h != 0) { 
            a->equilibre++; 
            if (a->equilibre == 0) {
                *h = 0;
            }else if (a->equilibre == 1) {
                *h = 1;
            }else { // Cas critique (+2)
                if (a->fd->equilibre == -1) {
                    a = doubleRotationDG(a); 
                }else {
                    a = rotationGauche(a);   
                }
                *h = 0;
            }
        }
    }else {
        // Si le nœud existe déjà, on met à jour les données
        a->capacite += cap;
        a->conso += flux;
        *h = 0; 
    }

    return a;
}

// Appel depuis le main
pStation inserer(pStation a, char* code, long cap, long flux) {
    int h = 0; 
    return inserer_interne(a, code, cap, flux, &h);
}

// Tri par ordre décroissant
void infixe(pStation a, FILE* fs) {     
    if (a != NULL) {
        infixe(a->fd, fs); 
        fprintf(fs, "%s;%ld;%ld\n", a->id_str, a->capacite, a->conso);
        infixe(a->fg, fs); 
    }
}

// Libération récursive de la mémoire
void liberer(pStation a) {
    if (a != NULL) {
        liberer(a->fg);
        liberer(a->fd);
        free(a);
    }
}
