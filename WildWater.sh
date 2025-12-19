#!/bin/bash

# --- Configuration ---
EXEC="./c-wire"
DEFAULT_DATA="water.dat"
MAKEFILE="Makefile"

# --- Fonctions ---

generer_graphique() {
    local input="$1"
    local output="$2"
    local titre="$3"
    local color="$4"

    # Sécurité : si le fichier dat est vide, on ne fait rien
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

# --- 1. Vérification et Compilation ---

if [ ! -x "${EXEC}" ] ; then
    echo "Compilation en cours..."
    if [ ! -f "${MAKEFILE}" ]; then
        echo "Erreur : Makefile introuvable."
        exit 1
    fi
    make
    if [ $? -ne 0 ] ; then
        echo "Erreur : La compilation a échoué."
        exit 1
    fi
fi

# --- 2. Préparation ---

DATA_FILE="${1:-$DEFAULT_DATA}"

if [ ! -f "${DATA_FILE}" ] ; then
    echo "Erreur : Fichier '${DATA_FILE}' introuvable."
    exit 1
fi

rm -f graphs/*.png
mkdir -p graphs tmp

echo "=== Démarrage de la génération des 6 histogrammes ==="

# --- 3. Boucle principale (Max, Src, Real) ---

for MODE in "max" "src" "real"; do
    # Définition des titres selon ta demande
    case "${MODE}" in
        "max")  SUJET="Volume maximal de traitement de l’usine";;
        "src")  SUJET="Volume total capté par les sources";;
        "real") SUJET="Volume total réellement traité";;
    esac

    echo "Traitement en cours : ${MODE}..."

    # IMPORTANT : On supprime l'ancien fichier stats.csv avant de lancer le C
    # Cela évite que 'src' ou 'real' ne réutilisent les données de 'max' en cas d'erreur.
    rm -f stats.csv

    # Exécution du programme C
    "${EXEC}" "${DATA_FILE}" "histo" "${MODE}"
    
    if [ $? -ne 0 ]; then
        echo "   -> Erreur lors de l'exécution C pour ${MODE}"
        continue
    fi
    
    # Vérification que le fichier de sortie existe et n'est pas vide
    if [ -s "stats.csv" ]; then
        # Tri numérique sur la colonne 2 (Volume)
        tail -n +2 stats.csv | sort -t";" -k2,2n > tmp/sorted.tmp

        # --- Graphique 1 : Les 50 plus petites valeurs (Vert #228B22) ---
        head -n 50 tmp/sorted.tmp > tmp/min50.dat
        generer_graphique "tmp/min50.dat" "graphs/${MODE}_min50.png" "${SUJET} - 50 Plus Faibles" "#228B22"

        # --- Graphique 2 : Les 10 plus grandes valeurs (Rouge #B22222) ---
        tail -n 10 tmp/sorted.tmp > tmp/max10.dat
        generer_graphique "tmp/max10.dat" "graphs/${MODE}_max10.png" "${SUJET} - 10 Plus Forts" "#B22222"
        
        echo "   -> Graphiques générés."
    else
        echo "   -> Attention : Aucune donnée trouvée pour ${MODE} (stats.csv vide ou absent)."
    fi
done

echo "=== Terminé ! Images disponibles dans le dossier graphs/ ==="

end_script 0
