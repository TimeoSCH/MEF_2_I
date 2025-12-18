#include "avl.h"

// Fonctions utilitaires internes (remplacent string.h) 

// Remplace strcmp : renvoie un nb < 0, 0 ou > 0
int comparerTexte(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return (s1[i] - s2[i]); // Retourne la différence ASCII
        }
        i++;
    }
    return (s1[i] - s2[i]);
}

// Remplace strcpy
void copierTexte(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}



// Utile pour les hauteurs
int max(int a, int b) {
    if (a > b) return a;
    return b;
}

// Récupère la hauteur d'un noeud (gère le cas NULL)
int hauteur(pStation a) {
    if (a == NULL) return 0;
    return a->h;
}

// Calcul du facteur d'équilibre : h(fg) - h(fd)
int equilibre(pStation a) {
    if (a == NULL) return 0;
    return hauteur(a->fg) - hauteur(a->fd);
}

// Rotations Simples 

pStation rotationDroite(pStation a) {
    pStation pivot = a->fg;
    a->fg = pivot->fd;
    pivot->fd = a;
    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));
    pivot->h = 1 + max(hauteur(pivot->fg), hauteur(pivot->fd));
    return pivot;
}

// Par symétrie, voici la rotation Gauche adaptée
pStation rotationGauche(pStation a) {
    pStation pivot = a->fd;
    a->fd = pivot->fg;
    pivot->fg = a;
    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));
    pivot->h = 1 + max(hauteur(pivot->fg), hauteur(pivot->fd));
    return pivot;
}

//  Rotations Doubles 
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


//  Gestion de l'arbre 

pStation creerStation(int id, char* code, long cap) {
    pStation nouv = (pStation)malloc(sizeof(Station));
    if (nouv == NULL) {
        exit(1); // Erreur alloc
    }
    nouv->id = id;
    
    // Remplacement de strcpy par notre boucle
    copierTexte(nouv->id_str, code);
    
    nouv->capacite = cap;
    nouv->conso = 0;
    nouv->h = 1;
    nouv->fg = NULL;
    nouv->fd = NULL;
    return nouv;
}

pStation equilibrerAVL(pStation a) {
    // 1. Mise à jour de la hauteur
    a->h = 1 + max(hauteur(a->fg), hauteur(a->fd));

    // 2. Calcul du facteur d'équilibre
    int eq = equilibre(a); 

    // 3. Tests et Rotations
    if (eq >= 2) { 
        // L'arbre penche à GAUCHE (eq positif car hauteur(fg) > hauteur(fd))
        if (equilibre(a->fg) >= 0) {
            return rotationDroite(a);
        } else {
            return doubleRotationGD(a);
        }
    } 
    else if (eq <= -2) {
        // L'arbre penche à DROITE (eq négatif)
        if (equilibre(a->fd) <= 0) {
            return rotationGauche(a);
        } else {
            return doubleRotationDG(a);
        }
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
        // Mise à jour (INCHANGÉE)
        if (cap > 0) a->capacite = cap;
        a->conso += flux;
        return a; 
    }
    return equilibrerAVL(a);
}

void traiter(pStation a, FILE* fs) {
    if (a != NULL && fs != NULL) {
        fprintf(fs, "%s;%ld;%ld\n", a->id_str, a->capacite, a->conso);
    }
}

// Parcours infixe 
void infixe(pStation a, FILE* fs) {
    if (a != NULL) {
        infixe(a->fd, fs);       
        traiter(a, fs);         
        infixe(a->fg, fs);       
    }
}

// Fonction de libération de la mémoire
void liberer(pStation a) {
    if (a != NULL) {
        liberer(a->fg);
        liberer(a->fd);
        free(a);
    }
}



