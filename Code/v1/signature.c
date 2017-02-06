#include <string.h>
#include "structure.h"
#include "signature.h"

// ######
// 1. Print a signature
// ######



void print_vertexsignature(unsigned int sig){
  int id,label,index;
  label = sig & 0xff; sig = sig>>8;
  id = sig & 0xff; sig = sig>>8;
  index = sig & 0xff;
  printf("id of the vertex %d, label of the edge %d, index of the next vertex %d \n",id,label,index);
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
  //using char for rank seems very bad for performance
  int stack_length = 1, signature_equal = 1, current_vertex, current_edge, val;
  stack_vertex[0]= M->vertexarray[vertex_deb].edges[edge_deb].vertexindex;
  stack_edge[0] =  M->vertexarray[vertex_deb].edges[edge_deb].edgeindex;
  stack_vertex[1]= vertex_deb;
  stack_edge[1] = edge_deb;
  // DFS computing an element of the signature for every visited vertex
  for(int S_index = 1; S_index < M-> edgenumber; S_index++){
    current_vertex = stack_vertex[stack_length];
    current_edge = stack_edge[stack_length];
    stack_length--;
    if(!rank[current_vertex]){
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
    val = rank[current_vertex] + M->vertexarray[current_vertex].edges[current_edge].val;
    if (signature_equal && (val > M->signature[S_index])) { return;} // Signature is higher ==> stop
    signature_equal = signature_equal && (val == M->signature[S_index]); // When the signature is lower, we cannot stop early
    M->signature[S_index] = val;
  }
}

// ######
// 3. Computes the minimal signature of a map
// ######
void signature_compute(Map *M) {
  
  for (int vertex = 0 ; vertex<M->vertexnumber; vertex++){//could be computed during the construction of the map but less efficient
    for (int edge = 0 ; edge < M->vertexarray[vertex].degree ; edge++){
      M->vertexarray[vertex].edges[edge].val = (vertices[M->vertexarray[vertex].type].id<<16) + (vertices[M->vertexarray[vertex].type].edges[edge]<<8); 
    }
  }
  // Intialize the signature
  M->signature[0] = M->edgenumber; 
  M->signature[1] = 0x7fffffff; 	// The left most bit is always 0 in a signature. 
  // This initialization garanties that the first computed signature will be lower
  // Computes the signature for each starting position (vertex,edge)
  for (int vertex=0 ; vertex < M->vertexnumber ; vertex++){
    for (int edge=0 ; edge < M->vertexarray[vertex].degree ; edge++){
      signature_vertex_edge(M,vertex,edge);
    }
  }
}


