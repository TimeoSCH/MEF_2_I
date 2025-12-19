#!/bin/bash

# --- Configuration ---
EXEC="./c-wire"
DEFAULT_DATA="water.dat"
MAKEFILE="Makefile"
TMP_INPUT="tmp/input_data.tmp" # Fichier temporaire pour les données nettoyées

# --- 1. FORCE LA COMPATIBILITE NUMERIQUE (Point vs Virgule) ---
export LC_NUMERIC=C

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
        
        # Division par l'échelle
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

# --- Vérification Arguments ---
DATA_FILE="${1:-$DEFAULT_DATA}"
if [ ! -f "${DATA_FILE}" ] ; then
    echo "Erreur : Fichier '${DATA_FILE}' introuvable."
    exit 1
fi

rm -f graphs/*.png
mkdir -p graphs tmp

echo "=== Génération des graphiques ==="

for MODE in "max" "src" "real"; do
    
    # 1. Configuration des paramètres graphiques
    case "${MODE}" in
        "max")  
            SUJET="Volume max traitement usine (HVA)"
            DIVISEUR=1000000
            UNITE="Millions de m3" 
            ;;
        "src")  
            SUJET="Volume total capté par les sources"
            DIVISEUR=1000000
            UNITE="Millions de m3" 
            ;;
        "real") 
            SUJET="Volume total réellement traité (Consommation)"
            DIVISEUR=1000000 # On passe en Millions comme la correction
            UNITE="Millions de m3" 
            ;;
    esac

    echo "Traitement : ${MODE}..."

    # 2. PRE-TRAITEMENT AWK (Logique inspirée de la correction)
    # On filtre et calcule les données AVANT le C pour gérer le cas "real" (fuites)
    # tr -d '\r' supprime les retours chariots Windows
    case "${MODE}" in
        "max")
            < "$DATA_FILE" tr -d '\r' | awk -F';' '$2 ~ "Plant" && $4 != "-" {print $2";"$4";0"}' > "$TMP_INPUT"
            ;;
        "src")
            < "$DATA_FILE" tr -d '\r' | awk -F';' '$2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {print $3";"$4";0"}' > "$TMP_INPUT"
            ;;
        "real")
            # C'est ici que la magie opère pour le "real" : Calcul Vol * (1 - Fuite%)
            < "$DATA_FILE" tr -d '\r' | awk -F';' '$2 ~ "Source|Well|Resurgence|Spring|Fountain" && $3 ~ "Plant" {
                vol = $4; 
                fuite = ($5 == "-" ? 0 : $5); 
                reel = vol * (1 - fuite/100);
                printf "%s;%.6f;0\n", $3, reel
            }' > "$TMP_INPUT"
            ;;
    esac

    # 3. Exécution du C
    rm -f stats.csv
    # On passe le fichier temporaire pré-traité au programme C
    "${EXEC}" "$TMP_INPUT" 
    
    if [ -s "stats.csv" ]; then
        # 4. Tri des résultats
        sort -t";" -k2,2n stats.csv > tmp/sorted_volume.tmp

        # Graphique 1 : Min 50
        head -n 50 tmp/sorted_volume.tmp > tmp/min50.dat
        generer_graphique "tmp/min50.dat" "graphs/${MODE}_min50.png" "${SUJET} - 50 Plus Faibles" "#228B22" "$DIVISEUR" "$UNITE"

        # Graphique 2 : Max 10
        tail -n 10 tmp/sorted_volume.tmp > tmp/max10.dat
        generer_graphique "tmp/max10.dat" "graphs/${MODE}_max10.png" "${SUJET} - 10 Plus Forts" "#B22222" "$DIVISEUR" "$UNITE"
        
        echo "   -> OK : graphs/${MODE}_min50.png et graphs/${MODE}_max10.png générés."
    else
        echo "   -> [VIDE] Pas de données générées pour ${MODE}."
    fi
done

echo "=== Terminé ! ==="
rm -f tmp/sorted_volume.tmp tmp/min50.dat tmp/max10.dat "$TMP_INPUT"
