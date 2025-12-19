#!/bin/bash

# --- Configuration ---
EXEC="./c-wire"
DEFAULT_DATA="water.dat"
MAKEFILE="Makefile"

# --- Fonction graphique (Millions de m3) ---
generer_graphique() {
    local input="$1"
    local output="$2"
    local titre="$3"
    local color="$4"

    if [ ! -s "$input" ]; then return; fi

    gnuplot -persist <<-GNU
        set terminal png size 1800,900 enhanced font "arial,12"
        set output '$output'
        set title '$titre' font "arial,16"
        set xlabel 'Identifiant Station' font "arial,14"
        set ylabel 'Volume (Millions de m3)' font "arial,14"
        set style data histogram
        set style histogram cluster gap 1
        set style fill solid 1.0 border -1
        set boxwidth 0.8 relative
        set grid ytics linestyle 1 lc rgb "#B0B0B0"
        set ytics font "arial,12"
        set xtics rotate by -45 scale 0 font "arial,10"
        set bmargin 15
        set lmargin 10
        set key off
        set datafile separator ";"
        plot '$input' using (\$2/1000000):xtic(1) linecolor rgb "$color"
GNU
}

# --- Compilation ---
if [ ! -x "${EXEC}" ] ; then
    echo "Exécutable introuvable. Compilation..."
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

echo "=== Génération des graphiques (Millions de m3) ==="

# --- Boucle Principale ---
for MODE in "max" "src" "real"; do
    case "${MODE}" in
        "max")  SUJET="Volume maximal de traitement de l’usine (HVA)";;
        "src")  SUJET="Volume total capté par les sources";;
        "real") SUJET="Volume total réellement traité";;
    esac

    echo "Traitement : ${MODE}..."
    rm -f stats.csv

    "${EXEC}" "${DATA_FILE}" "histo" "${MODE}"
    
    if [ -s "stats.csv" ]; then
        tail -n +2 stats.csv | sort -t";" -k2,2n > tmp/sorted.tmp
        head -n 50 tmp/sorted.tmp > tmp/min50.dat
        generer_graphique "tmp/min50.dat" "graphs/${MODE}_min50.png" "${SUJET} - 50 Plus Faibles" "#228B22"
        tail -n 10 tmp/sorted.tmp > tmp/max10.dat
        generer_graphique "tmp/max10.dat" "graphs/${MODE}_max10.png" "${SUJET} - 10 Plus Forts" "#B22222"
        echo "   -> Graphiques générés."
    else
        echo "   -> [VIDE] Pas de données pour ${MODE}."
    fi
done

echo "=== Terminé ! ==="

end_script 0
