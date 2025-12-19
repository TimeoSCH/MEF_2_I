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
    // Sauter l'entête
    if (!fgets(ligne, MAX_LIGNE, fp)) {
        fclose(fp);
        return;
    }

    while (fgets(ligne, MAX_LIGNE, fp)) {
        // Nettoyage fin de ligne
        char *p = ligne;
        while (*p) { if (*p == '\r' || *p == '\n') *p = '\0'; p++; }

        char *cols[12]; 
        int col_count = 0;
        char *token = ligne;
        char *next_token;

        // Parsing manuel robuste pour gérer les champs vides (;;)
        while (token != NULL && col_count < 12) {
            next_token = strchr(token, ';');
            if (next_token) *next_token = '\0';
            cols[col_count++] = token;
            if (next_token) token = next_token + 1;
            else token = NULL;
        }

        // --- MAPPING DES COLONNES (Indices typiques) ---
        // Indice 0: Power Plant
        // Indice 1: HVB Station
        // Indice 2: HVA Station
        // Indice 3: LV Station  <-- ID pour le mode 'real'
        // Indice 4: Capacity
        // Indice 5: Load
        // Indice 6: ...

        if (estEgal(mode, "real")) {
            // On vérifie si la colonne LV (3) n'est pas vide
            if (col_count > 5 && !estEgal(cols[3], "-") && strlen(cols[3]) > 0) {
                int id = atoi(cols[3]);
                long load = atol(cols[5]);
                if (load > 0) {
                    *racine = inserer(*racine, id, cols[3], 0, load);
                }
            }
        } 
        else if (estEgal(mode, "max") || estEgal(mode, "src")) {
            // Pour HVA ou HVB, on regarde les colonnes 1 ou 2
            char* id_str = "-";
            if (!estEgal(cols[2], "-")) id_str = cols[2];
            else if (!estEgal(cols[1], "-")) id_str = cols[1];

            if (!estEgal(id_str, "-")) {
                int id = atoi(id_str);
                long cap = atol(cols[4]);
                if (cap > 0) {
                    *racine = inserer(*racine, id, id_str, cap, 0);
                }
            }
        }
    }
    fclose(fp);
}
