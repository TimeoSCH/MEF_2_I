#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Pour isdigit
#include "avl.h"
#include "file.h"

#define MAX_LIGNE 2048

// --- Implémentation de la fonction déclarée dans file.h ---
int estEgal(const char* s1, const char* s2) {
    if (s1 == NULL || s2 == NULL) return 0;
    return strcmp(s1, s2) == 0;
}

// Fonction utilitaire locale pour nettoyer une chaîne (enlève \r, \n, espaces)
void nettoyer(char* s) {
    char* p = s;
    // Avancer tant que c'est un espace au début
    while (*p && isspace(*p)) p++;
    if (p != s) memmove(s, p, strlen(p) + 1);
    
    // Enlever la fin (espaces, retours ligne)
    size_t len = strlen(s);
    while (len > 0 && isspace(s[len-1])) {
        s[len-1] = '\0';
        len--;
    }
}

long chaineVersLong(const char* s) {
    if (s == NULL || estEgal(s, "-")) return 0;
    return atol(s);
}

void copierChaine(char* dest, const char* src) {
    strcpy(dest, src);
}

// --- FONCTION PRINCIPALE : CHARGEMENT ---
void charger(char* chemin, pStation* racine, char* mode) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        perror("Erreur ouverture fichier");
        exit(1);
    }

    char ligne[MAX_LIGNE];
    
    // Ignorer l'entête
    fgets(ligne, MAX_LIGNE, fp);

    int nb_lignes_lues = 0;

    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        nb_lignes_lues++;
        
        // Nettoyage de la ligne brute (enlève le \n et \r final)
        nettoyer(ligne);
        if (strlen(ligne) == 0) continue; // Sauter lignes vides

        char cols[6][50]; // On prévoit large (6 colonnes)
        // Init à "-"
        for(int k=0; k<6; k++) strcpy(cols[k], "-");

        // --- Parsing avec strtok est dangereux pour les champs vides (;;) ---
        // --- On utilise un parsing manuel robuste ---
        
        int idxCol = 0;
        char* ptr = ligne;
        char* debut = ligne;
        
        while (*ptr != '\0' && idxCol < 6) {
            if (*ptr == ';') {
                *ptr = '\0'; // Coupe la chaîne ici
                
                if (strlen(debut) > 0) strcpy(cols[idxCol], debut);
                else strcpy(cols[idxCol], "-");
                
                debut = ptr + 1; // Le prochain mot commence après le ;
                idxCol++;
            }
            ptr++;
        }
        // Récupérer la dernière colonne (après le dernier ;)
        if (idxCol < 6) {
            if (strlen(debut) > 0) strcpy(cols[idxCol], debut);
            else strcpy(cols[idxCol], "-");
        }

        // --- Conversion ---
        // Format supposé : HVB(0); HVA(1); LV(2); Cap(3); Load(4)
        
        long val4 = chaineVersLong(cols[3]); // Capacité
        long val5 = chaineVersLong(cols[4]); // Consommation

        // --- DEBUG TEMPORAIRE (Afficher les 3 premières lignes parsées) ---
        // Cela s'affichera dans le terminal lors de l'exécution
        if (nb_lignes_lues <= 3) {
            fprintf(stderr, "[DEBUG] Ligne %d -> Cols: '%s' '%s' '%s' | Cap: %ld | Load: %ld\n", 
                    nb_lignes_lues, cols[0], cols[1], cols[2], val4, val5);
        }

        // --- LOGIQUE METIER ---

        if (estEgal(mode, "real")) {
            // Consommateurs (LV) : Colonne 2
            if (val5 > 0 && !estEgal(cols[2], "-")) {
                int id = (int)chaineVersLong(cols[2]);
                *racine = inserer(*racine, id, cols[2], 0, val5);
            }
        }
        else if (estEgal(mode, "src")) {
            // Sources : HVB (0) ou HVA (1)
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
            // HVA (1)
            if (val4 > 0 && !estEgal(cols[1], "-")) {
                int id = (int)chaineVersLong(cols[1]);
                *racine = inserer(*racine, id, cols[1], val4, 0);
            }
        }
    }

    fclose(fp);
}
