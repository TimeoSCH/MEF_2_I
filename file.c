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

    while (fgets(ligne, MAX_LIGNE, fp)) {
        // Nettoyage des retours à la ligne
        ligne[strcspn(ligne, "\r\n")] = 0;
        if (strlen(ligne) == 0){
            continue;
        }

        // Découpage manuel des colonnes 
        char *cols[6] = {NULL}; 
        char *ptr = ligne;
        int i = 0;

        while (ptr != NULL && i < 6) {
            cols[i] = ptr;
            char *sep = strchr(ptr, ';');
            if (sep != NULL) {
                *sep = '\0'; // On remplace le séparateur par fin de chaine
                ptr = sep + 1;
            } else {
                ptr = NULL;
            }
            i++;
        }
        
        // Vérification de sécurité : besoin d'au moins 5 colonnes
        if (i < 5){
            continue;
        }

        char* id_amont = cols[1]; 
        char* id_aval = cols[2];  
        char* val_vol = cols[3]; 
        char* val_fuite = cols[4];

        // Logique selon le mode choisi 

        if (estEgal(mode, "max")) {
            // Mode MAX : On cherche les lignes définissant une usine
            if (estEgal(id_aval, "-") && !estEgal(id_amont, "-")) {
                long cap = atol(val_vol);
                if (cap > 0) {
                    *racine = inserer(*racine, id_amont, cap, 0);
                }
            }
        }
        
        else if (estEgal(mode, "src") || estEgal(mode, "real")) {
            // Critère : Amont et Aval existent
            if (!estEgal(id_aval, "-") && !estEgal(id_amont, "-")) {

                char* id_usine = id_aval; // L'usine est en aval
                long volume = atol(val_vol);

                if (volume > 0) {
                    if (estEgal(mode, "src")) {
                        // On somme le volume capté brut
                        *racine = inserer(*racine, id_usine, 0, volume);
                    } 
                    else if (estEgal(mode, "real")) {
                        // On somme le volume réel après déduction des fuites
                        double fuite_pct = 0.0;
                        if (!estEgal(val_fuite, "-")) {
                            fuite_pct = atof(val_fuite);
                        }
                        long vol_reel = (long)(volume * (1.0 - (fuite_pct / 100.0)));
                        *racine = inserer(*racine, id_usine, 0, vol_reel);
                    }
                }
            }
        }
    }
    fclose(fp);
}
