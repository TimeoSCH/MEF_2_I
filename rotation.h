#include "wildwater.h"

Arbre* rotationGauche(Arbre *a) {
    Arbre *pivot;
    int eq_a, eq_p;
    pivot = a->fd;
    a->fd = pivot->fg;
    pivot->fg = a;
    eq_a = a->equilibre;
    eq_p = pivot->equilibre;
    a->equilibre = eq_a - (eq_p > 0 ? eq_p : 0) - 1;
    int v1 = eq_a - 2;
    int v2 = eq_a + eq_p - 2;
    int v3 = eq_p - 1;
    int min_val = v1;
    if (v2 < min_val) min_val = v2;
    if (v3 < min_val) min_val = v3;
    pivot->equilibre = min_val;

    return pivot;
}


Arbre* rotationDroite(Arbre *a) {
    Arbre *pivot;
    int eq_a, eq_p;
    pivot = a->fg;
    a->fg = pivot->fd;
    pivot->fd = a;
    eq_a = a->equilibre;
    eq_p = pivot->equilibre;
    a->equilibre = eq_a - (eq_p < 0 ? -eq_p : 0) - 1;
    int v1 = eq_a - 2;
    int v2 = eq_a + eq_p - 2;
    int v3 = eq_p - 1;
    int min_val = v1;
    if (v2 < min_val) min_val = v2;
    if (v3 < min_val) min_val = v3;
    pivot->equilibre = min_val;

    return pivot;
}


Arbre* doubleRotationGauche(Arbre* a){
   a->fd=rotationDroite(a->fg);
   a=rotationGauche(a);
   return a;
}


Arbre* doubleRotationDroite(Arbre* a){
   a->fg=rotationGauche(a->fg);
   a=rotationDroite(a);
   return a;
}


Arbre* equilibreAVL(Arbre* a){
  if(a->equilibre>=2){
     if(a->fd->equilibre>=0){
        return rotationGauche(a);
     }else{
        return doubleRotationGauche(a);
     }
  }else if(a->equilibre<=-2){
      if(a->fg->equilibre<=0){
         return rotationDroite(a);
      }else{
         return doubleRotationDroite(a);
      }
  }
  return a;
}
         
