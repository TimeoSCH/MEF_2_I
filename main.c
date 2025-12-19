#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "file.h" // Fournit maintenant 'estEgal' et 'charger'
#include "leak.h"

int main(int argc, char** argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <fichier_csv> <commande> <mode>\n", argv[0]);
        return 1;
    }

    char* fichier_in = argv[1];
    char* commande = argv[2]; 
    char* mode = argv[3]; 

    pStation arbre = NULL;
    FILE* f_out = stdout; 

    // estEgal vient maintenant de file.h/file.c
    if (estEgal(commande, "histo")) {
        // En-têtes CSV séparés par ':' pour correspondre au format de sortie AVL
        if (estEgal(mode, "max"))      fprintf(f_out, "Station:Capacite:Consommation\n");
        else if (estEgal(mode, "real")) fprintf(f_out, "Station:Capacite:Consommation\n");
        else                           fprintf(f_out, "Station:Capacite:Consommation\n");
        
        charger(fichier_in, &arbre, mode);
        infixe(arbre, f_out);
        liberer(arbre);
    }
    else if (estEgal(commande, "leaks")) {
        fprintf(f_out, "Station:Fuite\n");
        traiter_fuites(fichier_in, mode, f_out); 
    }
    else {
        fprintf(stderr, "Commande inconnue : %s\n", commande);
        return 2;
    }

    return 0;
}
