#include "structure.h"


void print_signature(Map *M);

// Computes the minimal signature of M
void signature_compute(Map *M);

// Computes the signature starting at vertex_deb, edge_deb
void signature_vertex_edge(Map *M, int vertex_deb, int edge_deb);

// Helps to define a signature filter
void set_rarest_pair(int* edge_min, int* edge_min_nb, int vertexnumber);
