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
   		val = (rank[current_vertex]) + M->vertexarray[current_vertex].edges[current_edge].val;
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
// 3. Face size labelling.
// Description: given a vertex and an adjacent edge, compute the size (in edges) of the face associated with this edge. 
// And update val of all edges encountered.
// ######

/*
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
    int val = face_size<<8;
    for(int i=0; i < face_size; i++){
        *val_addr[i] += val; //update val
    }
}
*/

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
  
  	for (int vertex = 0 ; vertex<M->vertexnumber; vertex++){
    	for (int edge = 0 ; edge < M->vertexarray[vertex].degree ; edge++){
      		M->vertexarray[vertex].edges[edge].val = (vertices[M->vertexarray[vertex].type].id<<24) + (vertices[M->vertexarray[vertex].type].edges[edge]<<16); 
    	}
  	}//initialization of val by type of edge and vertex
 


  	// Intialize the signature
  	M->signature[0] = M->edgenumber; 
  	M->signature[1] = 0x7fffffff; 	// The left most bit is always 0 in a signature. 
  	// This initialization garanties that the first computed signature will be lower
  	// Computes the signature for each starting position (vertex,edge)
  	for (int vertex=0 ; vertex < M->vertexnumber ; vertex++){
      if(vertices[M->vertexarray[vertex].type].id == rarest_vertex_id){
      	for (int edge=0 ; edge < M->vertexarray[vertex].degree ; edge++){
        	if(vertices[M->vertexarray[vertex].type].edges[edge] == rarest_edge_label)
            signature_vertex_edge(M,vertex,edge);
      	}
      }
  	}
}


// ######
// 5. Choose a signature filter
// ######

void set_rarest_pair(int* edge_min, int* edge_min_nb, int vertexnumber)
{
    int i,j,stack_level,completed,print_ready,max_edge;
    int *instruction_stack=calloc(vertexnumber,sizeof(int));
    int *direction_stack=calloc(vertexnumber,sizeof(int));
    int *vertex_distrib=calloc(vertexnumber,sizeof(int));
    unsigned long long int *total_vertex_distrib=calloc(vertexnumber,sizeof(unsigned long long int));
    int *edge_distrib=calloc(label.size,sizeof(int));
    vertex_distrib[0]=mapsize;
    stack_level=vertexnumber-1;
    completed=0;
    print_ready=1;

    //Initiate edge distribution vector (nullity of this vector is a necessary condition for an acceptable map)
    for(i=0;i<vertices[0].degree;i++){
        edge_distrib[vertices[0].edges[i]]+=vertex_distrib[0];
    }
    //Retrieve max label of edges
//    max_edge=0;
//    for (i = 0;i < vertexnumber;i++){
//        for (j = 0;j < vertices[i].degree;j++){
//            if(vertices[i].edges[j] > max_edge){max_edge = vertices[i].edges[j];}
//        }
//    }
    max_edge=(label.size)/2;

    //Create tranfer vectors (only need from previous vertex type, and from first vertex type)
    int **transfer_from_first=malloc(vertexnumber*sizeof(int*));
    int **transfer_from_previous=malloc(vertexnumber*sizeof(int*));
    for(i=1;i<vertexnumber;i++){
        transfer_from_first[i]=calloc(label.size,sizeof(int));
        transfer_from_previous[i]=calloc(label.size,sizeof(int));
        for (j = 0;j < vertices[i].degree;j++){
            transfer_from_first[i][vertices[i].edges[j]]++;
            transfer_from_previous[i][vertices[i].edges[j]]++;
        }
        for (j = 0;j < vertices[0].degree;j++){ //Substract first vector (edge distribution vector of first type of vertex)
            transfer_from_first[i][vertices[0].edges[j]]--;
        }
        for (j = 0;j < vertices[i-1].degree;j++){ //Substract previous vector (edge distribution vector of previous type of vertex)
            transfer_from_previous[i][vertices[i-1].edges[j]]--;
        }
    }

    // We explore all possible configurations of <mapsize> (N) vertices in <vertexnumber> (k) types
    // Some combinatory algebra gives (N+k-1)!/(N!*(k-1)!) combinaisons
    // Therefore, one can have access to a guess of the total vertices distribution for all maps. This is then used to guess the rarest (vertex, edge) pair of all maps.
    // The configurational space to explore is a portion of an hyperplane of dimension <mapsize>. (e.g. : a segment for N=2, a triangle for N=3, a tetrahedron for N=4, ...)
    // To minimize the space complexity, and have a fast exploration algorithm, 2*N transfer vectors (in dimension N) are precomputed.
    // The exploration is performed through a sequence of tranfers exploring the all space without repetition
    // Example for N=3: vertex types are A, B and C
    //   A
    //    |
    //    | |\   C
    //    | | |/
    //    |/
    //   B
    // Everything is on A, transfers from A to B until no A, transfer from A to C, then transfers from A to B until no B, transfer from A to C, ...
    // If we define phase X (== stack_level) as 'transfer from A to X' and -X as 'transfer from X to A', we have the algorithm:
    // X : (X-1)                                                    -X : -(X-1)
    //     X-1 -> X                                                      X -> A / X-1 (depending on parity for exhaustive exploration path)
    //     check if completed (i.e. (A,...,X-1) == (0,...,0))            check if completed (i.e. (B,...,X) == (0,...,0))
    //     -(X-1)                                                        (X-1)
    //     A -> X                                                        X -> X-1 / A (idem)
    //     check if completed (i.e. (A,...,X-1) == (0,...,0))            check if completed (i.e. (B,...,X) == (0,...,0))
    completed=1;
    while(stack_level<vertexnumber){
        while(completed == 0){
            if(direction_stack[stack_level] == 0){ // Phase X (X == stack_level)
                if(instruction_stack[stack_level] == 0){ // Call Phase (X-1)
//                    instruction_stack[stack_level]++;
//                    instruction_stack[stack_level] &= 0x3;
                    instruction_stack[stack_level]++;
                    if(stack_level > 1){
                        stack_level--;
                        instruction_stack[stack_level] = 0;
                        direction_stack[stack_level] = 0;
                        print_ready=0;
                        break;
                    }
                } else if(instruction_stack[stack_level] == 1) { // Transfer (X-1)->X
                    instruction_stack[stack_level]++;
                    vertex_distrib[stack_level-1]--;
                    vertex_distrib[stack_level]++;
                    for(i=0;i<label.size;i++){
                        edge_distrib[i]+=transfer_from_previous[stack_level][i];
                    }
                    print_ready=1;
                    break; // test if completed
                } else if(instruction_stack[stack_level] == 2) { // Call Phase -(X-1)
                    instruction_stack[stack_level]++;
                    if(stack_level > 1){
                        stack_level--;
                        instruction_stack[stack_level] = 2*(vertex_distrib[stack_level] & 0x1); // Need to tell to -(X-1) how to start a path reaching A at the end, depending on the parity of elements that must be given
                        direction_stack[stack_level] = 1;
                        print_ready=0;
                        break;
                    }
                } else if(instruction_stack[stack_level] == 3) { // Transfer A->X
                    instruction_stack[stack_level] = 0;
                    vertex_distrib[0]--;
                    vertex_distrib[stack_level]++;
                    for(i=0;i<label.size;i++){
                        edge_distrib[i]+=transfer_from_first[stack_level][i];
                    }
                    print_ready=1;
                    break; // test if completed
                }
            } else { // Phase -X
                if(instruction_stack[stack_level] == 0){ // Call Phase -(X-1)
                    instruction_stack[stack_level]++;
                    if(stack_level > 1){
                        stack_level--;
                        instruction_stack[stack_level] = 2*(vertex_distrib[stack_level] & 0x1); // Need to tell to -(X-1) how to start a path reaching A at the end, depending on the parity of elements that must be given
                        direction_stack[stack_level] = 1;
                        print_ready=0;
                        break;
                    }
                } else if(instruction_stack[stack_level] == 1) { // Transfer X->A
                    instruction_stack[stack_level]++;
                    vertex_distrib[0]++;
                    vertex_distrib[stack_level]--;
                    for(i=0;i<label.size;i++){
                        edge_distrib[i]-=transfer_from_first[stack_level][i];
                    }
                    print_ready=1;
                    break; // test if completed
                } else if(instruction_stack[stack_level] == 2) { // Call Phase (X-1)
                    instruction_stack[stack_level]++;
                    if(stack_level > 1){
                        stack_level--;
                        instruction_stack[stack_level] = 0;
                        direction_stack[stack_level] = 0;
                        print_ready=0;
                        break;
                    }
                } else if(instruction_stack[stack_level] == 3) { // Transfer X->(X-1)
                    instruction_stack[stack_level] = 0;
                    vertex_distrib[stack_level-1]++;
                    vertex_distrib[stack_level]--;
                    for(i=0;i<label.size;i++){
                        edge_distrib[i]-=transfer_from_previous[stack_level][i];
                    }
                    print_ready=1;
                    break; // test if completed
                }
            }
        }
        if(print_ready == 1){ // A transfer just occured
//            for(i=0;i<vertexnumber;i++){
//                printf("%d ",vertex_distrib[i]);
//            }
//            printf("\t%d\t%d\t",stack_level*(-2*direction_stack[stack_level]+1),max_edge);
//            for(i=0;i<NBMAXEDGETYPES;i++){
//                printf("%d ",edge_distrib[i]);
//            }
//            printf("\n");

            completed=1; // check if acceptable edge distribution
            for(i=0;i<max_edge;i++){
                if(edge_distrib[i] != edge_distrib[i+max_edge]){completed=0;break;}
            }
            if(completed == 1){ // If acceptable edge distribution, update total_vertex_distrib
                for(i=0;i<vertexnumber;i++){
                    total_vertex_distrib[i] += (unsigned long long int)vertex_distrib[i];
                }
            }
        }
        completed=1;
        if(direction_stack[stack_level] == 0){ // We are in phase X, check if (A, ..., X-1) = (0, ..., 0)
            for(i=0;i<stack_level;i++){
                if(vertex_distrib[i] != 0){
                    completed=0;
                    break;
                }
            }
        } else { // We are in phase -X, check if (B, ..., X) = (0, ..., 0)
            for(i=1;i<=stack_level;i++){
                if(vertex_distrib[i] != 0){
                    completed=0;
                    break;
                }
            }
        }
        if(completed > 0){ // if phase X completed, come back to phase (-)X+1
            stack_level++;
            print_ready=0;
        }
    }

    // Choose the minimum (vertex id, edge id) pair appearing in all acceptable configurations explored, weighted by minimum edge multiplicity
    unsigned long long int min_occurence,occurences = 0;
    rarest_vertex_id=0;
    min_occurence = 0;
    for (i = 0;i < vertexnumber;i++){
        occurences = total_vertex_distrib[i]*edge_min_nb[i];
//        printf("o%llu, v%i, m%i\n",occurences,i,edge_min_nb[i]);
        if ((occurences != 0 && occurences < min_occurence) || min_occurence == 0){
            min_occurence = occurences;
            rarest_vertex_id = i;
        }
    }
    rarest_edge_label = edge_min[rarest_vertex_id];

    free(instruction_stack);
    free(direction_stack);
    free(vertex_distrib);
    free(edge_distrib);
    free(total_vertex_distrib);
    for (i = 1;i < vertexnumber;i++){
        free(transfer_from_first[i]);
        free(transfer_from_previous[i]);
    }
    free(transfer_from_first);
    free(transfer_from_previous);
}
