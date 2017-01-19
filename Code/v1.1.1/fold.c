#include <stdlib.h>
#include <time.h>
#include "structure.h"
#include "fold.h"
#include "hash.h"
#include <string.h> 

//Print function for debug//
void printmap(Map *map){
	int i,j;
	printf("Maps with %d vertices \n",map->vertexnumber);
	for(i=0;i < map->vertexnumber;i++)
	{
		printf("Type : %d Degree %d Neighbour :",map->vertexarray[i].type,map->vertexarray[i].degree);
		for(j=0;j <map->vertexarray[i].degree;j++)
		{
			printf("(%d,%d) ",map->vertexarray[i].edges[j].vertexindex,map->vertexarray[i].edges[j].edgeindex);
		}
		printf("\n");
	}
}

void printoutline(FreeEdge *outline, int outlinesize){
	int i;
	printf("Outline with %d edges: \n",outlinesize);
	for(i=0; i < outlinesize; i++)
	{
		printf("(%d,%d,%d) ",outline[i].label,outline[i].vertexindex,outline[i].edgeindex);
	}
	printf("\n");
}


//compute the outline of a Map with unconnected edge if it is a tree (should traverse the outer face in the case of a planar graph)
//use the fact that going down a vertex is the first in the next 
//could we do it along the creaton of the tree ?

int computeoutline(Map *map, FreeEdge *outline, Vertex* vert){
//if(!BACKBONETYPE){
//
//}
	int vertex=0, edge=0, outlinesize = 0, nextvertex;
	do{
    //printf("vertex : %d, edge : %d\n",vertex,edge);
    	nextvertex = map->vertexarray[vertex].edges[edge].vertexindex;//could be optimized for the case of paths
    	if(nextvertex == -1)
      	{//there is a free edge to add to the outline
      		outline[outlinesize].label = label.list[vert[map->vertexarray[vertex].type].edges[edge]];
      		outline[outlinesize].vertexindex = vertex;
      		outline[outlinesize].edgeindex = edge;
      		outlinesize++;
      		edge++;
      	}
    	else //we follow the edge to a new vertex
    	{
			edge = map->vertexarray[vertex].edges[edge].edgeindex + 1;//store the edge after the one we have used
			vertex = nextvertex;
		}	
    	if(edge == map->vertexarray[vertex].degree) {edge = 0;} //next edge in the neighborood 
  	}while(vertex != 0 || edge !=0); //stop when we are back to the first vertex and edge
  	return outlinesize;
}

int is_foldable(FreeEdge *w, int length){//return true when the backbone is foldable
	int i,j=1,counter=0;
	for(i=0;i<length;i++) { //could be done by a mempcy to speed up
    	previous[i] = i-1; //initialize the previous unfolded letter 
    }
    i=0;
  	while(j<length) {//fold in a greedy manner, if it does not work, then the backbone is not foldable
  		if(w[i].label + w[j].label == 0) {
  			counter+=2;
      		if(j == length-1){break;}//break now to avoid to manipulate values outside the array
      		if(previous[i]==-1) {
      			i=j+1;
      			j=j+2; 
      			previous[i] = -1;
      		}
      		else {
      			j++; 
      			i = previous[i]; 
      			previous[j] = i;
      		}
      	}
      	else {
      		i=j;
      		j++;
      	}
    }
    return (counter == length);  
  }

void precompute_foldable(FreeEdge *w, int length) //use only on foldable words, dynamic programming to compute the fold_matrix which contains 
//for each i the list of j which be folded with i so that the words obtained are still foldable
{ 
	int i,j,k;
	memset(mat,0,length*length*sizeof(int));
	memset(foldable_with,0,length*sizeof(int));//represent the last known element which can be folded with i
  	for(k=1;k <length;k+=2) 
    {//k is the distance between two letters we want to decide if they are foldable
    	for(i=0; i < length - k;i++) 
    	{
    		if( ((w[i].label+w[i+k].label)==0 && (k==1 || mat[(i+1)*length+i+k-1])) || (foldable_with[i] && mat[(foldable_with[i]+1)*length + i+k])) {
	    //either the word can be written  aw-a with w foldable or w1w2 with w1 and w2 foldable
    			mat[i*length+i+k] =1;
    			foldable_with[i]=i+k;
    		} 
    	}
    } 
    for(i=0;i < length-1;i++) {
    	k=0;
    	for(j=i+1; j< length;j++) 
    	{
    		if(mat[i*length+j] && (w[i].label + w[j].label == 0) ) {fold_matrix[i][k]=j; k++;}
    	}
    	fold_matrix[i][k]=length; // mark the end of the array
	}
}


void generation(Map *map, Hasht* h, FreeEdge *outline,int index_to_fold, int index_folded,int folded_letter,pair word_to_fold, pair word_folded){//Fold procedure
	while(1){
    if(index_to_fold == -1) {//word totally folded, store the produced map
      //printmap(&map);
    	hash_insert(h, map);
    } 
    else {
    	word_to_fold = to_fold[index_to_fold];
      folded_letter = fold_matrix[word_to_fold.first][call_stack[index_folded]];//next letter which can be folded with the current letter
      if(folded_letter <= word_to_fold.second) {
	     index_to_fold --; //remove the word being folded
	     folded[index_folded].first = word_to_fold.first; //write the folded pair in folded
	     folded[index_folded].second = folded_letter;
	//create the two corresponding edges in the map between first and second vertex
	     map->vertexarray[outline[word_to_fold.first].vertexindex].edges[outline[word_to_fold.first].edgeindex].vertexindex = outline[folded_letter].vertexindex;
	     map->vertexarray[outline[word_to_fold.first].vertexindex].edges[outline[word_to_fold.first].edgeindex].edgeindex = outline[folded_letter].edgeindex;
	     map->vertexarray[outline[folded_letter].vertexindex].edges[outline[folded_letter].edgeindex].vertexindex = outline[word_to_fold.first].vertexindex;
	     map->vertexarray[outline[folded_letter].vertexindex].edges[outline[folded_letter].edgeindex].edgeindex = outline[word_to_fold.first].edgeindex;
	//insert the 2 new subwords to fold if they aren't of size 0
	     if(word_to_fold.second != folded_letter) {
	     	to_fold[++index_to_fold].first = folded_letter +1 ;
	     	to_fold[index_to_fold].second =  word_to_fold.second; 
	     }
	     if(folded_letter != word_to_fold.first + 1) {
	     	to_fold[++index_to_fold].first = word_to_fold.first + 1;
	     	to_fold[index_to_fold].second = folded_letter - 1;
	     }
	     index_folded++;
	     call_stack[index_folded]=0;//reinitialize next possible fold
	     continue;
	 }
	}
    if(index_folded == 0) {break;}//stop when all possibilities have been exhausted

    index_folded--;
    word_folded = folded[index_folded];
    if(word_folded.first +1 == word_folded.second) { //the two elements are consecutive
    	if(index_to_fold == -1 || word_folded.second +1 != to_fold[index_to_fold].first) {
    		to_fold[++index_to_fold] = folded[index_folded];
    	}
      //the pair of element is put as it is in to_fold
    	else {
    		to_fold[index_to_fold].first= word_folded.first;
    	}
      //the pair of elements is concatenated in front of the first pair of to_fold
    }

    else { //the two elements are not consecutives
    	if(index_to_fold == 0 || to_fold[index_to_fold -1].first != word_folded.second +1) {
    		to_fold[index_to_fold]= word_folded;
    	}
      //the pair of elements is concatenated around the first pair of to_fold
    	else {
    		to_fold[--index_to_fold].first = word_folded.first;
    	} 
      //the pair of elements is concatenated with the two first pairs of to_fold
    }
    call_stack[index_folded]++;
}
}



//function which produces all maps obtained by folding the outline of a backbone
void folding_enumeration(Map *map, Hasht *h, FreeEdge *outline)
{ 
  //Begin of the folding
	int i,j;
	int index_to_fold=0;
  	int index_folded=0;// number of pairs folded in folded 
  	int folded_letter=0; // current letter being folded to the first letter of word_to_fold.first
  	pair word_to_fold; //current subword
  	pair word_folded; // pair of letter to unfold 
  	int outlinesize = computeoutline(map,outline,vertices);//compute the outline
  //printoutline(outline,outlinesize);
  	if(is_foldable(outline,outlinesize))//test wether the word is foldable
  	{
  	  precompute_foldable(outline, outlinesize); //populates the helper structures 


      if(BACKBONETYPE == 2)//deal with the case of cycles or paths with the proper initialization before generation() is called
      {
      	int element; 
      	for(i=0; i < outlinesize -1;i++){
      		j=0;
	    while(fold_matrix[i][j] != outlinesize)//only consider element foldable with the first and wich are connected to the last
	    {
	    	element = fold_matrix[i][j];
		if(( outline[i].vertexindex == 0 && outline[element].vertexindex == map->vertexnumber -1) || (outline[i].vertexindex == map->vertexnumber -1  && outline[element].vertexindex == 0  )) //test whether the two considered foldable free vertices are attached to the first and last motif 
		{
		    //we fold the two vertices 
			map->vertexarray[outline[i].vertexindex].edges[outline[i].edgeindex].vertexindex = outline[element].vertexindex;
			map->vertexarray[outline[i].vertexindex].edges[outline[i].edgeindex].edgeindex = outline[element].edgeindex;
			map->vertexarray[outline[element].vertexindex].edges[outline[element].edgeindex].vertexindex = outline[i].vertexindex;
			map->vertexarray[outline[element].vertexindex].edges[outline[element].edgeindex].edgeindex = outline[i].edgeindex;
			index_to_fold=-1;
		    call_stack[0] = 0;//initialize the stack for the recursive calls
		    if(i!= 0)
		    {
		    	index_to_fold++;
		    	to_fold[0].first = 0; 
		    	to_fold[0].second = i-1;

		    }
		    if(i+1 != element)
		    {
		    	index_to_fold++;
		    	to_fold[index_to_fold].first = i+1; 
		    	to_fold[index_to_fold].second = element-1;
		    }
		    if(element != outlinesize -1)
		    {
		    	index_to_fold++;
		    	to_fold[index_to_fold].first = element+1; 
		    	to_fold[index_to_fold].second = outlinesize-1;
		    }
		    generation(map, h, outline, index_to_fold, index_folded, folded_letter, word_to_fold, word_folded);
		}
		j++;
	}
}
}
      else{//generation from a path or a tree
	call_stack[0] = 0;//initialize the stack for the recursive calls
	to_fold[0].first = 0; 
	to_fold[0].second = outlinesize-1;
	generation(map, h, outline, index_to_fold, index_folded, folded_letter, word_to_fold, word_folded);
}
for(i=0; i < outlinesize; i++) 
	map->vertexarray[outline[i].vertexindex].edges[outline[i].edgeindex].vertexindex = -1; //reinitialize the free edges to -1
}	
}




