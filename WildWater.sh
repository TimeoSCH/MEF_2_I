#!/bin/bash

# Configuration
DATA_FILE="water.dat"  # Par défaut water.dat (plus courant pour vous)
EXECUTABLE="./c-wire"
MAKEFILE="Makefile"

# --- Fonctions ---

show_help() {
    echo "Usage: $0 [fichier_dat]"
    echo "Ce script génère automatiquement les 6 histogrammes demandés :"
    echo " - 3 catégories (max, src, real)"
    echo " - 2 graphiques par catégorie (Top 10 et Min 50)"
    echo ""
    echo "Exemple: $0 water.dat"
}

# Fonction pour générer un graphique avec Gnuplot
generer_graphique() {
    local input_dat="$1"
    local output_png="$2"
    local titre="$3"
    local color="$4"

    # Vérifie si le fichier est vide ou trop petit
    if [ ! -s "$input_dat" ]; then
        echo "Attention : Pas de données pour $output_png"
        return
    fi

    gnuplot -persist <<-EOF
        set terminal png size 1600,900 enhanced font "arial,12"
        set output '$output_png'
        set title '$titre'
        set datafile separator ";"
        set style data histograms
        set style fill solid 1.0 border -1
        set boxwidth 0.7
        
        # Axe X : Rotation pour lisibilité
        set xtics rotate by -45 scale 0 font ",9"
        set xlabel 'Stations'
        
        set ylabel 'Volume (m3)'
        set grid ytics
        
        # Marges pour éviter de couper les noms
        set bmargin 12
        
        plot '$input_dat' using 2:xtic(1) notitle linecolor rgb "$color"
EOF
}

# --- Vérifications ---

# Compilation
if [ ! -x "$EXECUTABLE" ]; then
    echo "Compilation..."
    if [ ! -f "$MAKEFILE" ]; then
        echo "Erreur : Makefile absent."
        exit 1
    fi
    make
    if [ $? -ne 0 ]; then
        echo "Erreur compilation."
        exit 1
    fi
fi

# Gestion argument fichier
if [ -n "$1" ]; then
    if [ "$1" == "-h" ]; then show_help; exit 0; fi
    DATA_FILE="$1"
fi

if [ ! -f "$DATA_FILE" ]; then
    echo "Erreur : Fichier '$DATA_FILE' introuvable."
    exit 1
fi

mkdir -p graphs
mkdir -p tmp

echo "=== Lancement de la génération des 6 histogrammes ==="

for MODE in "max" "src" "real"; do
    echo "Traitement du mode : $MODE"
    
    # 1. Exécution du C
    # On passe "histo" et le mode (ex: max)
    "$EXECUTABLE" "$DATA_FILE" "histo" "$MODE"
    
    if [ ! -f "stats.csv" ]; then
        echo "Erreur : stats.csv non généré pour $MODE"
        continue
    fi

    # 2. Tri global (numérique sur colonne 2)
    # On enlève l'entête avec tail +2
    tail -n +2 stats.csv | sort -t";" -k2,2n > tmp/sorted_global.tmp

    # --- PARTIE 1 : Les 50 plus petites ---
    head -n 50 tmp/sorted_global.tmp > tmp/data_min50.dat
    # Génération Graphe
    generer_graphique "tmp/data_min50.dat" "graphs/${MODE}_min50.png" "Mode $MODE : 50 plus petites stations" "forest-green"

    # --- PARTIE 2 : Les 10 plus grandes ---
    tail -n 10 tmp/sorted_global.tmp > tmp/data_max10.dat
    # Génération Graphe (en rouge pour les max)
    generer_graphique "tmp/data_max10.dat" "graphs/${MODE}_max10.png" "Mode $MODE : 10 plus grosses stations" "firebrick"

    echo " -> Généré : graphs/${MODE}_min50.png et graphs/${MODE}_max10.png"
done

echo "=== Terminé ! 6 graphiques créés dans le dossier 'graphs/' ==="
