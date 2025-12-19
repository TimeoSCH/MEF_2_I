#!/bin/bash

EXEC="./c-wire"
DEFAULT_DATA="water.dat"
MAKEFILE="Makefile"
START_TIME=$(date +%s)

show_help() {
    echo "Usage: $0 [fichier_dat]"
    echo "Génère 6 histogrammes (Min 50 / Max 10) pour 3 catégories."
    echo "Exemple: $0 water.dat"
}

end_script() {
    RET_VAL=${1}
    END_TIME=$(date +%s)
    DURATION=$((END_TIME - START_TIME))
    echo "------------------------------------------------"
    echo "Durée totale du traitement : ${DURATION} secondes."
    exit ${RET_VAL}
}

generer_graphique() {
    local input="$1"
    local output="$2"
    local titre="$3"
    local color="$4"

    if [ ! -s "$input" ]; then return; fi

    gnuplot -persist <<-GNU
        set terminal png size 1800,900 enhanced font "arial,11"
        set output '$output'
        set title '$titre'
        set datafile separator ";"
        set style data histograms
        set style fill solid 1.0 border -1
        set boxwidth 0.7
        set xtics rotate by -45 scale 0 font ",9"
        set xlabel 'Identifiant Station'
        set ylabel 'Volume (m3)'
        set grid ytics
        set bmargin 12
        plot '$input' using 2:xtic(1) notitle linecolor rgb "$color"
GNU
}

if [ ! -x "${EXEC}" ] ; then
    echo "Compilation en cours..."
    if [ ! -f "${MAKEFILE}" ]; then
        echo "Erreur : Makefile introuvable."
        end_script 1
    fi
    make
    if [ $? -ne 0 ] ; then
        echo "Erreur : La compilation a échoué."
        end_script 1
    fi
fi

DATA_FILE="${1:-$DEFAULT_DATA}"

if [ ! -f "${DATA_FILE}" ] ; then
    echo "Erreur : Fichier '${DATA_FILE}' introuvable."
    show_help
    end_script 1
fi

rm -f graphs/*.png
mkdir -p graphs tmp

echo "=== Démarrage de la génération des 6 histogrammes ==="

for MODE in "max" "src" "real"; do
    case "${MODE}" in
        "max")  SUJET="Capacité Usine (HVA)";;
        "src")  SUJET="Volume Capté (Sources)";;
        "real") SUJET="Volume Consommé (Utilisateurs)";;
    esac

    echo "Traitement : ${MODE}"

    "${EXEC}" "${DATA_FILE}" "histo" "${MODE}"
    if [ $? -ne 0 ]; then
        echo "Erreur lors de l'exécution C pour ${MODE}"
        continue
    fi
    
    if [ -f "stats.csv" ]; then
        tail -n +2 stats.csv | sort -t";" -k2,2n > tmp/sorted.tmp

        # 50 plus faibles : couleur vert forêt (#228B22)
        head -n 50 tmp/sorted.tmp > tmp/min50.dat
        generer_graphique "tmp/min50.dat" "graphs/${MODE}_min50.png" "${SUJET} - 50 Plus Faibles" "#228B22"

        # 10 plus forts : couleur rouge brique (#B22222)
        tail -n 10 tmp/sorted.tmp > tmp/max10.dat
        generer_graphique "tmp/max10.dat" "graphs/${MODE}_max10.png" "${SUJET} - 10 Plus Forts" "#B22222"
    fi
done

echo "=== Terminé ! Images disponibles dans graphs/ ==="
ls -1 graphs/*.png

end_script 0
