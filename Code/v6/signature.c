#include <string.h>
#include "structure.h"
#include "signature.h"

// ######
// 1. Print a signature
// ######



void print_vertexsignature(unsigned int sig){
  int label,index;
  index = sig & 0xffff; label = sig>>16;
  printf(" label of the edge %d, index of the vertex %d \n",label,index);
}


void print_signature(Map *M) {
  
  printf("%d edges\n Début de la signature \n",M->edgenumber);
  for (int i=0 ; i<M->edgenumber ; i++) {
   print_vertexsignature(M->signature[i]);
 }
 printf("Fin de la signature \n \n");
}

// ######
// 2. Computes the signature starting from the vertex and the edge given in args.
// ######


void signature_vertex_edge(Map *M, int vertex_deb, int edge_deb) {
  // Rank is an array that gives for each vertex its rank in the signature, -1 if the vertex has no rank yet
  memset(rank,0,sizeof(int)*mapsize);
  int stack_length = 1;
  stack_edge[0] = edge_deb;
  stack_edge[1] =  M->vertexarray[vertex_deb].edges[edge_deb].edgeindex;
  stack_vertex[0]= vertex_deb;
  stack_vertex[1]= M->vertexarray[vertex_deb].edges[edge_deb].vertexindex;;
  int current_vertex, current_edge;
  int S_index = 1;

  // DFS computing an element of the signature for every visited vertex
  while(stack_length >= 0){
    current_vertex = stack_vertex[stack_length];
    current_edge = stack_edge[stack_length];
    stack_length--;
    if( !rank[current_vertex]){
      rank[current_vertex] = S_index;//index a new found vertex
      for(int i = current_edge+1; i< M->vertexarray[current_vertex].degree;i++){//add the neighborood of a new vertex to the stack of vertices to visit
        stack_length++;
        stack_vertex[stack_length] = M->vertexarray[current_vertex].edges[i].vertexindex;
        stack_edge[stack_length] = M->vertexarray[current_vertex].edges[i].edgeindex;
      }
      for(int i = 0; i< current_edge;i++){
        stack_length++;
        stack_vertex[stack_length] = M->vertexarray[current_vertex].edges[i].vertexindex;
        stack_edge[stack_length] = M->vertexarray[current_vertex].edges[i].edgeindex;
      }
    }
    M->signature[S_index] = rank[current_vertex] + (vertices[M->vertexarray[current_vertex].type].edges[current_edge]<<16); //no need for 
//color of vertices and precomputation of colors
    S_index++;
  }
  //print_signature(M);
}


