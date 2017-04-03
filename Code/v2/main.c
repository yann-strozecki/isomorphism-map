#include <math.h>
#include <unistd.h>
#include "structure.h"
#include "input.h"
#include "concat.h"
#include "hash.h"
#include "signature.h"
#include "chrono.h"

void prettyprint(unsigned long long int  x) { //affichage à mettre dans un seul fichier et à améliorer
	int t,g,m,k,u;
	t = x/1e12; x = x-t*1e12;
	g = x/1e9;  x = x-g*1e9;
	m = x/1e6;  x = x-m*1e6;
	k = x/1e3;  u = x-k*1e3;
	if (t) printf("%5d,%03d,%03d,%03d,%03d",t,g,m,k,u);
	else if (g) printf("      %3d,%03d,%03d,%03d",g,m,k,u);
	else if (m) printf("          %3d,%03d,%03d",m,k,u);
	else if (k) printf("              %3d,%03d",k,u);
	else        printf("                  %3d",u);
}

int main (int argc, char *argv[])
{
	int i,j;
	printf("## START ##\n");
	chrono();
	if (argc < 3)  { fprintf(stderr,"usage: %s mot_file size\n",argv[0]); exit(1); }
	mapsize = atoi(argv[2]);

	printf("1. Read of the input file: %s, Size = %d. \n Creation of the data structures.\n",argv[1],mapsize);


	int vertexnumber;//could be in a struct (maybe with the list of label and its size, and 
  //the list of Vertices and  its size)
	vertexnumber = read_input (argv[1], &maxdegree);
  //printvertices(vertexnumber,vertices);
	normalize_labels(vertexnumber, vertices);
  compute_label_values(vertexnumber, vertices);
  //for(i=0;i<label.size;i++) printf("label %d : %d ",i,label.list[i]);
  //printvertices(vertexnumber,vertices);
	vertices = create_rotated_vertices(&vertexnumber,vertices);
  //printvertices(vertexnumber,vertices);
	create_concatenation_helper(vertexnumber, vertices);
  //printcompatible(label.size);
  if(BACKBONETYPE){
  	almost_foldable_path(vertexnumber, vertices);
  }else{
  	almost_foldable_tree(vertexnumber, vertices);
  }

  /********* structures useful for the concat and the fold, memory allocation once and for all here **********/

  //Some of the memory allocation are used for several variables which are not used at the same time

  Map *M = initializemap();//create the map 
  Hasht *h = initialize_hasht();
  maxoutlinesize = (maxdegree-1)*mapsize;
  FreeEdge *outline = malloc(maxoutlinesize*sizeof(FreeEdge));
  if (!outline) { fprintf(stderr,"cannot map.outline in concat.c\n"); exit(41); }
  previous = malloc(maxoutlinesize*sizeof(int));// the same memory is used for several arrays
  mat = malloc(maxoutlinesize*maxoutlinesize*sizeof(int));
  foldable_with = previous;
  fold_matrix = malloc((maxoutlinesize-1)*sizeof(int*));//compute the array for the dynamic programming
  if (!fold_matrix) { fprintf(stderr,"cannot malloc res in fold.c\n"); exit(51); }
  for(i=0; i < maxoutlinesize-1;i++) 
  {
  	fold_matrix[i]=malloc(sizeof(int)*(maxoutlinesize/2+1));
  	if (!fold_matrix[i]) { fprintf(stderr,"cannot malloc res[%d] in fold.c\n",i); exit(52);}
  }
  to_fold = malloc(maxoutlinesize/2*sizeof(pair));
  if (!to_fold) { fprintf(stderr,"cannot malloc to_fold in fold.c\n"); exit(53); }
  folded = malloc(maxoutlinesize/2*sizeof(pair));
  if (!folded) { fprintf(stderr,"cannot malloc folded in fold.c\n"); exit(54); }
  call_stack = previous;

  /**************************** Allocation of the structures used to compute the signature ******************/
  
  rank = malloc(mapsize*sizeof(int)); 
  stack_edge = malloc(mapsize*maxdegree*sizeof(int));
  stack_vertex = malloc(mapsize*maxdegree*sizeof(int));
  val_addr = malloc(mapsize*maxdegree*sizeof(int*));

  /************************************** The generation procedure *************************************************/
  chrono();

  printf("2. Build of the maps\n");

  unsigned long long int backbonenumber;
  if(BACKBONETYPE){
  	backbonenumber = generate_paths(vertexnumber, h, M, outline);
  }else{
  	backbonenumber =  generate_trees(vertexnumber, h, M, outline);
  }
  printf("\r\033[2K   Nb backbones:"); prettyprint(backbonenumber); printf("\n");
  printf("  Nb folded maps    : "); prettyprint(h->insertions); printf("\n");
  printf("  Nb unique maps    : "); prettyprint(h->elements); printf("\n");
  chrono();

  /*********************************** Free of all allocated memory ****************************/ 
  printf("3. Free of the memory\n");
  
  affiche_hasht(h);
  free_hasht(h);
  //Immutable structure
  
  if(BACKBONETYPE){
  	for (i=0 ; i < mapsize ; i++) {
  		for (j=0 ; j<label.size ; j++) {free(almostfoldablepath[i][j]);}
  		free(almostfoldablepath[i]);
  	}
  	free(almostfoldablepath);
  }
  else{
  	for (i = 0 ; i < mapsize ; i++) {free(almostfoldabletree[i]);}
  		free(almostfoldabletree);
  }

  for (i=0 ; i < vertexnumber ; i++) free(vertices[i].edges);
  free(vertices);
  for (i=0 ; i<label.size ; i++) free(connection[i].list);
  free(connection);
  free(label.list);

  // mutable structures used for the map generation or index computation
  free(outline);
  free_map(M);
  free(previous);
  free(mat);
  free(to_fold);
  free(folded);
  for (i=0 ; i<maxoutlinesize-1 ; i++) {free(fold_matrix[i]);}
  free(fold_matrix);
  free(rank);
  free(stack_edge);
  free(stack_vertex);
  free(val_addr);
  chrono();
  printf("## END ##\n");
return 0;
}
