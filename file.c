#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

// --- Fonctions utilitaires rapides ---

int estEgal(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return 0;
        i++;
    }
    return (s1[i] == '\0' && s2[i] == '\0');
}

void copierChaine(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') { dest[i] = src[i]; i++; }
    dest[i] = '\0';
}

long chaineVersLong(const char* s) {
    long res = 0;
    int i = 0;
    if (s[0] == '\0' || s[0] == '-') return 0;
    while (s[i] >= '0' && s[i] <= '9') {
        res = res * 10 + (s[i] - '0');
        i++;
    }
    return res;
}

// --- CHARGEMENT RAPIDE ---
void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) exit(1);

    char ligne[MAX_LIGNE];
    // Sauter l'entête
    fgets(ligne, MAX_LIGNE, fp);

    // Boucle de lecture
    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        
        char cols[5][50]; 
        char tampon[50];  
        int idxLigne = 0, idxCol = 0, idxTampon = 0;
        
        // Initialisation rapide
        for(int k=0; k<5; k++) cols[k][0] = '\0';

        // Parsing manuel (Plus rapide que strtok)
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

        // Conversion
        long val4 = chaineVersLong(cols[3]); // Capacité
        long val5 = chaineVersLong(cols[4]); // Consommation

        // --- FILTRAGE OPTIMISÉ ---

        // MODE SRC : Centrales (Capacité > 0 et Pas de Conso)
        if (estEgal(mode, "src")) {
            if (val4 > 0 && val5 == 0) { 
                if (estEgal(cols[0], "-") == 0) *racine = inserer(*racine, 0, cols[0], val4, 0);
                else if (estEgal(cols[1], "-") == 0) *racine = inserer(*racine, 0, cols[1], val4, 0);
            }
        }
        // MODE REAL : Consommateurs (Conso > 0)
        else if (estEgal(mode, "real")) {
            if (val5 > 0) {
                if (estEgal(cols[2], "-") == 0) *racine = inserer(*racine, 0, cols[2], 0, val5);
            }
        }
        // MODE MAX : Station HVA (Capacité > 0 et Col 1 existe)
        else if (estEgal(mode, "max")) {
            if (val4 > 0 && estEgal(cols[1], "-") == 0) {
                 *racine = inserer(*racine, 0, cols[1], val4, 0);
            }
        }
    }
    fclose(fp);
}
