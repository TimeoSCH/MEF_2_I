#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak.h"

#define MAX_LIGNE 2048

void traiter_fuites(char* chemin, char* station_id, FILE* f_out) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        perror("Erreur ouverture fichier");
        exit(1);
    }
    
    char ligne[MAX_LIGNE];
    
    if (fgets(ligne, MAX_LIGNE, fp) == NULL) {
        fclose(fp);
        return;
    }

    long capacite_usine = 0;
    long total_conso_clients = 0;
    int station_trouvee = 0;

    
    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        
       
        ligne[strcspn(ligne, "\r\n")] = 0;
        
        char *ptr = ligne;
        char *cols[6] = {NULL}; 
        int i = 0;

        while (ptr != NULL && i < 6) {
            cols[i] = ptr;
            char *sep = strchr(ptr, ';');
            if (sep != NULL) {
                *sep = '\0'; 
                ptr = sep + 1;
            } else {
                ptr = NULL;
            }
            i++;
        }

        // Vérification qu'on a bien lu assez de colonnes
        if (i >= 5) {
            char* col_hvb = cols[0];
            char* col_hva = cols[1];
            char* col_cap = cols[3]; 
            char* col_conso = cols[4];

            if ((strcmp(col_hvb, station_id) == 0) || (strcmp(col_hva, station_id) == 0)) {
                station_trouvee = 1;

                // atol renvoie 0 si la chaine est vide 
                long val_cap = atol(col_cap);
                long val_conso = atol(col_conso);

                if (val_cap > 0){
                    capacite_usine += val_cap;
                }
                if (val_conso > 0){
                    total_conso_clients += val_conso;
                }
            }
        }
    }
    fclose(fp);

    // Écriture du résultat
    if (station_trouvee == 0) {
        // La station n'existe pas dans les données
        fprintf(f_out, "%s;-1\n", station_id);
    } else {
        long fuites = capacite_usine - total_conso_clients;
        fprintf(f_out, "%s;%ld\n", station_id, fuites);
    }
}
