#include "structure.h"
#include "signature.h"


// add a new map in the Hashtable
// if a Map with the same signature already exist in the Hashtable, nothing is added 
void hash_insert(Hasht* h, Map *M);
Hasht* initialize_hasht();
void free_hasht(Hasht* h);
void affiche_hasht(Hasht *h);
