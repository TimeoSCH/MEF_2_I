#include "avl.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Fonction utilitaire : Max entre deux entiers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Fonction utilitaire : Hauteur d'un noeud (gère le NULL)
int hauteur(pStation a) {
    if (a == NULL)
        return 0;
    return a->h;
}

// Fonction utilitaire : Facteur d'équilibre
int equilibre(pStation a) {
    if (a == NULL)
        return 0;
    return hauteur(a->fg) - hauteur(a->fd);
}

// Création d'une station
// On initialise la conso à 0 car elle sera ajoutée via 'inserer'
pStation creerStation(int id, char* code, long cap) {
    pStation new = malloc(sizeof(Station));
    if (new == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(1);
    }
    new->id = id;
    // Important : on copie la chaîne de caractères
    if (code != NULL) {
        strncpy(new->id_str, code, 49);
        new->id_str[49] = '\0'; // Sécurité
    } else {
        new->id_str[0] = '\0';
    }
    
    new->capacite = cap;
    new->conso = 0; // Initialisation
    new->h = 1;
    new->fg = NULL;
    new->fd = NULL;
    return new;
}

// --- Rotations ---

pStation rotationDroite(pStation y) {
    pStation x = y->fg;
    pStation T2 = x->fd;

    // Rotation
    x->fd = y;
    y->fg = T2;

    // Mise à jour hauteurs
    y->h = max(hauteur(y->fg), hauteur(y->fd)) + 1;
    x->h = max(hauteur(x->fg), hauteur(x->fd)) + 1;

    return x;
}

pStation rotationGauche(pStation x) {
    pStation y = x->fd;
    pStation T2 = y->fg;

    // Rotation
    y->fg = x;
    x->fd = T2;

    // Mise à jour hauteurs
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

// --- Insertion et Logique métier ---

pStation inserer(pStation a, int id, char* code, long cap, long flux) {
    // 1. Insertion classique ABR
    if (a == NULL) {
        pStation n = creerStation(id, code, cap);
        n->conso = flux; // On assigne le premier flux
        return n;
    }

    if (id < a->id) {
        a->fg = inserer(a->fg, id, code, cap, flux);
    } else if (id > a->id) {
        a->fd = inserer(a->fd, id, code, cap, flux);
    } else {
        // ID existant : on ajoute la consommation au cumul
        a->conso += flux; 
        return a;
    }

    // 2. Mise à jour de la hauteur
    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));

    // 3. Équilibrage AVL
    int bal = equilibre(a);

    // Cas Gauche-Gauche
    if (bal > 1 && id < a->fg->id)
        return rotationDroite(a);

    // Cas Droite-Droite
    if (bal < -1 && id > a->fd->id)
        return rotationGauche(a);

    // Cas Gauche-Droite
    if (bal > 1 && id > a->fg->id)
        return doubleRotationGD(a);

    // Cas Droite-Gauche
    if (bal < -1 && id < a->fd->id)
        return doubleRotationDG(a);

    return a;
}

// --- Parcours et Sortie ---

void infixe(pStation a, FILE* fs) {
    if (a != NULL) {
        infixe(a->fg, fs);
        // Format de sortie demandé (ID:CAP:CONSO)
        // Modifiez cette ligne si votre sujet demande un format différent (ex: ID;CAP;CONSO)
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
