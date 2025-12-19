#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "avl.h"

// Fonction utilitaire pour comparer des chaînes
int estEgal(const char* s1, const char* s2) {
    return strcmp(s1, s2) == 0;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        return 1; 
    }

    char* fichier_in = argv[1];
    char* commande = argv[2]; 
    char* mode = argv[3]; // mode contient "max"/"src"/"real" OU l'ID pour leaks

    pStation arbre = NULL;

    // 1. Ouverture Fichier Sortie
    FILE* f_out = fopen("stats.csv", "w");
    if (f_out == NULL) return 1;

    // 2. Traitement selon la commande
    if (estEgal(commande, "histo")) {
        // Écriture En-têtes Histo
        if (estEgal(mode, "max")) {
            fprintf(f_out, "identifier;max volume (k.m3.year-1)\n");
        } 
        else if (estEgal(mode, "src")) {
            fprintf(f_out, "identifier;source volume (k.m3.year-1)\n");
        } 
        else if (estEgal(mode, "real")) {
            fprintf(f_out, "identifier;real volume (k.m3.year-1)\n");
        }
        
        // Chargement et Traitement via AVL
        charger(fichier_in, &arbre, mode);
        infixe(arbre, f_out);
        liberer(arbre);
    }
    else if (estEgal(commande, "leaks")) {
        // Écriture En-tête Leaks
        fprintf(f_out, "identifier;Leak volume (M.m3.year-1)\n");
        
        // Appel de la fonction linéaire (pas d'AVL ici)
        traiter_fuites(fichier_in, mode, f_out);
    }

    fclose(f_out);
    return 0;
}
