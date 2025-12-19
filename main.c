#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "file.h"
#include "leak.h"

// Fonction utilitaire pour comparer les chaînes
int estEgal(const char* s1, const char* s2) {
    return strcmp(s1, s2) == 0;
}

int main(int argc, char** argv) {
    // Vérification des arguments
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <fichier_csv> <commande> <mode>\n", argv[0]);
        return 1;
    }

    char* fichier_in = argv[1];
    char* commande = argv[2]; 
    char* mode = argv[3]; 

    pStation arbre = NULL;

    FILE* f_out = stdout; 

    if (estEgal(commande, "histo")) {
        if (estEgal(mode, "max")) {
            fprintf(f_out, "Station:Capacite:Consommation\n");
        } 
        else if (estEgal(mode, "real")) {
            fprintf(f_out, "Station:Capacite:Consommation\n");
        }
        else {
            // En-tête générique pour les autres modes
            fprintf(f_out, "Station:Capacite:Consommation\n");
        }
        
        // Chargement de l'arbre (fonction définie dans file.c)
        charger(fichier_in, &arbre, mode);
        
        // Écriture des données triées (fonction définie dans avl.c)
        infixe(arbre, f_out);
        
        // Nettoyage
        liberer(arbre);
    }
    // --- Traitement : LEAKS ---
    else if (estEgal(commande, "leaks")) {
        fprintf(f_out, "Station:Fuite\n");
    
        traiter_fuites(fichier_in, mode); 
    }
    else {
        fprintf(stderr, "Commande inconnue : %s\n", commande);
        return 2;
    }

    return 0;
}
