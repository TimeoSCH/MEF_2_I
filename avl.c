#include "avl.h"

int max(int a, int b) { return (a > b) ? a : b; }
int hauteur(pStation a) { return (a == NULL) ? 0 : a->h; }
int equilibre(pStation a) { return (a == NULL) ? 0 : hauteur(a->fg) - hauteur(a->fd); }

pStation rotationDroite(pStation y) {
    pStation x = y->fg;
    pStation T2 = x->fd;
    x->fd = y;
    y->fg = T2;
    y->h = max(hauteur(y->fg), hauteur(y->fd)) + 1;
    x->h = max(hauteur(x->fg), hauteur(x->fd)) + 1;
    return x;
}
pStation rotationGauche(pStation x) {
    pStation y = x->fd;
    pStation T2 = y->fg;
    y->fg = x;
    x->fd = T2;
    x->h = max(hauteur(x->fg), hauteur(x->fd)) + 1;
    y->h = max(hauteur(y->fg), hauteur(y->fd)) + 1;
    return y;
}
pStation doubleRotationGD(pStation a) {
    a->fg = rotationGauche(a->fg);
    return rotationDroite(a);
}
pStation doubleRotationDG(pStation a) {
    a->fd = rotationDroite(a->fd);
    return rotationGauche(a);
}

// CORRECTION MAJEURE ICI
pStation creerStation(char* code, long cap, long conso) {
    pStation p = malloc(sizeof(Station));
    if (p == NULL) exit(1);
    strcpy(p->id_str, code); // Copie du texte
    p->capacite = cap;
    p->conso = conso;
    p->h = 1;
    p->fg = NULL;
    p->fd = NULL;
    return p;
}

pStation inserer(pStation a, char* code, long cap, long flux) {
    if (a == NULL) {
        return creerStation(code, cap, flux);
    }

    // Utilisation de STRCMP pour le tri alphabétique
    int cmp = strcmp(code, a->id_str);

    if (cmp < 0) {
        a->fg = inserer(a->fg, code, cap, flux);
    } else if (cmp > 0) {
        a->fd = inserer(a->fd, code, cap, flux);
    } else {
        a->capacite += cap; // Important pour les modes HVA
        a->conso += flux;
        return a;
    }

    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));
    int bal = equilibre(a);

    if (bal > 1 && strcmp(code, a->fg->id_str) < 0) return rotationDroite(a);
    if (bal < -1 && strcmp(code, a->fd->id_str) > 0) return rotationGauche(a);
    if (bal > 1 && strcmp(code, a->fg->id_str) > 0) return doubleRotationGD(a);
    if (bal < -1 && strcmp(code, a->fd->id_str) < 0) return doubleRotationDG(a);

    return a;
}

void infixe(pStation a, FILE* fs) {
    if (a != NULL) {
        infixe(a->fg, fs);
        fprintf(fs, "%s;%ld;%ld\n", a->id_str, a->capacite, a->conso);
        infixe(a->fd, fs);
    }
}

void liberer(pStation a) {
    if (a != NULL) {
        liberer(a->fg);
        liberer(a->fd);
        free(a);
    }
}
