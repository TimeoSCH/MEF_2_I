#!/bin/bash

# Configuration par défaut
DATA_FILE="data.csv"
EXECUTABLE="./c-wire"
MAKEFILE="Makefile"

# Fonction d'aide
show_help() {
    echo "Usage: $0 <commande> <option> [fichier_csv]"
    echo ""
    echo "Commandes:"
    echo "  histo max|src|real      : Génère l'histogramme (50 min + 10 max)."
    echo "  leaks <id_station>      : Calcule les fuites pour une station donnée."
    echo ""
    echo "Options:"
    echo "  [fichier_csv]           : Chemin vers le fichier de données."
    echo ""
    echo "Exemples:"
    echo "  $0 histo max water.dat"
    echo "  $0 leaks 'Facility complex #RH400057F' water.dat"
}

# Vérification des arguments
if [ "$1" == "-h" ] || [ "$#" -lt 2 ]; then
    show_help
    exit 1
fi

COMMANDE="$1"
OPTION="$2"

# Gestion du fichier de données (3ème argument optionnel)
if [ -n "$3" ]; then
    DATA_FILE="$3"
fi

# Vérifications de sécurité
if [ ! -f "$DATA_FILE" ]; then
    echo "Erreur : Le fichier de données '$DATA_FILE' est introuvable."
    exit 1
fi

if [ ! -f "$MAKEFILE" ]; then
    echo "Erreur : Makefile introuvable. Veuillez créer le Makefile avant de lancer."
    exit 2
fi

# Compilation si nécessaire
if [ ! -x "$EXECUTABLE" ]; then
    echo "Compilation en cours..."
    make
    if [ $? -ne 0 ]; then
        echo "Erreur : La compilation a échoué."
        exit 2
    fi
fi

# Création des dossiers temporaires et de sortie
mkdir -p graphs
mkdir -p tmp

# --- TRAITEMENT HISTOGRAMMES ---
if [ "$COMMANDE" == "histo" ]; then
    
    echo "Traitement des données en mode '$OPTION'..."
    
    # Exécution du programme C
    "$EXECUTABLE" "$DATA_FILE" "$COMMANDE" "$OPTION"
    if [ $? -ne 0 ]; then
        echo "Erreur lors de l'exécution du programme C."
        exit 3
    fi

    if [ ! -f "stats.csv" ]; then
        echo "Erreur : Le fichier 'stats.csv' n'a pas été généré."
        exit 3
    fi

    # Nom du fichier de sortie
    OUTPUT_IMG="graphs/vol_${OPTION}.png"

    echo "Génération du graphique : $OUTPUT_IMG"

    # --- LOGIQUE DE TRI (50 min / 10 max) ---
    
    # 1. On trie le CSV numériquement par la 2ème colonne (séparateur ;)
    # tail -n +2 permet d'ignorer la ligne d'en-tête "identifier;..."
    tail -n +2 stats.csv | sort -t";" -k2,2n > tmp/sorted_data.tmp

    # 2. Vérification : A-t-on assez de données ?
    NB_LIGNES=$(wc -l < tmp/sorted_data.tmp)
    
    if [ "$NB_LIGNES" -le 60 ]; then
        # Si moins de 60 lignes, on prend tout
        cat tmp/sorted_data.tmp > tmp/data_plot.dat
    else
        # 3. Extraction des 50 plus petits (début du fichier trié)
        head -n 50 tmp/sorted_data.tmp > tmp/min_50.tmp
        
        # 4. Extraction des 10 plus grands (fin du fichier trié)
        tail -n 10 tmp/sorted_data.tmp > tmp/max_10.tmp
        
        # 5. Fusion pour Gnuplot
        cat tmp/min_50.tmp tmp/max_10.tmp > tmp/data_plot.dat
    fi

    # --- GNUPLOT ---
    # Note : On augmente la taille (size 2500,800) pour afficher 60 barres proprement
    gnuplot -persist <<-EOF
        set terminal png size 2500,800 enhanced font "arial,10"
        set output '$OUTPUT_IMG'
        set title 'Histogramme : 50 plus faibles et 10 plus forts volumes ($OPTION)'
        set datafile separator ";"
        set style data histograms
        set style fill solid 1.0 border -1
        set boxwidth 0.7
        
        # Rotation des étiquettes en bas pour qu'elles ne se chevauchent pas
        set xtics rotate by -45 scale 0 font ",8"
        
        set ylabel 'Volume (m3)'
        set grid ytics
        set xlabel 'Stations (Identifiants)'
        
        # Marge en bas augmentée pour les noms longs
        set bmargin 10
        
        # On trace le graph. 
        # using 2:xtic(1) -> colonne 2 pour la hauteur, colonne 1 pour le nom
        plot 'tmp/data_plot.dat' using 2:xtic(1) notitle linecolor rgb "forest-green"
EOF

    echo "Terminé. Graphique disponible ici : $OUTPUT_IMG"


# --- TRAITEMENT LEAKS (FUITES) ---
elif [ "$COMMANDE" == "leaks" ]; then
    ID_LEAK="$OPTION"
    if [ -z "$ID_LEAK" ]; then
        echo "Erreur : L'identifiant de la station est manquant."
        exit 1
    fi

    echo "Recherche des fuites pour $ID_LEAK..."
    "$EXECUTABLE" "$DATA_FILE" "$COMMANDE" "$ID_LEAK"
    
    # Gestion simple de l'historique des fuites (optionnel selon votre sujet)
    OUTPUT_LEAKS="leaks_history.dat"
    if [ -f "stats.csv" ]; then
        cat stats.csv >> "$OUTPUT_LEAKS"
        echo "Résultats ajoutés à $OUTPUT_LEAKS"
    else
        echo "Aucune donnée trouvée ou erreur programme."
    fi

else
    show_help
    exit 1
fi
