#include "avl.h"
#include <stdio.h>
#include <stdlib.h>

// --- VOTRE CODE (STRUCTURE ET OUTILS) ---

pArbre creerArbre(int id, long capacity, long consumption) {
    pArbre new = malloc(sizeof(Arbre));
    if (new == NULL) {
        exit(1);
    }
    new->id = id;
    new->capacity = capacity;
    new->consumption = consumption;
    new->height = 1;
    new->fg = NULL;
    new->fd = NULL;
    return new;
}

int hauteur(pArbre a) {
    if (a == NULL) {
        return 0;
    }
    return a->height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

pArbre rotationDroite(pArbre a) {
    pArbre b = a->fg;
    pArbre T2 = b->fd;

    b->fd = a;
    a->fg = T2;

    a->height = max(hauteur(a->fg), hauteur(a->fd)) + 1;
    b->height = max(hauteur(b->fg), hauteur(b->fd)) + 1;

    return b;
}

pArbre rotationGauche(pArbre a) {
    pArbre b = a->fd;
    pArbre T2 = b->fg;

    b->fg = a;
    a->fd = T2;

    a->height = max(hauteur(a->fg), hauteur(a->fd)) + 1;
    b->height = max(hauteur(b->fg), hauteur(b->fd)) + 1;

    return b;
}

pArbre equilibrage(pArbre a) {
    if (a == NULL) {
        return NULL;
    }

    int delta = hauteur(a->fg) - hauteur(a->fd);

    if (delta > 1) {
        if (hauteur(a->fg->fg) >= hauteur(a->fg->fd)) {
            return rotationDroite(a);
        } else {
            a->fg = rotationGauche(a->fg);
            return rotationDroite(a);
        }
    }

    if (delta < -1) {
        if (hauteur(a->fd->fd) >= hauteur(a->fd->fg)) {
            return rotationGauche(a);
        } else {
            a->fd = rotationDroite(a->fd);
            return rotationGauche(a);
        }
    }
    return a;
}

pArbre insertion(pArbre a, int id, long capacity, long consumption) {
    if (a == NULL) {
        return creerArbre(id, capacity, consumption);
    }
    if (id < a->id) {
        a->fg = insertion(a->fg, id, capacity, consumption);
    } else if (id > a->id) {
        a->fd = insertion(a->fd, id, capacity, consumption);
    } else {
        // ID existant : on somme la capacité et la consommation (Logique correcte)
        a->capacity += capacity; // Attention: selon le sujet, la capacité est parfois fixe, mais pour la somme c'est ok
        a->consumption += consumption;
        return a;
    }

    a->height = max(hauteur(a->fg), hauteur(a->fd)) + 1;
    return equilibrage(a);
}

// --- PARTIE AJOUTÉE (INTEGRATION DE LA CORRECTION) ---
// Cette fonction manquait pour sortir les données vers le script .sh

void parcoursInfixe(pArbre a) {
    if (a != NULL) {
        parcoursInfixe(a->fg);
        // Format de sortie standard : ID:CAPACITE:CONSOMMATION
        printf("%d:%ld:%ld\n", a->id, a->capacity, a->consumption);
        parcoursInfixe(a->fd);
    }
}

