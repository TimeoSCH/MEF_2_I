#!/bin/bash

# Initialisation des variables [cite: 921]
EXEC="c-wildwater"
DATA_FILE="Data.csv"
# Utilisation de la substitution de commande $() [cite: 977]
START_TIME=$(date +%s)

# Définition de la fonction d'aide [cite: 1450]
show_help() {
    echo "Usage: $0 [histo max|src|real] | [leaks <FactoryID>]"
    echo "  histo max  : Histogramme de la capacité maximale de traitement."
    echo "  histo src  : Histogramme du volume total capté."
    echo "  histo real : Histogramme du volume réellement traité."
    echo "  leaks ID   : Calcul des fuites pour l'usine donnée."
}

# Fonction de fin de script avec calcul du temps [cite: 1022]
end_script() {
    # Récupération de l'argument (code de retour) [cite: 1445]
    RET_VAL=${1}
    END_TIME=$(date +%s)
    # Calcul arithmétique avec $(( ... )) [cite: 1022]
    DURATION=$((END_TIME - START_TIME))
    
    echo "Durée totale du traitement : ${DURATION} secondes."
    exit ${RET_VAL}
}

# Vérification de la présence de l'exécutable [cite: 1322]
# L'opérateur ! est séparé par un espace [cite: 1342]
if [ ! -f "${EXEC}" ] ; then
    echo "Compilation en cours..."
    make
    # Vérification du code retour de la commande précédente [cite: 740]
    if [ $? -ne 0 ] ; then
        echo "Erreur : La compilation a échoué."
        end_script 1
    fi
fi

# Vérification du nombre d'arguments [cite: 1445]
if [ $# -lt 1 ] ; then
    echo "Erreur : Commande incomplète."
    show_help
    end_script 1
fi

MODE="${1}"

# Utilisation de la structure case 
case "${MODE}" in
    "histo")
        # Vérification argument manquant (chaine vide) [cite: 1311]
        if [ -z "${2}" ] ; then
            echo "Erreur : Argument manquant pour 'histo'."
            end_script 1
        fi

        # Vérification argument invalide avec OR (||) [cite: 1107]
        if [ "${2}" != "max" ] && [ "${2}" != "src" ] && [ "${2}" != "real" ] ; then
            echo "Erreur : Argument '${2}' invalide."
            end_script 1
        fi

        SUB_MODE="${2}"
        OUTPUT_DAT="histo_${SUB_MODE}.dat"
        OUTPUT_IMG="histo_${SUB_MODE}.png"

        # Appel du programme C
        ./"${EXEC}" "${MODE}" "${SUB_MODE}" "${DATA_FILE}" "${OUTPUT_DAT}"
        
        # Vérification du succès du C
        if [ $? -ne 0 ] ; then
            echo "Erreur lors de l'exécution du programme C."
            end_script 1
        fi

        # Vérification existence du fichier de sortie [cite: 1320]
        if [ -f "${OUTPUT_DAT}" ] ; then
            gnuplot -e "
                set terminal png size 1000,600;
                set output '${OUTPUT_IMG}';
                set title 'Histogramme : ${SUB_MODE}';
                set style data histograms;
                set style fill solid 1.0 border -1;
                set boxwidth 0.7;
                set xtics rotate by -45;
                set datafile separator ';';
                set ylabel 'Volume (k m^3)';
                plot '${OUTPUT_DAT}' using 2:xtic(1) title '${SUB_MODE}' with boxes lc rgb 'light-blue';
            "
        else
            echo "Erreur : Fichier '${OUTPUT_DAT}' non généré."
            end_script 1
        fi
        ;;

    "leaks")
        if [ -z "${2}" ] ; then
            echo "Erreur : Identifiant manquant."
            end_script 1
        fi

        FACTORY_ID="${2}"
        
        ./"${EXEC}" "${MODE}" "${FACTORY_ID}" "${DATA_FILE}"
        
        if [ $? -ne 0 ] ; then
            echo "Erreur lors de l'exécution du programme C."
            end_script 1
        fi
        ;;

    *)
        echo "Erreur : Commande '${MODE}' inconnue."
        show_help
        end_script 1
        ;;
esac

end_script 0
