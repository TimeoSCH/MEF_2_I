#!/bin/bash

# --- Configuration ---
EXEC="./c-wire"
DEFAULT_DATA="water.dat"
MAKEFILE="Makefile"

# --- Fonction graphique ---
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
        plot '$input' using (\$2/$diviseur):xtic(1) linecolor rgb "$color"
GNU
}

# --- Compilation ---
if [ ! -x "${EXEC}" ] ; then
    echo "Compilation..."
    if [ ! -f "${MAKEFILE}" ]; then
        gcc -O3 -o c-wire main.c file.c avl.c
    else
        make
    fi
fi

# --- Préparation ---
DATA_FILE="${1:-$DEFAULT_DATA}"
if [ ! -f "${DATA_FILE}" ] ; then
    echo "Erreur : Fichier '${DATA_FILE}' introuvable."
    exit 1
fi

rm -f graphs/*.png
mkdir -p graphs tmp

echo "=== Génération (Tri Décroissant via C-Wire) ==="

for MODE in "max" "src" "real"; do
    case "${MODE}" in
        "max")  
            SUJET="Volume max traitement usine (HVA)"
            DIVISEUR=1000000
            UNITE="Millions de m3" ;;
        "src")  
            SUJET="Volume total capté par les sources"
            DIVISEUR=1000000
            UNITE="Millions de m3" ;;
        "real") 
            SUJET="Volume total réellement traité"
            DIVISEUR=1
            UNITE="m3" ;;
    esac

    echo "Traitement : ${MODE}..."
    rm -f stats.csv

    "${EXEC}" "${DATA_FILE}" "histo" "${MODE}"
    
    if [ -s "stats.csv" ]; then
        # On ne trie PLUS avec le shell (sort), car le C sort déjà en décroissant.
        # stats.csv contient : [Plus Gros] ... [Plus Petit]

        # Max 10 = Les 10 premières lignes (le haut du panier)
        head -n 10 stats.csv > tmp/max10.dat
        generer_graphique "tmp/max10.dat" "graphs/${MODE}_max10.png" "${SUJET} - 10 Plus Forts" "#B22222" "$DIVISEUR" "$UNITE"

        # Min 50 = Les 50 dernières lignes (le bas du panier)
        tail -n 50 stats.csv > tmp/min50.dat
        generer_graphique "tmp/min50.dat" "graphs/${MODE}_min50.png" "${SUJET} - 50 Plus Faibles" "#228B22" "$DIVISEUR" "$UNITE"
        
        echo "   -> OK."
    else
        echo "   -> [VIDE] Pas de données pour ${MODE}."
    fi
done

echo "=== Terminé ! ==="

end_script 0
