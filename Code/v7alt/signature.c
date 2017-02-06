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
	int stack_length = 2, current_vertex,current_edge;
  stack_sig[1].first = vertex_deb;
  stack_sig[1].second = edge_deb;
  stack_sig[0].first = M->vertexarray[vertex_deb].edges[edge_deb].vertexindex;
  stack_sig[0].second =  M->vertexarray[vertex_deb].edges[edge_deb].edgeindex;
	rank[M->vertexarray[vertex_deb].edges[edge_deb].vertexindex] = 1;
  rank[vertex_deb] = 2;
  int ranknumber = 2;
	int shift;
	for(int i = 0; i < M->vertexnumber; i++){
    stack_length--;
    current_vertex = stack_sig[stack_length].first;
    current_edge = stack_sig[stack_length].second;
  	M->signature[i] =  vertices[M->vertexarray[current_vertex].type].localtype[current_edge];//encode the type of
  	//the vertex and the edge which has been used to access it on the first 16 bits
  	shift = 16;
  	for(int j = current_edge +1 ; j < M->vertexarray[current_vertex].degree ;j++){
  		//on encode le rang des sommets sur 8 bits, au plus degré 6 (6*8 +16)
  		if(rank[M->vertexarray[current_vertex].edges[j].vertexindex] == 0){	
  			rank[M->vertexarray[current_vertex].edges[j].vertexindex] = ++ranknumber;
      	stack_sig[stack_length].first = M->vertexarray[current_vertex].edges[j].vertexindex;
      	stack_sig[stack_length].second = M->vertexarray[current_vertex].edges[j].edgeindex;
      	stack_length++;
  		}
  		shift +=8;
  		M->signature[i] |= rank[M->vertexarray[current_vertex].edges[j].vertexindex] << shift;
	  }
	  for(int j = 0; j < current_edge ;j++){
  		if(rank[M->vertexarray[current_vertex].edges[j].vertexindex] == 0){
  			rank[M->vertexarray[current_vertex].edges[j].vertexindex] = ++ranknumber;
      	stack_sig[stack_length].first = M->vertexarray[current_vertex].edges[j].vertexindex;
      	stack_sig[stack_length].second = M->vertexarray[current_vertex].edges[j].edgeindex;
      	stack_length++;
  		}
  	 shift +=8;
  	 M->signature[i] |= rank[M->vertexarray[current_vertex].edges[j].vertexindex] << shift;
	  }
  
  }
}


/*

void signature_vertex_edge(Map *M, int vertex_deb, int edge_deb) {
  // Rank is an array that gives for each vertex its rank in the signature, -1 if the vertex has no rank yet
  memset(rank,0,sizeof(int)*mapsize);
  int stack_length = 1;
  stack_sig[0].first = vertex_deb;
  stack_sig[0].second = edge_deb;
  stack_sig[1].first = M->vertexarray[vertex_deb].edges[edge_deb].vertexindex;
  stack_sig[1].second =  M->vertexarray[vertex_deb].edges[edge_deb].edgeindex;
  int current_edgen, current_vertexn;
  VertexMap *current_vertex;
  //précalcul de M->vertexarray[current_vertex] ?
  // DFS computing an element of the signature for every visited vertex
  for(int S_index = 1;S_index < M->edgenumber; S_index++){
  	current_vertexn = stack_sig[stack_length].first;
    current_edgen = stack_sig[stack_length].second;
    current_vertex = &(M->vertexarray[current_vertexn]);
    stack_length--;
    if(!rank[current_vertexn]){
      rank[current_vertexn] = S_index;//index a new found vertex
      for(int i = current_edgen + 1; i< current_vertex->degree;i++){//add the neighborood of a new vertex to the stack of vertices to visit
        stack_length++;
        stack_sig[stack_length].first = current_vertex->edges[i].vertexindex;
        stack_sig[stack_length].second = current_vertex->edges[i].edgeindex;
      }
      for(int i = 0; i< current_edgen;i++){
        stack_length++;
        stack_sig[stack_length].first = current_vertex->edges[i].vertexindex;
        stack_sig[stack_length].second = current_vertex->edges[i].edgeindex;
      }
      
    }
    M->signature[S_index] = rank[current_vertexn] + (vertices[current_vertex->type].edges[current_edgen]<<16); //no need for 
//color of vertices and precomputation of colors
  }
  //print_signature(M);
}
*/

