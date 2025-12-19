#include <stdio.h>
#include <stdlib.h>
#include "avl.h"

// --- Fonctions utilitaires ---

int comparerTexte(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return (s1[i] - s2[i]);
        i++;
    }
    return (s1[i] - s2[i]);
}

void copierTexte(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int max(int a, int b) {
    if (a > b) return a;
    return b;
}

int hauteur(pStation a) {
    if (a == NULL) return 0;
    return a->h;
}

int equilibre(pStation a) {
    if (a == NULL) return 0;
    return hauteur(a->fg) - hauteur(a->fd);
}

// --- Rotations ---

pStation rotationDroite(pStation a) {
    pStation pivot = a->fg;
    a->fg = pivot->fd;
    pivot->fd = a;
    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));
    pivot->h = 1 + max(hauteur(pivot->fg), hauteur(pivot->fd));
    return pivot;
}

pStation rotationGauche(pStation a) {
    pStation pivot = a->fd;
    a->fd = pivot->fg;
    pivot->fg = a;
    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));
    pivot->h = 1 + max(hauteur(pivot->fg), hauteur(pivot->fd));
    return pivot;
}

pStation doubleRotationGD(pStation a){
   a->fg=rotationGauche(a->fg);
   a=rotationDroite(a);
   return a;
}

pStation doubleRotationDG(pStation a){
   a->fd=rotationDroite(a->fd);
   a=rotationGauche(a);
   return a;
}

// --- Gestion AVL ---

pStation creerStation(int id, char* code, long cap) {
    pStation nouv = (pStation)malloc(sizeof(Station));
    if (nouv == NULL) exit(1);
    
    nouv->id = id;
    copierTexte(nouv->id_str, code);
    nouv->capacite = cap;
    nouv->conso = 0;
    nouv->h = 1;
    nouv->fg = NULL;
    nouv->fd = NULL;
    return nouv;
}

pStation equilibrerAVL(pStation a) {
    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));
    int eq = equilibre(a); 

    if (eq >= 2) { 
        if (equilibre(a->fg) >= 0) return rotationDroite(a);
        else return doubleRotationGD(a);
    } 
    else if (eq <= -2) {
        if (equilibre(a->fd) <= 0) return rotationGauche(a);
        else return doubleRotationDG(a);
    }
    return a;
}

pStation inserer(pStation a, int id, char* code, long cap, long flux) {
    if (a == NULL) {
        pStation nouv = creerStation(id, code, cap);
        nouv->conso = flux;
        return nouv;
    }

    int cmp = comparerTexte(code, a->id_str);

    if (cmp < 0) {
        a->fg = inserer(a->fg, id, code, cap, flux);
    } 
    else if (cmp > 0) {
        a->fd = inserer(a->fd, id, code, cap, flux);
    } 
    else {
        if (cap > 0) a->capacite = cap;
        a->conso += flux;
        return a; 
    }
    return equilibrerAVL(a);
}

void traiter(pStation a, FILE* fs) {
    if (a != NULL && fs != NULL) {
        long valeur_finale = 0;
        // On choisit la bonne valeur
        if (a->capacite > 0) valeur_finale = a->capacite;
        else valeur_finale = a->conso;

        fprintf(fs, "%s;%ld\n", a->id_str, valeur_finale);
    }
}

// --- PARCOURS INVERSÉ (DÉCROISSANT) ---
void infixe(pStation a, FILE* fs) {
    if (a != NULL) {
        // On visite d'abord la DROITE (les plus grands)
        infixe(a->fd, fs);       
        
        // Puis le noeud courant
        traiter(a, fs);         
        
        // Puis la GAUCHE (les plus petits)
        infixe(a->fg, fs);       
    }
}

void liberer(pStation a) {
    if (a != NULL) {
        liberer(a->fg);
        liberer(a->fd);
        free(a);
    }
}

