#!/bin/bash

START_TIME=$(date +%s%3N)
EXEC="./c-wire"
MAKEFILE="Makefile"


afficher_aide() {
    echo "Usage :"
    echo "  $0 <fichier.dat> histo <max|src|real>"
    echo "  $0 <fichier.dat> leaks <Identifiant_Usine>"
    echo "  $0 -h (Affiche cette aide)"
    exit 1
}

fin_traitement() {
    END_TIME=$(date +%s%3N)
    DURATION=$((END_TIME - START_TIME))
    
    if [ "$DURATION" -lt 0 ]; then
        echo "Durée totale : < 1 seconde (système sans %N)"
    else
        echo "Durée totale du traitement : ${DURATION} ms"
    fi
}


if [ "$1" == "-h" ]; then
    afficher_aide
fi

if [ "$#" -ne 3 ]; then
    echo "Erreur : Nombre d'arguments incorrect." [cite: 191]
    afficher_aide
fi

DATA_FILE="$1"
ACTION="$2"
PARAM="$3"

if [ ! -f "$DATA_FILE" ]; then
    echo "Erreur : Le fichier '$DATA_FILE' est introuvable."
    fin_traitement
    exit 1
fi

case "$ACTION" in
    "histo")
        if [[ "$PARAM" != "max" && "$PARAM" != "src" && "$PARAM" != "real" ]]; then
            echo "Erreur : Option '$PARAM' invalide pour histo. (max, src, real)"
            fin_traitement
            exit 1
        fi
        ;;
    "leaks")
        if [ -z "$PARAM" ]; then
            echo "Erreur : Identifiant usine manquant pour leaks." [cite: 192]
            fin_traitement
            exit 1
        fi
        ;;
    *)
        echo "Erreur : Commande '$ACTION' inconnue."
        afficher_aide
        ;;
esac

if [ ! -f "$MAKEFILE" ]; then
    echo "Erreur critique : Makefile introuvable."
    fin_traitement
    exit 1
fi

make
if [ $? -ne 0 ]; then
    echo "Erreur : La compilation a échoué."
    fin_traitement
    exit 1
fi

if [ ! -x "$EXEC" ]; then
    echo "Erreur : L'exécutable '$EXEC' n'a pas été généré."
    fin_traitement
    exit 1
fi

mkdir -p graphs tmp

echo "Traitement en cours : $ACTION sur $DATA_FILE avec paramètre '$PARAM'..."

if [ "$ACTION" == "histo" ]; then
    case "$PARAM" in
        "max")
            OUTPUT_CSV="vol_max.csv"
            TITRE_GRAPH="Capacité Maximale (HVA)"
            Y_LABEL="Capacité (M m3)"
            DIVISEUR=1000000 # [cite: 146]
            ;;
        "src")
            OUTPUT_CSV="vol_src.csv"
            TITRE_GRAPH="Volume Capté (Sources)"
            Y_LABEL="Volume (M m3)"
            DIVISEUR=1000000 # [cite: 149]
            ;;
        "real")
            OUTPUT_CSV="vol_real.csv"
            TITRE_GRAPH="Volume Réellement Traité"
            Y_LABEL="Volume (M m3)"
            DIVISEUR=1000000 # [cite: 154]
            ;;
    esac

    "$EXEC" "$DATA_FILE" "$ACTION" "$PARAM" > "$OUTPUT_CSV"
    
    RET_CODE=$?
    if [ $RET_CODE -ne 0 ]; then
        echo "Erreur lors de l'exécution du programme C (Code: $RET_CODE)." [cite: 198]
        fin_traitement
        exit 1
    fi

    if [ -s "$OUTPUT_CSV" ]; then

        tail -n +2 "$OUTPUT_CSV" | sort -t";" -k2,2n > tmp/sorted.tmp
        head -n 50 tmp/sorted.tmp > tmp/data_min.dat
        tail -n 10 tmp/sorted.tmp > tmp/data_max.dat

        gnuplot -persist <<-GNU
            set terminal png size 1200,800 enhanced font "arial,10"
            set datafile separator ";"
            set style data histogram
            set style histogram cluster gap 1
            set style fill solid 1.0 border -1
            set boxwidth 0.9
            set xtics rotate by -45 scale 0
            set grid ytics
            set ylabel "$Y_LABEL"
            
            # Graphique 1 : Les 50 plus petites
            set output "graphs/${PARAM}_min50.png"
            set title "$TITRE_GRAPH - 50 plus faibles"
            # Colonne 2 divisée par le diviseur pour l'unité
            plot "tmp/data_min.dat" using (\$2/$DIVISEUR):xtic(1) title "Volume" linecolor rgb "#228B22"

            # Graphique 2 : Les 10 plus grandes
            set output "graphs/${PARAM}_max10.png"
            set title "$TITRE_GRAPH - 10 plus forts"
            plot "tmp/data_max.dat" using (\$2/$DIVISEUR):xtic(1) title "Volume" linecolor rgb "#B22222"
GNU
        echo "Graphiques générés dans le dossier 'graphs/'."
    else
        echo "Avertissement : Aucun résultat trouvé pour générer les graphiques."
    fi

elif [ "$ACTION" == "leaks" ]; then
    LEAK_FILE="fuites.dat"
    
    if [ ! -f "$LEAK_FILE" ]; then
        echo "Station;Fuite" > "$LEAK_FILE"
    fi
    
    RES=$("$EXEC" "$DATA_FILE" "$ACTION" "$PARAM")
    RET_CODE=$?

    if [ $RET_CODE -ne 0 ]; then
        echo "Erreur exécution C (Station introuvable ou erreur)."
        fin_traitement
        exit 1
    fi

    echo "$RES" | grep -v "Station;Fuite" >> "$LEAK_FILE"
    VALEUR=$(echo "$RES" | grep -v "Station;Fuite")
    echo "Résultat ajouté à '$LEAK_FILE' : $VALEUR"
fi

rm -f tmp/sorted.tmp tmp/data_min.dat tmp/data_max.dat

fin_traitement
exit 0
