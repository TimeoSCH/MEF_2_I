#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "avl.h"
#include "leak.h" 

// Fonction pour comparer des chaînes
int estEgalMain(const char* s1, const char* s2) {
    return strcmp(s1, s2) == 0;
}

int main(int argc, char** argv) {
    if (argc < 4) return 1; 

    char* fichier_in = argv[1];
    char* commande = argv[2]; 
    char* mode = argv[3]; 

    pStation arbre = NULL;

    FILE* f_out = fopen("stats.csv", "w");
    if (f_out == NULL) return 1;

    if (estEgalMain(commande, "histo")) {
        if (estEgalMain(mode, "max")) fprintf(f_out, "identifier;max volume (k.m3.year-1)\n");
        else if (estEgalMain(mode, "src")) fprintf(f_out, "identifier;source volume (k.m3.year-1)\n");
        else if (estEgalMain(mode, "real")) fprintf(f_out, "identifier;real volume (k.m3.year-1)\n");
        
        charger(fichier_in, &arbre, mode);
        infixe(arbre, f_out);
        liberer(arbre);
    }
    else if (estEgalMain(commande, "leaks")) {
        fprintf(f_out, "identifier;Leak volume (M.m3.year-1)\n");
        
        traiter_fuites(fichier_in, mode, f_out);
    }

    fclose(f_out);
    return 0;
}
