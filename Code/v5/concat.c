#include "structure.h"
#include "input.h"
#include "fold.h"
#include "hash.h"
#include "chrono.h"
#define CURRENTLABEL vertices[map->vertexarray[currentsize].type].edges[currentedge]


int smallest_representation(Map *map)
{//the id should be in the map instead of the type: then simplify the condition
	int i;
	for(i=0; i < map->vertexnumber/2 &&  vertices[map->vertexarray[i].type].id == vertices[map->vertexarray[map->vertexnumber-i-1].type].id; i++);
  	return (vertices[map->vertexarray[i].type].id <= vertices[map->vertexarray[map->vertexnumber-i-1].type].id);
}

Map *initializemap(){//create a new map with value -1 on the edges
	Map *M= malloc(sizeof(Map));
	M->vertexnumber = mapsize;
	M->edgenumber = 0;
	M->vertexarray = malloc(sizeof(struct VertexMap)*mapsize);
	if (!M->vertexarray) { fprintf(stderr,"cannot malloc map->vertexarray in concat.c\n"); exit(42); }
	for (int i=0 ; i<mapsize ; i++) {
		M->vertexarray[i].edges = malloc(sizeof(struct Edge)*maxdegree);
		if (!M->vertexarray[i].edges) { fprintf(stderr,"cannot malloc map->vertexarray[%d].edges in concat.c\n",i); exit(43); }
		for(int j=0;j<maxdegree;j++) {
			M->vertexarray[i].edges[j].vertexindex = -1;
			M->vertexarray[i].edges[j].edgeindex = -1;
		}
	}
	M->signature = malloc(mapsize * maxdegree * sizeof(unsigned int));
	return M;
}//we should add more thing (outline ...) to the map and do the intitialization/free in a dedicated file

//free the map
void free_map(Map *M){
	for(int i = 0; i< mapsize; i++){
		free(M->vertexarray[i].edges);
	}
	free(M->signature);
	free(M->vertexarray);
	free(M);
}


//all print functions should be put in their own file
void prettyprint2(FILE *F, unsigned long long int x, char *s) {
	int t,g,m,k,u;
	t = x/1e12; x = x-t*1e12;
	g = x/1e9;  x = x-g*1e9;
	m = x/1e6;  x = x-m*1e6;
	k = x/1e3;  u = x-k*1e3;
	if (t) fprintf(F,"%d,%03d,%03d,%03d,%03d",t,g,m,k,u);
	else if (g) fprintf(F,"%d,%03d,%03d,%03d",g,m,k,u);
	else if (m) fprintf(F,"%d,%03d,%03d",m,k,u);
	else if (k) fprintf(F,"%d,%03d",k,u);
	else        fprintf(F,"%3d",u);
	fprintf(F," %s",s);
}

void affichage_dynamique(unsigned long long int numberofbackbone, Hasht *h) {
	static unsigned int lemodulo=0xff; 
	static double start_time;
	static double last_time;
	double current_time = what_time_is_it();
	if (!numberofbackbone) { start_time = last_time = current_time; }
	if (!(numberofbackbone & lemodulo)) {
		if (current_time-last_time > 1.0) lemodulo = (lemodulo>>1);
		if (current_time-last_time < 0.5) lemodulo = (lemodulo<<1)+1;
		last_time = current_time;
		printf("\r\033[2K");
		printf("   %.1lf sec   ",current_time-start_time);
		prettyprint2(stdout,numberofbackbone,"quasi-uniq backbones   ");
		prettyprint2(stdout,h->insertions,"folded maps   ");
		prettyprint2(stdout,h->elements,"uniq maps");
		fflush(stdout); 
	}
}

//Main function which create every possible path and then for each call folding_enumeration to create all possible folding
unsigned long long int generate_paths(int verticesnumber, Hasht *h, Map *map, FreeEdge *outline){
	static unsigned long long int backbonenumber = 0;
  	int i,j, currentsize, currentedge, labelvalues;
  	int *connectionstack = malloc(mapsize*sizeof(int));
  	if (!connectionstack) { fprintf(stderr,"cannot malloc connectionstack in concat.c\n"); exit(41); }
  
    //The concatenation begin here
    int bound = (BACKBONETYPE==2) ? 1 : verticesnumber; //optimization for cycle begin by one type of vertex only: we assume that all motifs will be used in the maps 
    for(i=0; i<bound; i++)
      {//optimization : go over the first vertices only (not the rotated ones)
		for(j=0; j < vertices[i].degree; j++)
	  	{ 
	    	if (non_isomorph(vertices[i].edges,vertices[i].degree,j))//optimization: do not consider two edges which are equivalent in their neighborood for the first vertex
	    	{
				map->vertexarray[0].type = i;   //initialize the first vertex of the path
				map->vertexarray[0].degree = vertices[i].degree;
				map->edgenumber = vertices[i].degree - mapsize + 2 + 1;//-mapsize+2 corresponds to the number of edges not used in the signature
				currentsize=0; //size -1 of the current path
				connectionstack[0] = 0;
				labelvalues = label.shift - vertices[i].labelvalue;//the complementay of the current labelvalue
				currentedge = j; //the edge we want to connect the next vertex to
				while(currentsize >=0)
		  		{
		    		while(currentedge < map->vertexarray[currentsize].degree && (connectionstack[currentsize] == connection[CURRENTLABEL].vertexnumber || ( !almostfoldablepath[currentsize+1][CURRENTLABEL][labelvalues]) ))//move to the next free edge in the current vertex if the previous has been used or cannot be used to generate an almost foldable path
		      		//should add a condition with the size in the almostfoldable part
		      		{
						connectionstack[currentsize]=0;//reinitialize the possible connections
						currentedge++;//use the next edge
						if(currentsize == 0){currentedge =  map->vertexarray[currentsize].degree;}//when dealing with the first vertex skip to the last edge so that the program exit the while loop
		      		} 
		    		if(currentedge < map->vertexarray[currentsize].degree)//test whether there is an edge left to add some motif
		      		{
						//create the new vertex in the map and the corresponding edge 
						map->vertexarray[currentsize].edges[currentedge].vertexindex = currentsize+1;//create the new edge in the current vertex,  
						map->vertexarray[currentsize].edges[currentedge].edgeindex=0;  
						map->vertexarray[currentsize+1].type = connection[CURRENTLABEL].list[connectionstack[currentsize]];
						currentsize++;//the map has now one more vertex
						labelvalues -=  vertices[map->vertexarray[currentsize].type].labelvalue;//add the value of the labels of the new vertex
						map->vertexarray[currentsize].degree = vertices[map->vertexarray[currentsize].type].degree;
						map->edgenumber += map->vertexarray[currentsize].degree;
						map->vertexarray[currentsize].edges[0].vertexindex = currentsize -1;//could be put in the map once and for all
						map->vertexarray[currentsize].edges[0].edgeindex = currentedge;//create the edge in the new vertex
						currentedge = 1; //the edge 0 is connected to the previous vertex therefore we consider the next one
						connectionstack[currentsize]=0;
		      		}
		    		if(currentsize == mapsize-1 &&  (labelvalues==label.shift) && smallest_representation(map) ) //produce a complete backbone when the path is of the right size and the path is almost foldable
		      		{
						affichage_dynamique(backbonenumber, h);
						backbonenumber++;//should increment this value before testing smallest representation and almost foldable
		        		folding_enumeration(map, h, outline);
		    		}
		    		if(currentsize == mapsize-1 || currentedge == map->vertexarray[currentsize].degree)//remove the current vertex if the path is finished or if we have connected all possible vertices to the current vertex
		      		{
						labelvalues +=  vertices[map->vertexarray[currentsize].type].labelvalue;//subtract the labelvalue of the deleted vertex
						map->edgenumber -= map->vertexarray[currentsize].degree;
						currentedge = map->vertexarray[currentsize].edges[0].edgeindex;
						currentsize--;
						if(currentsize >= 0)
			  			{
			    			map->vertexarray[currentsize].edges[currentedge].vertexindex=-1;//remove the edge to the previous current vertex 
			    			connectionstack[currentsize]++; //use the next possible vertex in the future connection
			  			}
		    		}
				}
			}
  		}
  	}
  free(connectionstack);
  return backbonenumber;
}



//Function which create every possible tree and then for each call folding_enumeration to create all possible folding

unsigned long long int generate_trees(int verticesnumber, Hasht *h, Map *map, FreeEdge *outline)
{
  int size=1, free_edge_number=0,i,j,currentvertex,currentedge,currentlabel,newtype;
  static unsigned long long int backbonenumber = 0;
  pair *edgestack = malloc(maxdegree*mapsize*sizeof(pair));//first field vertex, second field edge
  pair **enumeration_position = malloc(mapsize*sizeof(pair*));//what is the next connection and in which context does it holds 
  for(i=0;i<mapsize;i++) enumeration_position[i] = malloc(maxdegree*sizeof(pair));
  
  map->vertexarray[0].type = 0;   //initialize the first vertex of the tree as vertex of type 0
  map->vertexarray[0].degree = vertices[0].degree;
  map->edgenumber = vertices[0].degree - mapsize + 2 + 1;//-mapsize+2 corresponds to the number of edges not used in the signature
  int labelvalues = vertices[0].labelvalue;
  for(i=0;i<map->vertexarray[0].degree;i++) //use free_edge_number instead of i ?
    {
      edgestack[free_edge_number].first = 0;
      edgestack[free_edge_number].second = i;
      enumeration_position[0][i].first=0;
      free_edge_number++;
    }//begin by the first type of vertex only 
  	while(1){
    //printf("size %d free_edge_number %d\n",size,free_edge_number);
    //for(i=0;i<free_edge_number;i++) printf("(%d,%d)  ", edgestack[i].first, edgestack[i].second);
    //printf("\n");
    if(size < mapsize && free_edge_number > 0 &&  almostfoldabletree[size][label.shift - labelvalues])
      {//we add a motif to the first free edge in the tree
		free_edge_number--;//the current edge will not be free anymore
		currentvertex = edgestack[free_edge_number].first;
		currentedge = edgestack[free_edge_number].second;
		map->vertexarray[currentvertex].edges[currentedge].vertexindex = size;
		map->vertexarray[currentvertex].edges[currentedge].edgeindex=0;
		map->vertexarray[size].edges[0].vertexindex= currentvertex;
		map->vertexarray[size].edges[0].edgeindex= currentedge;
		currentlabel = vertices[map->vertexarray[currentvertex].type].edges[currentedge];
		newtype = connection[currentlabel].list[enumeration_position[currentvertex][currentedge].first];
		map->vertexarray[size].type = newtype;
		labelvalues += vertices[newtype].labelvalue;
		map->vertexarray[size].degree = vertices[newtype].degree;
		map->edgenumber += vertices[newtype].degree;
	//printf("currentvertex %d currentedge %d currentlabel %d type %d\n",currentvertex,currentedge,currentlabel,newtype);
		for(i=1;i<vertices[newtype].degree;i++) {
	  		enumeration_position[size][i].first = 0;
	  		edgestack[free_edge_number].first = size;
	  		edgestack[free_edge_number].second = i;
	  		free_edge_number ++;
		}
		size++;
      	}
    	else
      	{//remove the last vertex
	//remove the edge of the vertex
	//printf("remove \n");
			size--;       
			if(size == 0) break;
			map->edgenumber -= map->vertexarray[size].degree; 
			currentvertex = map->vertexarray[size].edges[0].vertexindex;
			currentedge = map->vertexarray[size].edges[0].edgeindex;
	//printf("currentvertex % d currentedge %d\n",currentvertex,currentedge);
			enumeration_position[currentvertex][currentedge].first++;
			if(free_edge_number == 0){//case with no free edge usable : we must add back the frozen edges
	  //printf("add back frozen edges \n");
	  		for(i=0;i<size;i++){
	    		for(j=0;j<map->vertexarray[i].degree;j++){
	      		if(map->vertexarray[i].edges[j].vertexindex == -1 && size < enumeration_position[i][j].second){//if an edge is used in a context we have just changed, free it back
				edgestack[free_edge_number].first = i;
				edgestack[free_edge_number].second = j;
				free_edge_number ++;
				enumeration_position[i][j].first = 0;
				enumeration_position[i][j].second = 0;
	      	}
	    }
	  }
	}
	else{free_edge_number += 1 - vertices[map->vertexarray[size].type].degree;}//remove the unused free edges
	if(size == mapsize-1 ){//case with a full size map
	  affichage_dynamique(backbonenumber, h);
	  backbonenumber++;//should increment this value before testing smallest representation and almost foldable
	  if(!labelvalues)
	    {
			map->edgenumber += map->vertexarray[size].degree;	//this is a hack, but the whole function should be changed
	      	folding_enumeration(map, h, outline);
	      	map->edgenumber -= map->vertexarray[size].degree;	
	    }
	  //printmap(map);
	}//remove the free edges of the motif 
	labelvalues -= vertices[map->vertexarray[size].type].labelvalue;
	currentlabel =  vertices[map->vertexarray[currentvertex].type].edges[currentedge];
	map->vertexarray[currentvertex].edges[currentedge].vertexindex =-1;
	//printf("currentvertex %d currentedge %d pos %d degree %d currentlabel %d \n",currentvertex,currentedge,enumeration_position[currentvertex][currentedge], connection[currentlabel].vertexnumber,currentlabel);
	if(enumeration_position[currentvertex][currentedge].first < connection[currentlabel].vertexnumber) //if there is still possible connections add back the edge we have freed by removing the last motif
	  {
	    //printf("add edge back \n");
	    edgestack[free_edge_number].first = currentvertex;
	    edgestack[free_edge_number].second = currentedge;
	    free_edge_number++;
	  }
	else{enumeration_position[currentvertex][currentedge].second = size;}
    }
  }
  free(edgestack);
  for(i=0;i<mapsize;i++) free(enumeration_position[i]);
  free(enumeration_position);
  return backbonenumber;
}



