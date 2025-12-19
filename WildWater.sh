#!/bin/bash

DATA_FILE="data.csv"
EXECUTABLE="./c-wire"
MAKEFILE="Makefile"

show_help() {
    echo "Usage: $0 <commande> <option> [fichier_csv]"
    echo ""
    echo "Commandes:"
    echo "  histo max|src|real      : Génère l'histogramme des stations."
    echo "  leaks <id_station>      : Calcule les fuites pour une station donnée."
    echo ""
    echo "Options:"
    echo "  [fichier_csv]           : Chemin vers le fichier de données (par défaut: $DATA_FILE)"
    echo ""
    echo "Exemples:"
    echo "  $0 histo max"
    echo "  $0 leaks 'Facility complex #RH400057F'"
}

if [ "$1" == "-h" ] || [ "$#" -lt 2 ]; then
    show_help
    exit 1
fi

COMMANDE="$1"
OPTION="$2"

if [ -n "$3" ]; then
    DATA_FILE="$3"
fi

if [ ! -f "$DATA_FILE" ]; then
    echo "Erreur : Le fichier de données '$DATA_FILE' est introuvable."
    exit 1
fi

if [ ! -f "$MAKEFILE" ]; then
    echo "Erreur : Makefile introuvable."
    exit 2
fi

if [ ! -x "$EXECUTABLE" ]; then
    make
    if [ $? -ne 0 ]; then
        echo "Erreur : La compilation a échoué."
        exit 2
    fi
fi

mkdir -p graphs
mkdir -p tmp

START=$(date +%s)

if [ "$COMMANDE" == "histo" ]; then
    if [[ ! "$OPTION" =~ ^(max|src|real)$ ]]; then
        echo "Erreur : L'option pour 'histo' doit être 'max', 'src' ou 'real'."
        exit 1
    fi

    "$EXECUTABLE" "$DATA_FILE" "$COMMANDE" "$OPTION"
    CODE_RETOUR=$?

    if [ $CODE_RETOUR -ne 0 ]; then
        echo "Erreur : Le programme C a rencontré un problème (Code $CODE_RETOUR)."
        exit 3
    fi

    OUTPUT_DAT="vol_${OPTION}.dat"
    OUTPUT_IMG="graphs/vol_${OPTION}.png"

    if [ "$OPTION" == "max" ]; then
        HEADER="identifier;max volume (k.m3.year-1);source volume;real volume"
    else
        HEADER="identifier;volume (k.m3.year-1)"
    fi
    echo "$HEADER" > "$OUTPUT_DAT"

    if [ -f "stats.csv" ]; then
        sort -t';' -k1,1r stats.csv >> "$OUTPUT_DAT"
    else
        echo "Erreur : Fichier de sortie du programme C introuvable."
        exit 3
    fi

    tail -n +2 "$OUTPUT_DAT" | sort -t';' -k2,2n > tmp/data_sorted.dat

    NB_LINES=$(wc -l < tmp/data_sorted.dat)
    
    if [ "$NB_LINES" -le 10 ]; then
        cat tmp/data_sorted.dat > tmp/data_plot.dat
    else
        head -n 5 tmp/data_sorted.dat > tmp/data_plot.dat
        tail -n 5 tmp/data_sorted.dat >> tmp/data_plot.dat
    fi

    gnuplot -persist <<-EOF
        set terminal png size 1200,800
        set output '$OUTPUT_IMG'
        set title 'Histogramme : 5 plus faibles et 5 plus forts volumes ($OPTION)'
        set datafile separator ";"
        set style data histograms
        set style fill solid
        set boxwidth 0.5
        set xtics rotate by -45 scale 0 font ",9"
        set ylabel 'Volume (k.m3)'
        set grid ytics
        set xlabel 'Usines'
        plot 'tmp/data_plot.dat' using 2:xtic(1) title '$OPTION volume' linecolor rgb "blue"
EOF

elif [ "$COMMANDE" == "leaks" ]; then
    ID_LEAK="$OPTION"
    if [ -z "$ID_LEAK" ]; then
        echo "Erreur : L'identifiant de la station est manquant."
        exit 1
    fi

    "$EXECUTABLE" "$DATA_FILE" "$COMMANDE" "$ID_LEAK"
    CODE_RETOUR=$?

    if [ $CODE_RETOUR -ne 0 ]; then
         echo "Attention : Le programme C a signalé une erreur ou un avertissement."
    fi

    OUTPUT_LEAKS="leaks_history.dat"

    if [ -f "stats.csv" ]; then
        if [ ! -f "$OUTPUT_LEAKS" ]; then
             echo "identifier;Leak volume (M.m3.year-1)" > "$OUTPUT_LEAKS"
        fi
        cat stats.csv >> "$OUTPUT_LEAKS"
        cat stats.csv
    else
        echo "Erreur : Pas de données générées par le programme C."
    fi

else
    echo "Erreur : Commande '$COMMANDE' inconnue."
    show_help
    exit 1
fi

rm -f stats.csv
rm -rf tmp

END=$(date +%s)
DURATION=$((END - START))

echo "Durée totale : ${DURATION} secondes"

exit 0

