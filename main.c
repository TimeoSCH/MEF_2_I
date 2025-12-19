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

    // Pointeur vers la racine de l'arbre
    pStation arbre = NULL;

    // Sortie standard (stdout) pour redirection par le script Shell
    FILE* f_out = stdout; 

    // --- Traitement : HISTO ---
    if (estEgal(commande, "histo")) {
        // En-têtes pour les graphiques
        if (estEgal(mode, "max")) {
            fprintf(f_out, "Station:Capacite:Consommation\n");
        } 
        else if (estEgal(mode, "real")) {
            fprintf(f_out, "Station:Capacite:Consommation\n");
        }
        else {
            fprintf(f_out, "Station:Capacite:Consommation\n");
        }
        
        // Chargement (file.c)
        charger(fichier_in, &arbre, mode);
        
        // Écriture (avl.c)
        infixe(arbre, f_out);
        
        // Nettoyage
        liberer(arbre);
    }
    // --- Traitement : LEAKS ---
    else if (estEgal(commande, "leaks")) {
        fprintf(f_out, "Station:Fuite\n");
        
        // --- CORRECTION ICI ---
        // On ajoute f_out comme 3ème argument
        traiter_fuites(fichier_in, mode, f_out); 
    }
    else {
        fprintf(stderr, "Commande inconnue : %s\n", commande);
        return 2;
    }

    return 0;
}
