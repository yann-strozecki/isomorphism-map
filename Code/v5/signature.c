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
   		val = (rank[current_vertex]<<24) + M->vertexarray[current_vertex].edges[current_edge].val;
    	if (signature_equal && (val < M->signature[S_index])) { return;} // Signature is higher ==> stop
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
// 3. Face size labelling.
// Description: given a vertex and an adjacent edge, compute the size (in edges) of the face associated with this edge. 
// And update val of all edges encountered.
// ######


void compute_face(Map *M, int vertex_deb, int edge_deb) {
  int face_size = 0;
  int nextvertex;
  int currentvertex = vertex_deb, currentedge = edge_deb;
  do{
    val_addr[face_size] = &(M->vertexarray[currentvertex].edges[currentedge].val);
    face_size++;//update face size
    nextvertex = M->vertexarray[currentvertex].edges[currentedge].vertexindex;// update the current vertex and edge
    currentedge = M->vertexarray[currentvertex].edges[currentedge].edgeindex + 1;
    currentvertex = nextvertex;
    if(currentedge == M->vertexarray[currentvertex].degree) currentedge = 0;//modulo degree without %
  }  while(currentvertex != vertex_deb || currentedge != edge_deb);
  int index;
  int val_add = face_size*different_vertex*label.size;
  for(int i=0; i < face_size; i++){
    *val_addr[i] += val_add; //update val
    index = *val_addr[i];
    //printf("vertex=%i,edge=%i,face_size=%i\n",vertex,edge,face_size);
    if (occurences_array[index] == 0){
      occ_addr_array[index] = ini_addr;//update a chain of already seen elements
      ini_addr = index;//update head of chain
      //printf("Seen index %d\n",index);
    }
    occurences_array[index]++;//get occurences of each triple (face_size, vertex id, edge label)
  }
}


/* This version with two loops is slower than the one with a single loop but some additional adress storing
void compute_face(Map *M, int vertex_deb, int edge_deb) {
    int face_size = 0;
    int nextvertex;
    int currentvertex = vertex_deb, currentedge = edge_deb;
    do{
        //val_addr[face_size] = &M->vertexarray[currentvertex].edges[currentedge].val;
        face_size++;//update face size
        nextvertex = M->vertexarray[currentvertex].edges[currentedge].vertexindex;// update the current vertex and edge
        currentedge = M->vertexarray[currentvertex].edges[currentedge].edgeindex + 1;
        currentvertex = nextvertex;
        if(currentedge == M->vertexarray[currentvertex].degree) currentedge = 0;//modulo degree without %
    }  while(currentvertex != vertex_deb || currentedge != edge_deb);
    int val = face_size<<16;
    for(int i=0; i < face_size; i++){
		M->vertexarray[currentvertex].edges[currentedge].val += val;
		nextvertex = M->vertexarray[currentvertex].edges[currentedge].vertexindex;// update the current vertex and edge
        currentedge = M->vertexarray[currentvertex].edges[currentedge].edgeindex + 1;
        currentvertex = nextvertex;
        if(currentedge == M->vertexarray[currentvertex].degree) currentedge = 0;
    }
}*/



// ######
// 4. Computes the minimal signature of a map
// ######
void signature_compute(Map *M) {
  //printf("insertion of a new map with maxindex %d\n",maxindex);
	for (int vertex = 0 ; vertex < M->vertexnumber; vertex++){
  	for (int edge = 0 ; edge < M->vertexarray[vertex].degree ; edge++){
    	M->vertexarray[vertex].edges[edge].val = (vertices[M->vertexarray[vertex].type].id*label.size + vertices[M->vertexarray[vertex].type].edges[edge]); 
  	}
  }//initialization of val by type of edge and vertex
  ini_addr = -1;
	for(int vertex = 0 ; vertex<M->vertexnumber; vertex++){
    for (int edge = 0 ; edge < M->vertexarray[vertex].degree ; edge++){
      if(!(M->vertexarray[vertex].edges[edge].val << different_vertex*label.size)) compute_face(M,vertex,edge);
    }//compute the size of the face if not yet computed
  }//to minimize computation the in the signature the value of the different components are 
  //inversed but one seeks instead the maximal signature so that everything is comparable


  int min_occurence = mapsize*maxdegree;
  int min_index = maxindex; // We need to ensure unicity of rarest triple selection (besides, selects small faces, which is intuitively better of signature discrimination)
  int occurences;
  while (ini_addr != -1){//-1 marks the end of the list 
    occurences = occurences_array[ini_addr];
    if (occurences < min_occurence || (occurences == min_occurence && ini_addr < min_index)){//lexicographic order on (occurences,index) to ensure unicity of triple selection
      min_occurence = occurences;
      min_index = ini_addr;
    }
    occurences_array[ini_addr] = 0;
    ini_addr = occ_addr_array[ini_addr];//no need to reinitialize this list
  }
  int rarest_val = min_index;

	// Intialize the signature
	M->signature[0] = M->edgenumber; 
	M->signature[1] = 0;//0x7fffffff; 	// The left most bit is always 0 in a signature. 
	// This initialization garanties that the first computed signature will be lower
	// Computes the signature for each starting position (vertex,edge)
	for (int vertex=0 ; vertex < M->vertexnumber ; vertex++){
  	for (int edge=0 ; edge < M->vertexarray[vertex].degree ; edge++){
    	if(M->vertexarray[vertex].edges[edge].val == rarest_val)
        signature_vertex_edge(M,vertex,edge);
  	}
	}
}





