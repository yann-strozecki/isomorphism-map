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
  memcpy(rank,rank_init,sizeof(int)*mapsize);//initialize the rank array
  int stack_length = 1;
  stack_edge[0] = edge_deb;
  stack_edge[1] =  M->vertexarray[vertex_deb].edges[edge_deb].edgeindex;
  stack_vertex[0]= vertex_deb;
  stack_vertex[1]= M->vertexarray[vertex_deb].edges[edge_deb].vertexindex;;
  int signature_equal = 1;
  int current_vertex, current_edge, val;
  int S_index = 1;

  // DFS computing an element of the signature for every visited vertex
  while(stack_length >= 0){
    current_vertex = stack_vertex[stack_length];
    current_edge = stack_edge[stack_length];
    stack_length--;
    if(rank[current_vertex]==-1){
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
   val = (rank[current_vertex]<<16) + M->vertexarray[current_vertex].edges[current_edge].val;
    if (signature_equal && (val > M->signature[S_index])) { return;} // Signature is higher ==> stop
    signature_equal = signature_equal && (val == M->signature[S_index]); // When the signature is lower, we cannot stop early
    M->signature[S_index] = val;
    S_index++;
  }
}

//Variants with two loops, maybe very slightly more efficient

/*
void signature_vertex_edge(Map *M, int vertex_deb, int edge_deb) {
  // Rank is an array that gives for each vertex its rank in the signature, -1 if the vertex has no rank yet
  memcpy(rank,rank_init,sizeof(int)*mapsize);//initialize the rank array
  int stack_length = 1;
  stack_edge[0] = edge_deb;
  stack_edge[1] =  M->vertexarray[vertex_deb].edges[edge_deb].edgeindex;
  stack_vertex[0]= vertex_deb;
  stack_vertex[1]= M->vertexarray[vertex_deb].edges[edge_deb].vertexindex;;
  int current_vertex, current_edge, val;
  int S_index = 0;

  // DFS computing an element of the signature for every visited vertex
  do{//the first loop search the first value different from the minimum
    S_index++;
    current_vertex = stack_vertex[stack_length];
    current_edge = stack_edge[stack_length];
    stack_length--;
    if(rank[current_vertex]==-1){
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
   val = (rank[current_vertex]<<16) + M->vertexarray[current_vertex].edges[current_edge].val;
    if (val > M->signature[S_index]) { return;}// Signature is higher ==> stop
  }while(stack_length >= 0 && M->signature[S_index] == val);

  //if we go out of the first loop, then we have found a value which a signature striclty less than the current minimum
  M->signature[S_index] = val;


  while(stack_length >= 0){//ne minimum value, compute it
    S_index++;
    current_vertex = stack_vertex[stack_length];
    current_edge = stack_edge[stack_length];
    stack_length--;
    if(rank[current_vertex]==-1){
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
    M->signature[S_index] = (rank[current_vertex]<<16) + M->vertexarray[current_vertex].edges[current_edge].val;
  }

}
*/
//We should test again the alternate DFS implementation without stack to compare performance

// ######
// 3. Computes the minimal signature of a map
// ######
void signature_compute(Map *M) {
  for (int vertex = 0 ; vertex<M->vertexnumber; vertex++){//could be computed during the construction of the map but less efficient
    for (int edge = 0 ; edge < M->vertexarray[vertex].degree ; edge++){
      M->vertexarray[vertex].edges[edge].val = (vertices[M->vertexarray[vertex].type].id<<8) + vertices[M->vertexarray[vertex].type].edges[edge]; 
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


