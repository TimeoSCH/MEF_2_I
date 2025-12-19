#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "file.h" // Inclure le header pour valider les prototypes

#define MAX_LIGNE 2048

// --- Implémentation de la fonction déclarée dans file.h ---
int estEgal(const char* s1, const char* s2) {
    return strcmp(s1, s2) == 0;
}

// Fonction utilitaire locale (non exposée dans .h)
void copierChaine(char* dest, const char* src) {
    strcpy(dest, src);
}

// Fonction utilitaire locale
long chaineVersLong(const char* s) {
    return atol(s);
}

// --- FONCTION PRINCIPALE : CHARGEMENT ---
void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        perror("Erreur ouverture fichier");
        exit(1);
    }

    char buffer[16384];
    setvbuf(fp, buffer, _IOFBF, sizeof(buffer));

    char ligne[MAX_LIGNE];
    
    // Ignorer l'entête
    fgets(ligne, MAX_LIGNE, fp);

    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        char cols[5][50]; 
        char tampon[50];  
        int idxLigne = 0, idxCol = 0, idxTampon = 0;

        for(int k=0; k<5; k++) cols[k][0] = '\0';

        while (ligne[idxLigne] != '\0' && idxCol < 5) {
            char c = ligne[idxLigne];
            if (c == ';' || c == '\n' || c == '\r') {
                tampon[idxTampon] = '\0';
                if (idxTampon == 0) copierChaine(cols[idxCol], "-");
                else copierChaine(cols[idxCol], tampon);
                idxCol++;
                idxTampon = 0;
            } else {
                tampon[idxTampon] = c;
                idxTampon++;
            }
            idxLigne++;
        }

        long val4 = chaineVersLong(cols[3]); // Cap
        long val5 = chaineVersLong(cols[4]); // Load

        // Utilisation de estEgal défini plus haut
        if (estEgal(mode, "real")) {
            if (val5 > 0 && !estEgal(cols[2], "-")) {
                int id = (int)chaineVersLong(cols[2]);
                *racine = inserer(*racine, id, cols[2], 0, val5);
            }
        }
        else if (estEgal(mode, "src")) {
            if (val4 > 0 && val5 == 0) {
                if (!estEgal(cols[0], "-")) {
                    int id = (int)chaineVersLong(cols[0]);
                    *racine = inserer(*racine, id, cols[0], val4, 0);
                }
                else if (!estEgal(cols[1], "-")) {
                    int id = (int)chaineVersLong(cols[1]);
                    *racine = inserer(*racine, id, cols[1], val4, 0);
                }
            }
        }
        else if (estEgal(mode, "max")) {
            if (val4 > 0 && !estEgal(cols[1], "-")) {
                int id = (int)chaineVersLong(cols[1]);
                *racine = inserer(*racine, id, cols[1], val4, 0);
            }
        }
    }
    fclose(fp);
}
