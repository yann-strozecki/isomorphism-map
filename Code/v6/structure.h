#include <stdio.h>
#include <stdlib.h>


#ifndef ___STRUCTURE_H
#define ___STRUCTURE_H

#define BACKBONETYPE 0 // 0 for generating tree, 1 for generating path, 2 for generating cycle
#define HASH_SIZE 1000000 //augmenter pour diminuer les collisions
#define HEAP_SIZE 1000000 //stockage des signatures


typedef struct elem{
    struct elem* next;
} elem; 

typedef struct{
  int heap_level;
    elem* memory[16];//store in this order the next pointer, the size of the signature and the signature
  elem* next_position;
    int avalaible_space;
} Heap;

typedef struct{
  elem** hash;
  unsigned int elements;
  unsigned int insertions;
  unsigned int nombre_map; 
  Heap* hp;
} Hasht;  

typedef struct pair {
	int first;
	int second;
} pair;

typedef struct Label {
	int *list;// array of the value of the labels in the .mot (only the positive ones)
	int size; //number of different labels
	int shift;//shift of the label value, computed at the same time as maxlabelnumber 
} Label; //Label and compatibleVertices could be unified


typedef struct Vertex {  //Used in an array of all possible vertices is built at the beginning of the algorithm 
	int letter; // Integer that gives the rank of the letter in the alphabet 9 for I, 10 for J, 22 for V, ...
	int degree; // Number of edges
	int id; // Position in the .mot file 
	int *edges; // Array containing the label of the edges
	int labelvalue;//encode the vector of number of each type of labels
} Vertex;


typedef struct CompatibleVertices {
	int *list; //array of the index of vertices which can be connected to some letter by their first edge 
	int vertexnumber; //size of the list
} CompatibleVertices;


// The outline structure //
typedef struct FreeEdge {  //a free edge in the graph
	int label; //number which labels the edge
	int vertexindex;  //index of the corresponding vertex in the vertexarray of the  Map
	int edgeindex; //number of the edge in the edges array  
} FreeEdge; //to merge with Edge

// The Map Structure//
typedef struct Edge {
	int vertexindex;//vertex connected to the edge
	int edgeindex;//position of this edge in the neighborood of the end vertex
  int val;//partial signature containing (edge label, vertex id)
} Edge; 

typedef struct VertexMap {
  int degree;//degree of the vertex
  int type;//position in the list of vertices of type Vertex -> should be id
  Edge *edges;//array of the neighbour given by their index in VertexMap 
} VertexMap;//we should store the id instead of the type since it is what we really want



typedef struct Map{
  int vertexnumber; //number of vertices in the Map 
  int edgenumber; // Size of the signature -> renommer en signature size
  VertexMap *vertexarray;//Array of the vertices with their neighbours
  unsigned int *signature;
} Map;	

//All variables defined here globally correspond to unique instance of their type. They are used to store immutable informations all along the algorithm.
//Séparer les immutables des justes uniques

Vertex *vertices;// Vertices given in the .mot and their rotations
Label label; //Label of the edges
CompatibleVertices *connection;//helper structure to do the concatenation fast
unsigned int *** almostfoldablepath;//structure to cut in the enumeration of paths
unsigned int ** almostfoldabletree;//structure to cut in the enumeration of trees
int mapsize,maxdegree,maxoutlinesize;
int *previous,*mat,*foldable_with,**fold_matrix,*call_stack;
pair *to_fold,*folded;
int *rank, *stack_edge, *stack_vertex;

#endif
