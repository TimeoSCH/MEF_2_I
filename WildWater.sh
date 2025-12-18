#!/bin/bash


show_help() {
    echo "Usage: $0 <fichier_dat> <station_type> <consommateur_type> [id_station]"
    echo ""
    echo "Arguments:"
    echo "  <fichier_dat>       : Chemin vers le fichier de données (.csv)"
    echo "  <command>           : 'histo' ou 'leaks'"
    echo "  <mode>              : Pour histo : 'max', 'src' ou 'real'"
    echo "                        Pour leaks : l'identifiant de la station"
    echo ""
    echo "Exemples:"
    echo "  $0 data.csv histo max"
    echo "  $0 data.csv leaks 12345"
    echo "  $0 -h               : Affiche cette aide"
}

if [ "$1" == "-h" ]; then
    show_help
    exit 0
fi

if [ "$#" -lt 3 ]; then
    echo "Erreur : Arguments insuffisants."
    show_help
    exit 1
fi

FICHIER_CSV="$1"
COMMANDE="$2"
OPTION="$3"

if [ ! -f "$FICHIER_CSV" ]; then
    echo "Erreur : Le fichier '$FICHIER_CSV' est introuvable."
    exit 1
fi

EXECUTABLE="./c-wire"
MAKEFILE="Makefile"

if [ ! -f "$MAKEFILE" ]; then
    echo "Erreur : Makefile introuvable."
    exit 2
fi

make clean > /dev/null 2>&1
make
if [ $? -ne 0 ]; then
    echo "Erreur : La compilation a échoué."
    exit 2
fi

if [ ! -x "$EXECUTABLE" ]; then
    echo "Erreur : L'exécutable n'a pas été créé."
    exit 2
fi

mkdir -p tmp
mkdir -p graphs

START=$(date +%s%3N)

if [ "$COMMANDE" == "histo" ]; then
    if [[ ! "$OPTION" =~ ^(max|src|real)$ ]]; then
        echo "Erreur : Pour 'histo', l'option doit être 'max', 'src' ou 'real'."
        exit 1
    fi
    
    "$EXECUTABLE" "$FICHIER_CSV" "$COMMANDE" "$OPTION"
    CODE_RETOUR=$?

    if [ $CODE_RETOUR -ne 0 ]; then
        echo "Erreur : Le programme C a rencontré un problème (Code $CODE_RETOUR)."
        exit 3
    fi

    OUTPUT_FILE="${COMMANDE}_${OPTION}.dat"
    
    if [ "$OPTION" == "max" ]; then
        echo "identifier;max volume (k.m3.year-1);source volume;real volume" > "$OUTPUT_FILE"
    else
        echo "identifier;volume (k.m3.year-1)" > "$OUTPUT_FILE"
    fi
    
    sort -t';' -k1,1r stats.csv >> "$OUTPUT_FILE"

    tail -n +2 "$OUTPUT_FILE" > tmp/data_plot.dat
    
    sort -t';' -k2,2n tmp/data_plot.dat > tmp/data_sorted.dat
    
    head -n 50 tmp/data_sorted.dat > tmp/min_50.dat
    tail -n 10 tmp/data_sorted.dat > tmp/max_10.dat

    gnuplot -persist <<-EOF
        set terminal png size 1000,600
        set output 'graphs/${OPTION}_min_50.png'
        set title '50 stations avec le plus petit volume ($OPTION)'
        set datafile separator ";"
        set style data histograms
        set style fill solid
        set boxwidth 0.5
        set xtics rotate by -45 scale 0 font ",8"
        set ylabel 'Volume (m3)'
        plot 'tmp/min_50.dat' using 2:xtic(1) notitle linecolor rgb "blue"
EOF

    gnuplot -persist <<-EOF
        set terminal png size 1000,600
        set output 'graphs/${OPTION}_max_10.png'
        set title '10 stations avec le plus grand volume ($OPTION)'
        set datafile separator ";"
        set style data histograms
        set style fill solid
        set boxwidth 0.5
        set xtics rotate by -45 scale 0 font ",8"
        set ylabel 'Volume (m3)'
        plot 'tmp/max_10.dat' using 2:xtic(1) notitle linecolor rgb "red"
EOF

elif [ "$COMMANDE" == "leaks" ]; then
    ID_LEAK="$3"
    if [ -z "$ID_LEAK" ]; then
        echo "Erreur : L'identifiant est obligatoire pour 'leaks'."
        exit 1
    fi
    
    "$EXECUTABLE" "$FICHIER_CSV" "$COMMANDE" "$ID_LEAK"
    
    if [ -f "stats.csv" ]; then
        cat stats.csv >> "leaks_history.dat"
        cat stats.csv
    else
        echo "Identifiant introuvable ou erreur traitement."
    fi

else
    echo "Erreur : Commande '$COMMANDE' inconnue. Utilisez 'histo' ou 'leaks'."
    exit 1
fi

END=$(date +%s%3N)
DURATION=$((END - START))
echo "Durée totale : ${DURATION} ms"

rm -f stats.csv

exit 0
