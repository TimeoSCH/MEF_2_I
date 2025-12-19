#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

// --- Fonctions utilitaires internes ---

// Renvoie 1 si les chaines sont identiques, 0 sinon
int estEgalFile(const char* s1, const char* s2) {
    return strcmp(s1, s2) == 0;
}

// Copie src dans dest
void copierChaine(char* dest, const char* src) {
    strcpy(dest, src);
}

// Convertit une chaine en entier long
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
        
        int idxLigne = 0; 
        int idxCol = 0;   
        int idxTampon = 0;

        // Init
        for(int k=0; k<5; k++) cols[k][0] = '\0';

        // Parsing manuel (CSV avec ;)
        while (ligne[idxLigne] != '\0' && idxCol < 5) {
            char c = ligne[idxLigne];

            if (c == ';' || c == '\n' || c == '\r') {
                tampon[idxTampon] = '\0';
                
                if (idxTampon == 0) copierChaine(cols[idxCol], "-");
                else copierChaine(cols[idxCol], tampon);

                idxCol++;
                idxTampon = 0;
            } 
            else {
                tampon[idxTampon] = c;
                idxTampon++;
            }
            idxLigne++;
        }

        // Col 3 = Capacité, Col 4 = Conso
        long val4 = chaineVersLong(cols[3]); // Capacité
        long val5 = chaineVersLong(cols[4]); // Consommation

        // --- LOGIQUE CORRIGÉE ---

        // 1. Mode REAL (Consommateurs - LV - Col 2)
        if (estEgalFile(mode, "real")) {
            // Si conso > 0 et identifiant LV existe
            if (val5 > 0 && !estEgalFile(cols[2], "-")) {
                int id = (int)chaineVersLong(cols[2]); // <--- CONVERSION ICI
                *racine = inserer(*racine, id, cols[2], 0, val5);
            }
        }
        // 2. Mode SRC (Centrales - HVB/Col 0 ou HVA/Col 1)
        else if (estEgalFile(mode, "src")) {
            // Sources : Capacité > 0, pas de conso directe
            if (val4 > 0 && val5 == 0) {
                if (!estEgalFile(cols[0], "-")) {
                    int id = (int)chaineVersLong(cols[0]); // <--- CONVERSION ICI
                    *racine = inserer(*racine, id, cols[0], val4, 0);
                }
                else if (!estEgalFile(cols[1], "-")) {
                    int id = (int)chaineVersLong(cols[1]); // <--- CONVERSION ICI
                    *racine = inserer(*racine, id, cols[1], val4, 0);
                }
            }
        }
        // 3. Mode MAX (Usines - HVA - Col 1)
        else if (estEgalFile(mode, "max")) {
            // HVA saturées (Capacity > 0 et HVA existe)
            if (val4 > 0 && !estEgalFile(cols[1], "-")) {
                int id = (int)chaineVersLong(cols[1]); // <--- CONVERSION ICI
                // Note : Pour "max", on stocke la capacité comme "conso" pour le calcul ? 
                // Ou on stocke juste la capacité. Ici je stocke capacité.
                *racine = inserer(*racine, id, cols[1], val4, 0);
            }
        }
    }

    fclose(fp);
}
