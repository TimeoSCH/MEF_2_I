Si le traitement "real" prend trop de temps, c'est normal : il y a des millions de consommateurs dans le fichier, alors qu'il n'y a que quelques centaines de centrales pour max et src. Ton programme doit construire un arbre immense.

Cependant, on peut accélérer le programme x10 en activant l'optimisation maximale du compilateur (-O3) et en utilisant une astuce de mémoire tampon.

Voici les 2 étapes pour régler la lenteur :
Étape 1 : Optimiser la lecture du fichier (file.c)

La lecture ligne par ligne est lente. On va ajouter une ligne pour lire par gros blocs (Buffer). Copie ce code optimisé dans file.c.
C

#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

// --- Fonctions utilitaires ---
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

// --- CHARGEMENT OPTIMISÉ ---
void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) exit(1);

    // --- ACCELERATION DISQUE (BUFFER) ---
    // On lit par blocs de 16Ko au lieu de faire des aller-retours disque
    char buffer[16384];
    setvbuf(fp, buffer, _IOFBF, sizeof(buffer));

    char ligne[MAX_LIGNE];
    fgets(ligne, MAX_LIGNE, fp); // Sauter entête

    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        char cols[5][50]; 
        char tampon[50];  
        int idxLigne = 0, idxCol = 0, idxTampon = 0;
        
        for(int k=0; k<5; k++) cols[k][0] = '\0';

        // Parsing manuel
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

        long val4 = chaineVersLong(cols[3]); 
        long val5 = chaineVersLong(cols[4]); 

        // --- FILTRAGE RAPIDE ---
        if (estEgal(mode, "real")) {
            // Optimisation : On ne fait l'insertion que si c'est nécessaire
            if (val5 > 0 && estEgal(cols[2], "-") == 0) {
                 *racine = inserer(*racine, 0, cols[2], 0, val5);
            }
        }
        else if (estEgal(mode, "src")) {
            if (val4 > 0 && val5 == 0) { 
                if (estEgal(cols[0], "-") == 0) *racine = inserer(*racine, 0, cols[0], val4, 0);
                else if (estEgal(cols[1], "-") == 0) *racine = inserer(*racine, 0, cols[1], val4, 0);
            }
        }
        else if (estEgal(mode, "max")) {
            if (val4 > 0 && estEgal(cols[1], "-") == 0) {
                 *racine = inserer(*racine, 0, cols[1], val4, 0);
            }
        }
    }
    fclose(fp);
}
