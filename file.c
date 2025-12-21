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
    if (!fgets(ligne, MAX_LIGNE, fp)) {
        fclose(fp);
        return;
    }

    while (fgets(ligne, MAX_LIGNE, fp)) {
        char *p = ligne;
        while (*p) { 
            if (*p == '\r' || *p == '\n'){
                *p = '\0'; p++;
            }
        }

        char *cols[12]; 
        int col_count = 0;
        char *ptr = ligne;

        // Découpage manuel
        while (ptr != NULL && col_count < 12) {
            char *next = strchr(ptr, ';');
            if (next) *next = '\0';
            cols[col_count++] = ptr;
            ptr = next ? (next + 1) : NULL;
        }

        if (estEgal(mode, "real")) {
            // VERIFICATION : La colonne 3 (LV) doit exister et ne pas être '-'
            if (col_count > 5 && !estEgal(cols[3], "-") && strlen(cols[3]) > 0) {
                int id = atoi(cols[3]);
                long load = atol(cols[5]);
                if (load > 0) {
                    *racine = inserer(*racine, id, cols[3], 0, load);
                }
            }
        } 
        else if (estEgal(mode, "max") || estEgal(mode, "hva") || estEgal(mode, "hvb")) {
            char* id_str = "-";
            if (col_count > 2 && !estEgal(cols[2], "-")){
                id_str = cols[2];
            }
            else if (col_count > 1 && !estEgal(cols[1], "-")){
                id_str = cols[1];
            }

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
