#include "structures.h"
struct Station;

typedef struct Chainon {
    struct Station* station; 
    struct Chainon* suivant; 
} Chainon;

typedef struct Station {
    char id_str[50];        
    long capacity;          
    long consumption;       
    double leak_percent;    
    Chainon* listeEnfants;  
} Station;

#endif
