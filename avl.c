#include "avl.h"

Arbre* creerArbre(Station* s) {
    Arbre* nouv = (Arbre*)malloc(sizeof(Arbre));
    if (nouv == NULL) {
        printf("Erreur d'allocation.\n");
        exit(1);
    }
    nouv->elmt = s;
    nouv->fg = NULL;
    nouv->fd = NULL;
    nouv->equilibre = 0;
    return nouv;
}
