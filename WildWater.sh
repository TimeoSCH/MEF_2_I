#!/bin/bash

# --- 1. Démarrage Chronomètre ---
START_TIME=$(date +%s)

# --- Configuration ---
EXEC="./c-wire"
DEFAULT_DATA="water.dat"
MAKEFILE="Makefile"
DATA_FILE="$DEFAULT_DATA" # Par défaut, on peut le changer si besoin

# --- Fonction d'affichage de la durée (appelée à la fin ou sur erreur) ---
afficher_duree() {
    END_TIME=$(date +%s)
    DURATION=$((END_TIME - START_TIME))
    echo "Durée totale du traitement : ${DURATION} secondes."
}

# --- Fonction d'aide ---
usage() {
    echo "Usage incorrect."
    echo "Commandes valides :"
    echo "  $0 histo <max|src|real>"
    echo "  $0 leaks <Identifiant_Usine>"
    echo "  $0 -h (Aide)"
    afficher_duree
    exit 1
}

# --- Fonction graphique (Gnuplot) ---
generer_graphique() {
    local input="$1"
    local output="$2"
    local titre="$3"
    local color="$4"
    local diviseur="$5"
    local unite="$6"

    if [ ! -s "$input" ]; then return; fi

    gnuplot -persist <<-GNU
        set terminal png size 1800,900 enhanced font "arial,12"
        set output '$output'
        set title '$titre' font "arial,16"
        set xlabel 'Identifiant Station' font "arial,14"
        set ylabel 'Volume ($unite)' font "arial,14"
        set style data histogram
        set style histogram cluster gap 1
        set style fill solid 1.0 border -1
        set boxwidth 0.8 relative
        set grid ytics linestyle 1 lc rgb "#B0B0B0"
        set ytics font "arial,12"
        set xtics rotate by -45 scale 0 font "arial,10"
        set bmargin 15
        set lmargin 12
        set key off
        set datafile separator ";"
        set yrange [0:*]
        plot '$input' using (\$2/$diviseur):xtic(1) linecolor rgb "$color"
GNU
}

# --- 2. Vérification des Arguments (Strict) ---

# Vérification du nombre d'arguments
if [ "$#" -lt 2 ] || [ "$#" -gt 3 ]; then
    usage
fi

ACTION="$1"
OPTION="$2"

# Si on a 3 arguments (ex: ./WildWater.sh data.dat histo max), on décale
if [ -f "$1" ] && [ "$#" -eq 3 ]; then
    DATA_FILE="$1"
    ACTION="$2"
    OPTION="$3"
elif [ ! -f "$DEFAULT_DATA" ] && [ "$#" -eq 2 ]; then
     echo "Erreur : Fichier de données '$DEFAULT_DATA' introuvable."
     afficher_duree
     exit 1
fi

# Validation de l'action
case "$ACTION" in
    "histo")
        if [[ "$OPTION" != "max" && "$OPTION" != "src" && "$OPTION" != "real" ]]; then
            echo "Erreur : L'argument pour histo doit être 'max', 'src' ou 'real'."
            usage
        fi
        ;;
    "leaks")
        if [ -z "$OPTION" ]; then
            echo "Erreur : L'identifiant de l'usine est manquant pour leaks."
            usage
        fi
        ;;
    *)
        echo "Erreur : Commande '$ACTION' inconnue."
        usage
        ;;
esac

# --- 3. Compilation (Vérification existence) ---
if [ ! -x "${EXEC}" ]; then
    echo "Exécutable introuvable. Compilation..."
    if [ -f "${MAKEFILE}" ]; then
        make
    else
        rm -f c-wire
        gcc -O3 -o c-wire main.c file.c avl.c
    fi

    if [ ! -x "${EXEC}" ]; then
        echo "Erreur : La compilation a échoué."
        afficher_duree
        exit 1
    fi
fi

# --- 4. Exécution du Traitement ---

echo "Lancement du traitement : $ACTION $OPTION"
rm -f stats.csv

# Appel du programme C (On passe le fichier, l'action et l'option)
"${EXEC}" "${DATA_FILE}" "${ACTION}" "${OPTION}"
CODE_RETOUR=$?

# Vérification du code retour du C
if [ $CODE_RETOUR -ne 0 ]; then
    echo "Erreur : Le programme C s'est arrêté avec le code $CODE_RETOUR."
    afficher_duree
    exit 1
fi

# --- 5. Post-Traitement (Graphiques ou Affichage) ---

if [ "$ACTION" == "histo" ]; then
    if [ ! -s "stats.csv" ]; then
        echo "Attention : Le fichier de sortie est vide."
    else
        # Configuration selon le mode
        case "${OPTION}" in
            "max")  
                TITRE="Volume max traitement usine (HVA)"
                DIV=1000000; UNITE="Millions de m3" ;;
            "src")  
                TITRE="Volume total capté par les sources"
                DIV=1000000; UNITE="Millions de m3" ;;
            "real") 
                TITRE="Volume total réellement traité"
                DIV=1; UNITE="m3" ;;
        esac

        mkdir -p graphs tmp
        echo "Génération des graphiques..."

        # Tri croissant
        sort -t";" -k2,2n stats.csv > tmp/sorted.tmp

        # Min 50
        head -n 50 tmp/sorted.tmp > tmp/min50.dat
        generer_graphique "tmp/min50.dat" "graphs/${OPTION}_min50.png" "$TITRE - 50 Plus Faibles" "#228B22" "$DIV" "$UNITE"

        # Max 10
        tail -n 10 tmp/sorted.tmp > tmp/max10.dat
        generer_graphique "tmp/max10.dat" "graphs/${OPTION}_max10.png" "$TITRE - 10 Plus Forts" "#B22222" "$DIV" "$UNITE"
        
        echo "Graphiques générés dans le dossier graphs/."
    fi

elif [ "$ACTION" == "leaks" ]; then
    echo "Traitement des fuites terminé. Résultats dans 'stats.csv'."
    # Le sujet ne demande pas de graphique pour leaks, juste le fichier csv.
fi

afficher_duree
exit 0
