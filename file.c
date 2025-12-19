#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

int estEgal(const char* s1, const char* s2) {
    if (!s1 || !s2) return 0;
    return strcmp(s1, s2) == 0;
}

// Fonction pour nettoyer les résidus de fin de ligne (\r \n)
void clean_line(char* s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r' || s[len-1] == ' ')) {
        s[len-1] = '\0';
        len--;
    }
}

void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        perror("Erreur ouverture fichier");
        exit(1);
    }

    char ligne[MAX_LIGNE];
    // Ignorer l'entête
    if (!fgets(ligne, MAX_LIGNE, fp)) {
        fclose(fp);
        return;
    }

    while (fgets(ligne, MAX_LIGNE, fp)) {
        clean_line(ligne);
        if (strlen(ligne) == 0) continue;

        char *cols[10]; // On prévoit jusqu'à 10 colonnes
        int col_count = 0;
        char *ptr = ligne;
        
        // --- PARSING MANUEL ROBUSTE ---
        cols[col_count++] = ptr;
        while (*ptr) {
            if (*ptr == ';') {
                *ptr = '\0';
                cols[col_count++] = ptr + 1;
            }
            ptr++;
            if (col_count >= 10) break;
        }

        // Vérification du nombre de colonnes minimum (on attend au moins 5)
        if (col_count < 5) continue;

        // cols[0]: Power plant
        // cols[1]: HVB station
        // cols[2]: HVA station
        // cols[3]: LV station  <-- C'est celle-ci pour le mode 'real'
        // cols[4]: Capacity
        // cols[5]: Load

        if (estEgal(mode, "real")) {
            // Dans le mode real, l'identifiant de la station LV est souvent en colonne 3 (index 3)
            // ou colonne 2 (index 2). Vérifions l'index 2 et 3.
            char* id_str = cols[3]; // LV Station
            long load = atol(cols[5]); // Load est souvent en colonne 5 ou 6

            if (!estEgal(id_str, "-") && load > 0) {
                int id = atoi(id_str);
                *racine = inserer(*racine, id, id_str, 0, load);
            }
        } 
        else if (estEgal(mode, "max") || estEgal(mode, "src")) {
            char* id_str = cols[1]; // HVA ou HVB
            long cap = atol(cols[4]);
            if (!estEgal(id_str, "-") && cap > 0) {
                int id = atoi(id_str);
                *racine = inserer(*racine, id, id_str, cap, 0);
            }
        }
    }
    fclose(fp);
}
