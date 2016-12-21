int read_input(char* inputname, int *maxdegree);

int read_concat(char* inputname, Map *translation);

void normalize_labels(int size, Vertex *vert);

Vertex * create_metavertices(int motifnumber);

Vertex *create_rotated_vertices(int size,int *newsize, Vertex *vert);

void create_concatenation_helper(int size,  Vertex * vert);

void printcompatible(int size);

void printvertices(int size, Vertex *vert);

int non_isomorph(int *list, int size, int shift);

void almost_foldable_tree(int vertexnumber, Vertex *vert);

void almost_foldable_path(int vertexnumber, Vertex *vert);
