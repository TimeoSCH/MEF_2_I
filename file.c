#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

int estEgal(const char* s1, const char* s2) {
    if (!s1 || !s2) return 0;
    return strcmp(s1, s2) == 0;
}

void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        perror("Erreur ouverture fichier");
        exit(1);
    }

    char ligne[MAX_LIGNE];
    // On suppose que le fichier d'entrée n'a pas d'en-tête ou qu'il est déjà filtré par le shell
    // Si le shell ne supprime pas l'en-tête, décommentez la ligne suivante :
    // fgets(ligne, MAX_LIGNE, fp); 

    while (fgets(ligne, MAX_LIGNE, fp)) {
        // Nettoyage fin de ligne
        char *p = ligne;
        while (*p) { if (*p == '\r' || *p == '\n') *p = '\0'; p++; }

        // Parsing manuel simple (strtok est aussi possible mais gardons votre logique)
        char *cols[4] = {NULL, NULL, NULL, NULL}; // ID, CAP, CONSO
        int i = 0;
        char *token = strtok(ligne, ";");
        
        while(token != NULL && i < 3) {
            cols[i++] = token;
            token = strtok(NULL, ";");
        }

        // Mapping des colonnes selon le Shell
        // Le shell envoie : ID;CAP;CONSO
        // cols[0] = ID (String)
        // cols[1] = CAP (Nombre ou 0)
        // cols[2] = CONSO (Nombre ou 0)

        if (cols[0] && cols[1] && cols[2]) {
            char* id_str = cols[0];
            long cap = atol(cols[1]); // atol pour long
            long conso = atol(cols[2]);

            // Appel corrigé à inserer
            *racine = inserer(*racine, id_str, cap, conso);
        }
    }
    fclose(fp);
}
