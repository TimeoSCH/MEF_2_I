#include <stdio.h>
#include <stdlib.h>
#include "leaks.h"

#define MAX_LIGNE 2048

// --- Fonctions Utilitaires Internes (Privées à ce fichier) ---

// Comparaison de chaînes
int estEgal_leaks(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return 0;
        i++;
    }
    return (s1[i] == '\0' && s2[i] == '\0');
}

// Copie de chaînes
void copierChaine_leaks(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Conversion String -> Long
long chaineVersLong_leaks(const char* s) {
    long res = 0;
    int i = 0;
    if (s[0] == '\0' || s[0] == '-') return 0;
    while (s[i] >= '0' && s[i] <= '9') {
        res = res * 10 + (s[i] - '0');
        i++;
    }
    return res;
}

// --- Fonction Principale LEAKS ---

void traiter_fuites(char* chemin, char* station_id, FILE* f_out) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", chemin);
        exit(1);
    }

    // Buffer pour lecture rapide
    char buffer[16384];
    setvbuf(fp, buffer, _IOFBF, sizeof(buffer));

    char ligne[MAX_LIGNE];
    fgets(ligne, MAX_LIGNE, fp); // Sauter l'entête

    long capacite_usine = 0;
    long total_conso_clients = 0;
    int station_trouvee = 0;

    // Parcours linéaire du fichier
    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        
        char cols[5][50]; 
        char tampon[50];  
        int idxLigne = 0, idxCol = 0, idxTampon = 0;
        
        for(int k=0; k<5; k++) cols[k][0] = '\0';

        // Parsing
        while (ligne[idxLigne] != '\0' && idxCol < 5) {
            char c = ligne[idxLigne];
            if (c == ';' || c == '\n' || c == '\r') {
                tampon[idxTampon] = '\0';
                if (idxTampon == 0) copierChaine_leaks(cols[idxCol], "-");
                else copierChaine_leaks(cols[idxCol], tampon);
                idxCol++;
                idxTampon = 0;
            } else {
                tampon[idxTampon] = c;
                idxTampon++;
            }
            idxLigne++;
        }

        long val4 = chaineVersLong_leaks(cols[3]); // Capacité
        long val5 = chaineVersLong_leaks(cols[4]); // Consommation

        // Logique : On cherche la station dans HVB (col 0) ou HVA (col 1)
        if (estEgal_leaks(cols[0], station_id) || estEgal_leaks(cols[1], station_id)) {
            station_trouvee = 1;

            if (val4 > 0) capacite_usine += val4;       // Production
            if (val5 > 0) total_conso_clients += val5;  // Consommation Client
        }
    }
    fclose(fp);

    // Écriture du résultat
    if (station_trouvee == 0) {
        fprintf(f_out, "%s;-1\n", station_id);
    } else {
        long fuites = capacite_usine - total_conso_clients;
        fprintf(f_out, "%s;%ld\n", station_id, fuites);
    }
}
