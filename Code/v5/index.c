#include <string.h>
#include "structure.h"
#include "signature.h"
#include "output.h"
#include "index.h"
#include "concat.h"
#include "chrono.h"

// 1.0 Function that calls all the indices computation and do the recursive job

void compute_all_nodes_indices(struct AvlNode *avlnode, unsigned long int nb_to_compute, double start_time) {
	static int nb_computed = 0;
	if (!avlnode) return;
	compute_all_nodes_indices (avlnode->l, nb_to_compute, start_time);
	// Already done for signature
	if(!SELECTIONCUTINDEX){ avlnode->M->cutindex = sparsity_poly(avlnode->M);}
	avlnode->M->vertexautomorphic = automorphism(avlnode->M);
	avlnode->M->faces = faces(avlnode->M);
	nb_computed++;

	static double prev_time = 0.0;
	static unsigned int lemodulo=0xff;
	double now_time = what_time_is_it();
	if (!(nb_computed & lemodulo)) {
		now_time = what_time_is_it();
		if (now_time-prev_time > 1.0) lemodulo = (lemodulo>>1);
		if (now_time-prev_time < 0.5) lemodulo = (lemodulo<<1)+1;
		prev_time = now_time;
		printf("\r\033[2K");
		printf("   %d/%ld (%.2f%%) in %.1lf sec  (%.2lf per second)\r",nb_computed,nb_to_compute,100.0*(float)nb_computed/nb_to_compute,now_time-start_time,nb_computed/(now_time-start_time));
		fflush(stdout); 
	}
	compute_all_nodes_indices (avlnode->r, nb_to_compute, start_time);
}

// 1.1 General function to be called in the main()

void compute_all_indices(Avl avl) {
	double start_time = what_time_is_it();
	compute_all_nodes_indices(avl.root, avl.nbnodes, start_time);
}

// 2.1. Index 1 : cut index
#define min(a,b) ((a)<(b)?(a):(b))



/* compute the sparsity throug the enumeration of all cuts, can be used for comparison purposes*/
/*
float sparsity_exp(Map *M) {
	int i;
	int *vectorpos = malloc(sizeof(int)*M->vertexnumber);//representation of the current set as a list
	int *set = malloc(M->vertexnumber*sizeof(int));//representation of the current set as a -1 1 array
	for (i=1;i<M->vertexnumber;i++) { set[i] =-1; }
	int pos=0;//position of the largest element
	vectorpos[0]=0;//begin with element 0 alone in the set
	vectorpos[M->vertexnumber-1]=0; //the last elements is initialized, has value 0 during all the algorithm
	set[0]=1;//start with the first element in the set
	int cut = 0;//current cutsize (count the loop ?)
	float mincut = M->vertexarray[0].degree;
	float floatcut;
	int maxsize = M->vertexnumber -2;
	int modified=0;//index of the element which has been changed in the partition

	void updatemincut(){
		for(i=0;i<M->vertexarray[modified].degree;i++) { //update the new cutvalue by flipping some edges
			if(M->vertexarray[modified].edges[i].vertexindex != modified) cut -=  set[modified]*set[M->vertexarray[modified].edges[i].vertexindex];
		}
		floatcut = cut/(float) min(pos+1, M->vertexnumber-pos-1);//compute the normalized cut for this partition
		if(floatcut < mincut){mincut = floatcut;}
	}

	while(1){ //Gray code enumeration
		// Parity bit even 
		updatemincut();
		modified = maxsize;
		if (vectorpos[pos] == maxsize) {
			pos--;
			set[modified]=-1;
		}
		else { 
			pos ++;
			vectorpos[pos] = maxsize;
			set[modified]=1;
		}
		if (pos==-1) { break; }
		// Parity bit odd
			updatemincut();
		modified = vectorpos[pos]-1; 
		if (vectorpos[pos-1] == modified) {//i-1 is in the set
			pos --;
			vectorpos[pos]++;
			set[modified]=-1;
		}
		else { //i-1 is not in the set 
			vectorpos[pos+1] = vectorpos[pos];
			vectorpos[pos]--;
			pos ++;
			set[modified]=1;
		}
	}

	free(set);
	free(vectorpos);
	if (mincut<0) printf("\nmincut = %f\n",mincut);
	return mincut;
}*/

// 2.2 Isomorphisme computation
int egal_signature(int edgenumber, unsigned int *S1, unsigned int *S2) {
	int i;
	for (i=0 ; i<edgenumber ; i++) {
		if (S1[i] != S2[i]) return 0;
	}
	return 1;
}


int *automorphism (Map *M) {
	int *vertexautomorphic = malloc(M->vertexnumber*sizeof(int)); if (!vertexautomorphic) { exit(78);}
	int v, vv, e;
	unsigned int les_signatures[M->vertexnumber][M->edgenumber]; // Stockage de toutes les signatures
	unsigned int *the_signature = M->signature; // Save the minimal signature
	for (v=0 ; v < M->vertexnumber ; v++) { // Loop over the vertices 
		// 1. Computes the minimal signature starting from v
		M->signature = les_signatures[v]; // On fait pointer M->signature sur la partie du tableau qui va bien
		M->signature[0] = 0x7fffffff;
		for (e=0 ; e < M->vertexarray[v].degree ; e++) signature_vertex_edge(M, v, e); //Loop over the edges
		vv = 0;
		int type_v  = M->vertexarray[v].type;
		int type_vv = M->vertexarray[vv].type;
		while ( (vv<v) && ((!egal_signature(M->edgenumber,les_signatures[vv],les_signatures[v])) || (vertices[type_v].id!=vertices[type_vv].id))) {
			vv++; 
			type_vv = M->vertexarray[vv].type;
		}
		vertexautomorphic[v] = vv;
	}
	M->signature = the_signature; // Restore the minimal signature
	return vertexautomorphic;
}

// 3.0 Sizes of faces

struct Faces faces (Map *M) {
	struct Faces F;
	int distrib[6*M->vertexnumber]; // Temporary variable to store the faces sizes will be strored in F at the end 
	int i; for (i=0; i<=6*M->vertexnumber; i++) distrib[i]=0;
	F.max = 0;
	int c, e, eprime, prevc, preve, nextc, nexte;
	int nbedge;
	int dejavu[M->vertexnumber][16]; // Useful to goe through a face only once
	for (c=0; c<M->vertexnumber; c++) 
		for (e=0; e<M->vertexarray[c].degree; e++) 
			dejavu[c][e]=0;
	// For each vertex c and each edge (c,u) we compute the face size by turning left.
	// Each face will be counted as many times as its size, need normalization at the end
	for (c=0; c<M->vertexnumber; c++) {
		for (e=0; e<M->vertexarray[c].degree; e++) {
			if (!dejavu[c][e]) { // if the edge is not dejavu in a face
				prevc=c; preve=e;
				nbedge=0;
				do {
					// Compute the next couple (c,e) according the order
					nextc = M->vertexarray[prevc].edges[preve].vertexindex;
					eprime = M->vertexarray[prevc].edges[preve].edgeindex;
					nexte = (eprime+1) % M->vertexarray[nextc].degree;
					nbedge++;
					prevc=nextc; preve=nexte;
					dejavu[nextc][nexte] = 1;
				} while ((c!=nextc) || (e!=nexte)); // Until come back
				distrib[nbedge]++; // Add the new size
				if (nbedge>F.max) F.max=nbedge; // Update the max if needed
			}
		}
	}
	// Complete the data in the structure before returning F;
	F.distrib = malloc((F.max+1)*sizeof(unsigned long long int)); // Allocation of the minimal size needed
	if (!F.distrib) exit (71);
	F.distrib[0]=0;
	int second = 0; F.mean = 0; F.nb = 0; 
	for (i=1; i<=F.max; i++) { // Recopie of the distrib with normalization and computation of F.nb F.mean and second
		F.distrib[i] = distrib[i];
		if (F.distrib[i] && i<F.max) second = i; 
		F.mean += i*F.distrib[i];
		F.nb += F.distrib[i];
		}
	if (F.distrib[F.max]>1) second = F.max;
	F.mean = F.mean/F.nb;
	F.index = (F.max-second)/F.mean;
	return F;
}

// 4.0 Print of indices

void print_all_nodes_indices(FILE *F, struct AvlNode *avlnode) {
  if (!avlnode) return;
  print_all_nodes_indices (F, avlnode->l);
  // Cut index
  fprintf(F,"%f",avlnode->M->cutindex);
  fprintf(F," | ");
  // Automorphism
  int i; 
  for (i=0 ; i<avlnode->M->vertexnumber ; i++) {
    int the_type = avlnode->M->vertexarray[i].type;
    char letter = 'A' - 1 + vertices[the_type].letter;
    int id = vertices[the_type].id;
    fprintf(F,"  [%c%d]%2d",letter,id,avlnode->M->vertexautomorphic[i]);
  }
  //WriteIsoClass(F, avlnode->M,vertices);

  // distrib face sizes
  fprintf(F," | %d [",avlnode->M->faces.max);
  for (i=0; i<=avlnode->M->faces.max; i++) fprintf(F," %llu",avlnode->M->faces.distrib[i]);
  fprintf(F,"] %.3f",avlnode->M->faces.index);
  fprintf(F,"\n");
  print_all_nodes_indices (F, avlnode->r);
}

void print_all_indices(char *LongDirName,Avl avl) {
  char FileName[512];
  FILE *F;

  sprintf(FileName,"%s/all_indices.txt",LongDirName);
  F = fopen(FileName,"w");
  print_all_nodes_indices(F, avl.root);
  fclose(F);
}





// 5.0 Utilities for computing the cut index

int compute_faces(Map *M){//index each face by an integer and store in faces which edge belongs to which face
  int facesnumber=0;
  int currentvertex,currentedge,i,j,position,temp;
  for(i=0;i<M->vertexnumber;i++){
    for(j=0;j<M->vertexarray[i].degree;j++){
      edge[i][j].first = -1;
    }
  }
  
  for(i=0;i<M->vertexnumber;i++){
    for(j=0;j<M->vertexarray[i].degree;j++){
      if(edge[i][j].first==-1){
	face[facesnumber].first = i;
	face[facesnumber].second = j;
	currentvertex=i;
	currentedge =j;
	position = 0;
	do//go through all edges in the face to which belongs M.vertexarray[i].edges[j]
	  {
	    edge[currentvertex][currentedge].first = facesnumber;
	    edge[currentvertex][currentedge].second = position; 
	    temp = M->vertexarray[currentvertex].edges[currentedge].edgeindex;
	    currentvertex = M->vertexarray[currentvertex].edges[currentedge].vertexindex;
	    currentedge = temp +1;
	    if(currentedge == M->vertexarray[currentvertex].degree) currentedge = 0;
	    position ++;
	  } while(i != currentvertex || j!= currentedge); //label all edges as belonging to the same face
	facesnumber++;
      }
    }
  }
  return facesnumber;
}

/*
void printedge(pair **edge, Map M){
  int i,j;
for(i=0;i<M.vertexnumber;i++){
  printf("vertex %d",i);
  for(j=0;j<M.vertexarray[i].degree;j++){
    printf(" (%d %d)",edge[i][j].first,edge[i][j].second);
    }
    printf("\n");
  }
}

void printface(pair *face,int size){
  int i;
  for(i=0;i<size;i++)
    {
      printf("%d eme face : arete %d %d\n", i, face[i].first,face[i].second);
    }
}
*/

void compute_dual(Map *M,pair **edge, pair *face, DualMap *dual){// create the dualmap of M, using the labeling of each face
  int i,degree;
  int currentvertex,currentedge,temp;
  pair otheredge;
  dual->vertexnumber = compute_faces(M);
  //printedge(edge,M);
  //printface(face,dual->vertexnumber);
  for(i=0; i < dual->vertexnumber;i++){
    currentvertex = face[i].first;
    currentedge = face[i].second;
    degree = 0;
    //printf("face %d\n",i);
    do//go through all edges in the face to which belongs M.vertexarray[i].edges[j]
      {
	otheredge = edge[M->vertexarray[currentvertex].edges[currentedge].vertexindex][M->vertexarray[currentvertex].edges[currentedge].edgeindex];//compute the face on the other side of the current edge
	dual->vertexarray[i].edges[degree].vertexindex = otheredge.first;
	dual->vertexarray[i].edges[degree].edgeindex = otheredge.second;
	dual->vertexarray[i].edges[degree].region = -1;
	dual->vertexarray[i].edges[degree].weigth = -1;
	degree++;// an edge has been added in the dual
	temp = M->vertexarray[currentvertex].edges[currentedge].edgeindex; //go to the next edge in the face
	currentvertex = M->vertexarray[currentvertex].edges[currentedge].vertexindex;
	currentedge= (temp+1) % M->vertexarray[currentvertex].degree;
      } while(face[i].first != currentvertex || face[i].second != currentedge); //label all edges as belonging to the same face
    dual->vertexarray[i].degree = degree;//update the degree
  }
}

void printdual(DualMap *dual){
  int i,j;
  printf("Maps with %d vertices \n",dual->vertexnumber);
  for(i=0; i < dual->vertexnumber; i++)
    {
      printf(" Degree %d Neighbour :",dual->vertexarray[i].degree);
      for(j=0;j < dual->vertexarray[i].degree; j++)
	{
	  printf("(%d,%d,%d,%d) ",dual->vertexarray[i].edges[j].vertexindex, dual->vertexarray[i].edges[j].edgeindex,dual->vertexarray[i].edges[j].region,dual->vertexarray[i].edges[j].weigth);
	}
      printf("\n");
    }
}


void spanningtree(DualMap *dual,pair *ancester){// compute a spanning tree from the outerface and compute the regions around the vertices
  int currentedge = 0;
  int currentvertex = 0, nextvertex, i, region = 1;
  for(i=0;i<dual->vertexnumber;i++) ancester[i].first=-1; //ancester may be simplified
  ancester[0].first = 0;
  ancester[0].second = -1;//default value
  do{
    nextvertex = dual->vertexarray[currentvertex].edges[currentedge].vertexindex;
    if(currentedge != ancester[currentvertex].second)
      {
	if(ancester[nextvertex].first == -1)
	  {
	    ancester[nextvertex].first = currentvertex;
	    ancester[nextvertex].second = dual->vertexarray[currentvertex].edges[currentedge].edgeindex;
	    dual->vertexarray[currentvertex].edges[currentedge].region = region++;
	    dual->vertexarray[currentvertex].edges[currentedge].weigth = 0;//the weigth of a tree edge is 0
	    currentedge = (ancester[nextvertex].second + 1) % dual->vertexarray[nextvertex].degree;
	    currentvertex = nextvertex;
	  }
	else
	  {
	    currentedge = (currentedge + 1) %  dual->vertexarray[currentvertex].degree; //go to the next edge
	  }
      }
    else{
      dual->vertexarray[currentvertex].edges[currentedge].region = region++;
      dual->vertexarray[currentvertex].edges[currentedge].weigth = 0; //the weigth of a tree edge is 0
      currentedge = (dual->vertexarray[currentvertex].edges[currentedge].edgeindex +1) % dual->vertexarray[nextvertex].degree;
      currentvertex = nextvertex;
    }
  }while(currentvertex != 0 || currentedge != 0);
}


void computeregion(DualMap *dual){//update the field region so that every edge has a value, write -1 in all non tree edge and 0 in the others
  int i,j,temp,beginning;
  for(i=0;i<dual->vertexnumber;i++){
    beginning=dual->vertexarray[i].degree-1;
    while(dual->vertexarray[i].edges[beginning].region == -1){ beginning--;}//find the first edge with an initialized region field
    j = beginning -1;
    if(j<0) j+= dual->vertexarray[i].degree;
    temp = dual->vertexarray[i].edges[beginning].region;
    while(j != beginning){//between two edges with an initialized region fields, the region is labeled by the region of the first edge
      if(dual->vertexarray[i].edges[j].region == -1){dual->vertexarray[i].edges[j].region = temp;}
      else{temp = dual->vertexarray[i].edges[j].region;}
      j--;
      if(j<0) j+= dual->vertexarray[i].degree;
    }
  }
}

int compute_ancesters(int v, pair *ancester, pair *tree)
  {
    int size = 0;
    int currentvertex = v;
    while(currentvertex != 0)//0 is the root of the tree 
      {
	ancester[size].first = currentvertex;
	ancester[size].second = tree[currentvertex].second;
	size ++;
	currentvertex = tree[currentvertex].first;//go up in the tree
      }
    ancester[size].first = 0; //copy the root
    return size;
  }


int fundamentalcycle(int vertex1, int edge, pair* tree, pair *cycle, DualMap *dual){ // compute the fundamental cycle given by the vertex and edge and the tree in ancester
  int size1 =0,size2=0,i=1,cyclesize;
  int vertex2 = dual->vertexarray[vertex1].edges[edge].vertexindex;
  pair *ancestersequence1 = malloc(sizeof(pair)*dual->vertexnumber);//we could store only the edges to optimize
  pair *ancestersequence2 = malloc(sizeof(pair)*dual->vertexnumber);
 
  size1 = compute_ancesters(vertex1, ancestersequence1,tree);
  //printf("size : %d \n",size1); for(i=0;i<=size1;i++){printf(" %d ",ancestersequence1[i].first);} printf(" \n");
  size2 = compute_ancesters(vertex2, ancestersequence2,tree);
  //printf("size : %d \n",size2); for(i=0;i<=size2;i++){printf(" %d ",ancestersequence2[i].first);}printf(" \n");
  while(size1 >= i && size2 >= i && ancestersequence1[size1-i].first == ancestersequence2[size2-i].first) i++;
  i--;  //i is the index of the common ancester of vertex1 and vertex 2
  //printf("i %d",i);
  for(cyclesize = 0; cyclesize <= size2 - i;cyclesize++) cycle[cyclesize] = ancestersequence2[cyclesize];//copy the edges obtained by going up the tree from vertex2 
  for(i = size1-i-1; i >=0; i--)
    {//copy the edges obtained by going up the tree from vertex 1 in reverse order
      cycle[cyclesize].first  = ancestersequence1[i].first;
      cycle[cyclesize-1].second = dual->vertexarray[ancestersequence1[i].first].edges[ancestersequence1[i].second].edgeindex;
      cyclesize++;
    }
  cycle[cyclesize-1].second = edge;//add the edge at the end
  //for(i=0;i<cyclesize;i++){printf(" (%d,%d)",cycle[i].first,cycle[i].second);}
  //printf("\n");
  free(ancestersequence1);free(ancestersequence2);
  return cyclesize;
}

int cycle_enclosing(pair *cycle, DualMap *dual,int cyclesize,int **edges){ //return the number of faces inside the cycle
  //cycle is used as the heap of edges to use to compute their associated face 
  //edge is used to store the edges already used in a face
  int facesnumber = 0;
  int i,j,vertex,edge,currentedge,currentvertex,endvertex,endedge;
  for(i=0;i<dual->vertexnumber;i++){
    for(j=0;j < dual->vertexarray[i].degree; j++){
      edges[i][j] = 0;
    }
  }
  for(i=0;i<cyclesize;i++)
    {
      vertex = dual->vertexarray[cycle[i].first].edges[cycle[i].second].vertexindex;
      edge = dual->vertexarray[cycle[i].first].edges[cycle[i].second].edgeindex;
      edges[vertex][edge] = 1;
    }
  while(cyclesize > 0){
    cyclesize--;
    //printf("%d ",cyclesize);
    if(!edges[cycle[cyclesize].first][cycle[cyclesize].second]){
      endvertex = cycle[cyclesize].first;
      currentvertex = cycle[cyclesize].first;
      currentedge = cycle[cyclesize].second;
      endedge =  cycle[cyclesize].second;
      do
	{
	  edges[currentvertex][currentedge] = 1;
	  vertex = dual->vertexarray[currentvertex].edges[currentedge].vertexindex;
	  edge = dual->vertexarray[currentvertex].edges[currentedge].edgeindex;
	  cycle[cyclesize].first = vertex;
	  cycle[cyclesize].second = edge;
	  cyclesize++;
	  //printf("%d (%d,%d) ",cyclesize,vertex,edge);
	  currentedge = (edge + 1) % dual->vertexarray[vertex].degree;
	  currentvertex = vertex;
	}while(currentvertex != endvertex || currentedge != endedge );
      facesnumber++;
    }
  }
  // printf("facenumber %d\n",facesnumber);
  return facesnumber;
}

void weigth_dual(DualMap *dual, pair *tree, int **edges){//intialize the weigth of each edge in dual, according to the number of faces in its fundamental cycle and the orientation
  int i,j, vertex,edge,cyclesize;
  pair *cycle = malloc(sizeof(pair)*dual->vertexnumber*12);//max number of directed edges in the planar graph (no because multigraph)
  for(i=0; i < dual->vertexnumber; i++){//for all vertices i of the dual graph
    for(j=0; j<dual->vertexarray[i].degree; j++){//for all edges of the vertex i
      if(dual->vertexarray[i].edges[j].weigth !=0){//if it is a non tree edge, compute its weigth
	vertex = dual->vertexarray[i].edges[j].vertexindex;
	edge = dual->vertexarray[i].edges[j].edgeindex;
	if(dual->vertexarray[i].edges[j].region > dual->vertexarray[vertex].edges[edge].region){
	  cyclesize = fundamentalcycle(i, j, tree, cycle,dual);
	  dual->vertexarray[i].edges[j].weigth = cycle_enclosing(cycle,dual,cyclesize,edges);
	  dual->vertexarray[vertex].edges[edge].weigth = -dual->vertexarray[i].edges[j].weigth;//the other orientation of the edge is assigned the opposite weigth
	}
      } 
    }
  }
  free(cycle);
}

//the function should be cleaned 
//possible improvement: go over every vertex instead of every edge
//64 should be replaced by some constant or something related to mapsize

float sparsity(DualMap *dual,int primalsize){ //compute the minimum sparsity of the map M using the dual map for each starting point all shortest paths are computed and we keep the minimum sparsity we obtain
  int i,j,k,l,secondvertex,secondedge,w,nextw;
  int currentelement, lastelement, nextelement, nextvertex, currentvertex, currentweigth;
  int weigthmax= 2*primalsize-1;
  int distancemax= 1000; //bound on the distance given by the current sparsity
  float min = 10000,temp,partitionsize, minsize;
  for(i=0;i<dual->vertexnumber;i++){
    for(j=0; j < dual->vertexarray[i].degree; j++){
      secondvertex = dual->vertexarray[i].edges[j].vertexindex;//(i,j) is the edge used in the cycle
      secondedge = dual->vertexarray[i].edges[j].edgeindex;//(secondvertex,secondedge) is the opposite edge
      w =  dual->vertexarray[i].edges[j].weigth;
      if(i == secondvertex){//special case for loops of the dual graph
	partitionsize = abs(w);
	temp = 1 / ((partitionsize > primalsize -partitionsize) ? primalsize -partitionsize : partitionsize);//division by 0 not possible
	if(temp < min) {//printf("update min isthme,  temp %f partitionsize %f \n",temp,partitionsize);
	  min = temp;
	  distancemax =  (min*primalsize)/2;
	}
      }
      else{//initialization of the queue
	for(k=0; k < dual->vertexnumber;k++){//-1 means the vertex has not yet been explored by the BFS
	  for(l=0; l < weigthmax;l++){
	    queue[64*k + l].second = -1;
	  }
	}
	currentelement = secondvertex*64 + primalsize-1+w;
	queue[currentelement].first = currentelement;
	queue[currentelement].second = 1;
	lastelement = currentelement;
	do{//BFS
	  currentvertex = currentelement/64;
	  currentweigth = currentelement % 64;
	  if(currentvertex == i)//we have computed the distance from second vertex to i
	    {//update the min
	      partitionsize = abs(currentweigth - primalsize+1);
	      minsize = ((partitionsize > (primalsize - partitionsize)) ? (primalsize - partitionsize) : partitionsize);
	      temp = queue[currentelement].second / minsize;
	      if(temp < min) {min = temp; distancemax = (min*primalsize)/2;}//printf("update min %d %f \n",distance,minsize);}
	    }
	  for(k=0;k<dual->vertexarray[currentvertex].degree;k++){
	    if(currentvertex == secondvertex && k == secondedge){continue;}//the backedge cannot be used in the shortest path
	    nextw = dual->vertexarray[currentvertex].edges[k].weigth + currentweigth;
	    nextvertex = dual->vertexarray[currentvertex].edges[k].vertexindex;
	    nextelement = 64*nextvertex + nextw;
	    if(nextw >=0 && nextw < weigthmax && queue[nextelement].second == -1) 
	      {//add the element to the queue if not already seen 
		queue[lastelement].first = nextelement;
		lastelement = nextelement;
		queue[lastelement].second = 1 + queue[currentelement].second;//compute the distance of the element we have just added
		//printf("add element %d %d \n",nextvertex,nextw);
	      }
	  }  
	  currentelement = queue[currentelement].first;//go to the next element in the queue
	  //printf("fin de boucle premier élément %d %d, dernier élément %d %d \n",currentelement.first,currentelement.second,lastelement.first,lastelement.second);
	}while(currentelement != lastelement &&  queue[currentelement].second <= distancemax);
      }
    }
  }
  return min;
}


float sparsity_poly(Map *M){
  //printmap(M);
  compute_dual(M,edge,face,dual);
  spanningtree(dual,tree);
  computeregion(dual);
  weigth_dual(dual,tree,edgebis);
  return sparsity(dual,M->vertexnumber);
}

