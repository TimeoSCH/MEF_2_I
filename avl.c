#include "avl.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// --- Fonctions utilitaires ---

int max(int a, int b) {
    return (a > b) ? a : b;
}

int hauteur(pStation a) {
    if (a == NULL) return 0;
    return a->h;
}

int equilibre(pStation a) {
    if (a == NULL) return 0;
    return hauteur(a->fg) - hauteur(a->fd);
}

// --- Création ---

pStation creerStation(int id, char* code, long cap, long conso_initiale) {
    pStation new = malloc(sizeof(Station));
    if (new == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(1);
    }
    new->id = id;
    
    // Copie sécurisée de l'identifiant texte
    if (code != NULL) {
        strncpy(new->id_str, code, 49);
        new->id_str[49] = '\0';
    } else {
        new->id_str[0] = '\0';
    }

    new->capacite = cap;
    new->conso = conso_initiale; 
    new->h = 1;
    new->fg = NULL;
    new->fd = NULL;
    return new;
}

// --- Rotations ---

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

// --- Insertion ---

pStation inserer(pStation a, int id, char* code, long cap, long flux) {
    // 1. Insertion normale
    if (a == NULL) {
        // Si le nœud n'existe pas, on le crée avec le flux actuel comme consommation
        return creerStation(id, code, cap, flux);
    }

    if (id < a->id) {
        a->fg = inserer(a->fg, id, code, cap, flux);
    } else if (id > a->id) {
        a->fd = inserer(a->fd, id, code, cap, flux);
    } else {
        // ID déjà présent : on cumule la consommation
        a->conso += flux;
        return a;
    }

    // 2. Mise à jour hauteur
    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));

    // 3. Équilibrage
    int bal = equilibre(a);

    if (bal > 1 && id < a->fg->id) return rotationDroite(a);
    if (bal < -1 && id > a->fd->id) return rotationGauche(a);
    if (bal > 1 && id > a->fg->id) return doubleRotationGD(a);
    if (bal < -1 && id < a->fd->id) return doubleRotationDG(a);

    return a;
}

// --- Parcours et Libération ---

void infixe(pStation a, FILE* fs) {
    if (a != NULL) {
        infixe(a->fg, fs);
        // Format de sortie ID:CAP:CONSO (adaptez le séparateur ':' ou ';' selon votre sujet)
        fprintf(fs, "%d:%ld:%ld\n", a->id, a->capacite, a->conso);
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
