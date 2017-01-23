
Map *initializemap();
void free_map(Map *M);

unsigned long long int generate_trees(int verticesnumber,Hasht *h, Map *map, FreeEdge *outline);
unsigned long long int generate_paths(int verticesnumber, Hasht *h, Map *map, FreeEdge *outline);


