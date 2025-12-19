#!/bin/bash

# --- 1. Démarrage Chronomètre ---
START_TIME=$(date +%s)

# --- Configuration ---
EXEC="./c-wire"
DEFAULT_DATA="water.dat"
MAKEFILE="Makefile"
DATA_FILE="$DEFAULT_DATA"

# --- Fonction d'affichage de la durée ---
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
    echo "  $0 [fichier.dat] histo <max|src|real>"
    echo "  $0 -h (Aide)"
    afficher_duree
    exit 1
}

# --- Fonction graphique ---
generer_graphique() {
    local input="$1"; local output="$2"; local titre="$3"
    local color="$4"; local diviseur="$5"; local unite="$6"

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

# --- 2. Vérification Arguments ---
if [ "$1" == "-h" ]; then usage; fi

if [ -f "$1" ] && [ "$#" -eq 3 ]; then
    DATA_FILE="$1"; ACTION="$2"; OPTION="$3"
elif [ "$#" -eq 2 ]; then
    ACTION="$1"; OPTION="$2"
else
    usage
fi

if [ ! -f "$DATA_FILE" ]; then
     echo "Erreur : Fichier '$DATA_FILE' introuvable."
     afficher_duree; exit 1
fi

case "$ACTION" in
    "histo")
        if [[ "$OPTION" != "max" && "$OPTION" != "src" && "$OPTION" != "real" ]]; then
            usage
        fi ;;
    "leaks")
        if [ -z "$OPTION" ]; then usage; fi ;;
    *) usage ;;
esac

# --- 3. Compilation Intelligente ---
# On lance make à chaque fois.
# S'il n'y a rien à faire, make répondra "Nothing to be done" en 0.01s.
if [ -f "${MAKEFILE}" ]; then
    make
else
    # Fallback si pas de Makefile (plus lent car recompile tout)
    echo "Attention : Makefile introuvable."
    gcc -o c-wire main.c file.c avl.c leaks.c
fi

if [ ! -x "${EXEC}" ]; then
    echo "Erreur critique : La compilation a échoué."
    afficher_duree; exit 1
fi

# --- 4. Exécution ---
echo "Lancement : $ACTION $OPTION"
rm -f stats.csv

"${EXEC}" "${DATA_FILE}" "${ACTION}" "${OPTION}"
if [ $? -ne 0 ]; then
    echo "Erreur lors de l'exécution du C."
    afficher_duree; exit 1
fi

# --- 5. Post-Traitement ---
if [ "$ACTION" == "histo" ]; then
    if [ ! -s "stats.csv" ]; then
        echo "Avertissement : Fichier vide."
    else
        case "${OPTION}" in
            "max")  TITRE="Volume max (HVA)"; DIV=1000000; UNITE="Millions m3" ;;
            "src")  TITRE="Volume sources"; DIV=1000000; UNITE="Millions m3" ;;
            "real") TITRE="Volume réel"; DIV=1; UNITE="m3" ;;
        esac

        mkdir -p graphs tmp
        sort -t";" -k2,2n stats.csv > tmp/sorted.tmp
        head -n 50 tmp/sorted.tmp > tmp/min50.dat
        generer_graphique "tmp/min50.dat" "graphs/${OPTION}_min50.png" "$TITRE - 50 Min" "#228B22" "$DIV" "$UNITE"
        tail -n 10 tmp/sorted.tmp > tmp/max10.dat
        generer_graphique "tmp/max10.dat" "graphs/${OPTION}_max10.png" "$TITRE - 10 Max" "#B22222" "$DIV" "$UNITE"
        echo "Graphiques générés."
    fi
elif [ "$ACTION" == "leaks" ]; then
    echo "Résultat dans 'stats.csv'."
fi

afficher_duree
exit 0
