#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "leak.h"

#define MAX_LIGNE 2048
#define MAX_ID 100

typedef struct ListeEnfants {
    struct NoeudAVL* station; 
    double pourcentage_fuite; 
    struct ListeEnfants* suivant;
} ListeEnfants;

typedef struct NoeudAVL {
    char id[MAX_ID];
    long capacite;            
    ListeEnfants* enfants;    
    int nb_enfants;          
    int hauteur;             
    struct NoeudAVL* gauche;
    struct NoeudAVL* droite;
} NoeudAVL;


int hauteur(NoeudAVL* n) {
    return (n == NULL) ? 0 : n->hauteur;
}

int max_int(int a, int b) {
    return (a > b) ? a : b;
}

NoeudAVL* creer_noeud(char* id) {
    NoeudAVL* n = (NoeudAVL*)malloc(sizeof(NoeudAVL));
    if (!n) { perror("Malloc"); exit(1); }
    strncpy(n->id, id, MAX_ID - 1);
    n->id[MAX_ID - 1] = '\0';
    n->capacite = 0;
    n->enfants = NULL;
    n->nb_enfants = 0;
    n->hauteur = 1;
    n->gauche = NULL;
    n->droite = NULL;
    return n;
}

NoeudAVL* rotation_droite(NoeudAVL* y) {
    NoeudAVL* x = y->gauche;
    NoeudAVL* T2 = x->droite;
    x->droite = y;
    y->gauche = T2;
    y->hauteur = max_int(hauteur(y->gauche), hauteur(y->droite)) + 1;
    x->hauteur = max_int(hauteur(x->gauche), hauteur(x->droite)) + 1;
    return x;
}

NoeudAVL* rotation_gauche(NoeudAVL* x) {
    NoeudAVL* y = x->droite;
    NoeudAVL* T2 = y->gauche;
    y->gauche = x;
    x->droite = T2;
    x->hauteur = max_int(hauteur(x->gauche), hauteur(x->droite)) + 1;
    y->hauteur = max_int(hauteur(y->gauche), hauteur(y->droite)) + 1;
    return y;
}

int get_equilibre(NoeudAVL* n) {
    return (n == NULL) ? 0 : hauteur(n->gauche) - hauteur(n->droite);
}

NoeudAVL* chercher_ou_creer(NoeudAVL** racine, char* id) {
    if (*racine == NULL) {
        *racine = creer_noeud(id);
        return *racine;
    }

    int cmp = strcmp(id, (*racine)->id);
    NoeudAVL* resultat = NULL;

    if (cmp < 0) {
        resultat = chercher_ou_creer(&((*racine)->gauche), id);
    } else if (cmp > 0) {
        resultat = chercher_ou_creer(&((*racine)->droite), id);
    } else {
        return *racine; // Trouvé
    }

    (*racine)->hauteur = 1 + max_int(hauteur((*racine)->gauche), hauteur((*racine)->droite));
    int balance = get_equilibre(*racine);

    if (balance > 1 && strcmp(id, (*racine)->gauche->id) < 0)
        *racine = rotation_droite(*racine);
    else if (balance < -1 && strcmp(id, (*racine)->droite->id) > 0)
        *racine = rotation_gauche(*racine);
    else if (balance > 1 && strcmp(id, (*racine)->gauche->id) > 0) {
        (*racine)->gauche = rotation_gauche((*racine)->gauche);
        *racine = rotation_droite(*racine);
    } else if (balance < -1 && strcmp(id, (*racine)->droite->id) < 0) {
        (*racine)->droite = rotation_droite((*racine)->droite);
        *racine = rotation_gauche(*racine);
    }

    return resultat;
}

NoeudAVL* trouver_noeud(NoeudAVL* racine, char* id) {
    if (racine == NULL) return NULL;
    int cmp = strcmp(id, racine->id);
    if (cmp < 0) return trouver_noeud(racine->gauche, id);
    else if (cmp > 0) return trouver_noeud(racine->droite, id);
    else return racine;
}


void ajouter_enfant(NoeudAVL* parent, NoeudAVL* enfant, double pourcentage) {
    ListeEnfants* current = parent->enfants;
    while(current != NULL) {
        if(current->station == enfant) return;
        current = current->suivant;
    }

    ListeEnfants* nouv = (ListeEnfants*)malloc(sizeof(ListeEnfants));
    if (!nouv) { perror("Malloc Liste"); exit(1); }
    
    nouv->station = enfant;
    nouv->pourcentage_fuite = pourcentage;
    nouv->suivant = parent->enfants;
    parent->enfants = nouv;
    parent->nb_enfants++;
}

double calculer_fuites_rec(NoeudAVL* noeud, double volume_entrant) {
    if (noeud == NULL || volume_entrant <= 0 || noeud->nb_enfants == 0) {
        return 0.0;
    }
    
    double volume_par_troncon = volume_entrant / noeud->nb_enfants;
    double total_fuites = 0.0;

    ListeEnfants* cour = noeud->enfants;
    while (cour != NULL) {
        double fuite_troncon = volume_par_troncon * (cour->pourcentage_fuite / 100.0);
        double volume_restant = volume_par_troncon - fuite_troncon;
        total_fuites += fuite_troncon + calculer_fuites_rec(cour->station, volume_restant);
        cour = cour->suivant;
    }

    return total_fuites;
}

void liberer_avl_et_graphe(NoeudAVL* racine) {
    if (racine == NULL) return;
    
    liberer_avl_et_graphe(racine->gauche);
    liberer_avl_et_graphe(racine->droite);

    ListeEnfants* l = racine->enfants;
    while (l != NULL) {
        ListeEnfants* temp = l;
        l = l->suivant;
        free(temp);
    }
    free(racine);
}


void traiter_fuites(char* chemin, char* station_id, FILE* f_out) {
    FILE* fp = fopen(chemin, "r");
    if (fp == NULL) {
        perror("Erreur ouverture fichier");
        exit(1);
    }

    NoeudAVL* racine_avl = NULL;
    char ligne[MAX_LIGNE];
    
    while (fgets(ligne, MAX_LIGNE, fp) != NULL) {
        ligne[strcspn(ligne, "\r\n")] = 0;
        if (strlen(ligne) == 0) continue;

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

        if (i >= 5) {
            char* id_amont = cols[1];
            char* id_aval = cols[2];
            char* str_cap = cols[3];
            char* str_fuite = cols[4];
            int amont_existe = (strcmp(id_amont, "-") != 0 && strlen(id_amont) > 0);
            int aval_existe = (strcmp(id_aval, "-") != 0 && strlen(id_aval) > 0);
            int est_definition_usine = (amont_existe && !aval_existe && strcmp(str_cap, "-") != 0);
            
            if (est_definition_usine) {
                NoeudAVL* n = chercher_ou_creer(&racine_avl, id_amont);
                n->capacite = atol(str_cap);
            }
            else if (amont_existe && aval_existe) {
                NoeudAVL* u_amont = chercher_ou_creer(&racine_avl, id_amont);
                NoeudAVL* u_aval = chercher_ou_creer(&racine_avl, id_aval);
                
                double pct = (strcmp(str_fuite, "-") != 0) ? atof(str_fuite) : 0.0;
                
                ajouter_enfant(u_amont, u_aval, pct);
            }
        }
    }
    fclose(fp);
    NoeudAVL* depart = trouver_noeud(racine_avl, station_id);

    if (depart == NULL) {
        fprintf(f_out, "%s;-1\n", station_id); 
    } else {
        double fuites_totales = calculer_fuites_rec(depart, (double)depart->capacite);
        fprintf(f_out, "%s;%f\n", station_id, fuites_totales);
    }
    liberer_avl_et_graphe(racine_avl);
}
