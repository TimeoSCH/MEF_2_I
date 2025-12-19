#include <stdio.h>
#include <stdlib.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

// --- Fonctions utilitaires manuelles ---

int estEgal(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return 0;
        i++;
    }
    if (s1[i] == '\0' && s2[i] == '\0') return 1;
    return 0;
}

void copierChaine(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
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

// --- Fonction principale de chargement ---

void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", chemin);
        exit(1);
    }

    char ligne[MAX_LIGNE];
    
    // Ignorer l'entête
    fgets(ligne, MAX_LIGNE, fp);

    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        
        char cols[5][50]; 
        char tampon[50];  
        
        int idxLigne = 0; 
        int idxCol = 0;   
        int idxTampon = 0;

        for(int k=0; k<5; k++) cols[k][0] = '\0';

        // --- Parsing Manuel ---
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

        // --- Conversion des valeurs ---
        // cols[3] = Capacité (Production) -> val4
        // cols[4] = Consommation (Load)   -> val5
        long val4 = chaineVersLong(cols[3]); 
        long val5 = chaineVersLong(cols[4]); 

        // --- LOGIQUE DE FILTRAGE CORRIGÉE ---

        // 1. MODE MAX (HVA) : On cherche la capacité des stations HVA
        if (estEgal(mode, "max")) {
            // Si c'est une station HVA (Col 1 remplie) et qu'elle a une capacité
            if (estEgal(cols[1], "-") == 0 && val4 > 0) {
                *racine = inserer(*racine, 0, cols[1], val4, 0);
            }
        }
        
        // 2. MODE SRC (Sources) : On cherche les centrales (Production)
        else if (estEgal(mode, "src")) {
            // Une source a une Capacité (val4 > 0) mais pas de Conso (val5 == 0)
            if (val4 > 0 && val5 == 0) {
                // Elle peut être HVB (Col 0) ou HVA (Col 1)
                if (estEgal(cols[0], "-") == 0) {
                    *racine = inserer(*racine, 0, cols[0], val4, 0);
                }
                else if (estEgal(cols[1], "-") == 0) {
                    *racine = inserer(*racine, 0, cols[1], val4, 0);
                }
            }
        }
        
        // 3. MODE REAL (Consommateurs) : On cherche ceux qui consomment
        else if (estEgal(mode, "real")) {
            // Un consommateur a une Consommation (val5 > 0)
            // Il est identifié par le Poste (Col 2)
            if (val5 > 0 && estEgal(cols[2], "-") == 0) {
                *racine = inserer(*racine, 0, cols[2], 0, val5);
            }
        }
    }

    fclose(fp);
}
