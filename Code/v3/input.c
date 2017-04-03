#include "structure.h"
#include <math.h>
#include "fold.h"

//#################### Print functions ###########################

void printcompatible(int size){
  int i,j;
  for(i=0;i<size;i++)
  {
    printf("Label %d:",i);
    for(j=0; j< connection[i].vertexnumber;j++)
    {
     printf("Vertex %d ",connection[i].list[j]);
    }
   printf("\n");
  }
}

void printvertices(int size, Vertex *vert){
  int i,j;
  printf("%d vertices \n", size);
  for(i=0; i<size;i++) {
    printf("Degree: %d, Id:  %d, Value: %d Edges: ",vert[i].degree,vert[i].id,vert[i].labelvalue);
    for(j=0; j < vert[i].degree; j++) {
      printf("%d ",vert[i].edges[j]);
    }
    printf("\n");
  }
}

void printbinary(int code, int size)
{
  int i;
  for(i=0;i<size;i++)
  {
    printf("%d", code%2);
    code /= 2;
  }
}

void printmatrix(unsigned int ***mat, int dim1, int dim2,int dim3)
{
  int i,j,k;
  for(i=dim1-1; i>=0;i--)
  {
    printf("\n Size %d\n",dim1 - i);
    for(j=0;j<dim2;j++){
     printf("\n Complementary label %d\n",j);
     for(k=0;k<dim3;k++)
     {
       if(mat[i][j][k]){
         printf("%d",k); //we should decode this value
         printf("Set ");
         printbinary(mat[i][j][k],dim2);
         printf("; ");
       }
     }
   }
   printf("\n"); printf("\n");
 }
}



void printmatrixtree(unsigned int **mat, int dim1, int dim2,int magicnumber,int codesize){
  int i,j;
  for(i=0; i<dim1;i++)
  {
    printf("\n Complementary size %d\n",i);
    for(j=0;j<dim2;j++){
     if( mat[i][j])	printf("%d",j);
   }
   printf("\n");
 }
}

//#################### Input functions ###################################

//Create all vertices given in the .mot, return the number of vertices

int read_input(char* inputname, int *maxdegree){
  FILE* inputfile = NULL;
  int verticesnumber, degree, i, j;
  int ret;
  int period; //is not used, should be removed from the file format
  inputfile = fopen(inputname, "r");
  ret = fscanf(inputfile, "%d %d", &verticesnumber, &period);
  vertices = malloc(sizeof(Vertex)*verticesnumber);
  *maxdegree = 0;
  for(i=0; i < verticesnumber;i++) {
    ret = fscanf(inputfile, "%d", &(vertices[i].letter));
    ret = fscanf(inputfile, "%d", &degree);
    if (degree > *maxdegree) *maxdegree = degree;
    vertices[i].degree = degree;
    vertices[i].id = i;
    vertices[i].edges = malloc(sizeof(int)*degree);
    for(j=0; j < degree; j++) {
      ret = fscanf(inputfile,"%d", &(vertices[i].edges[j]));
    } 
  }
  fclose(inputfile);
  if (ret<0) exit(11);
  return verticesnumber;
} 


/////////////Fonctions to create datastructures with important precomputed informations to speed up path and tree enumeration////

//Index the labels by their order of apparition in vertices and store that in label. 
//The negative labels are stored after the positive ones in the same order

void normalize_labels(int size, Vertex *vert ){ 
  int i,j,k,labelnumber=0;
  label.list = malloc(sizeof(int)*10); //we have at most 10 different labels, should be dynamic 
  for(i=0; i < size; i++) {//create label.list
    for(j=0; j < vert[i].degree; j++) {
      for(k=0; k< labelnumber && label.list[k] != abs(vert[i].edges[j]);k++);
      if(k == labelnumber) {
        label.list[labelnumber] = abs(vert[i].edges[j]);
        labelnumber++;
      }
    }
  }
  for(i=0;i <labelnumber;i++){label.list[labelnumber+i] = -label.list[i];}
  label.size = 2*labelnumber; 
  for(i=0; i < size; i++){ 
    for(j=0; j < vert[i].degree; j++){
      k=0;
      while(label.list[k] != vert[i].edges[j]) {k++;}
      vert[i].edges[j] = k;
    }
  }
}

//Compute the field labelvalue of eachvertex and label.shift
void compute_label_values(int size, Vertex * vert){
  int labelnumber = label.size/2;
  int *label_count = calloc(labelnumber,sizeof(int));
  int *label_max = calloc(labelnumber+1,sizeof(int));
  for(int i=0; i < size; i++){ 
    for(int j=0; j < vert[i].degree; j++){
      if(vert[i].edges[j]< labelnumber){
        label_count[vert[i].edges[j]]++;
      }
      else{
        label_count[vert[i].edges[j]-labelnumber]--;
      }
    }
    for(int l = 0; l < labelnumber; l++){//update the maximal numbero of label of each type
      if( label_max[l] < abs(label_count[l])){ label_max[l] = abs(label_count[l]);} 
    }
  }
  free(label_count);

  label_max[labelnumber] = 1;
  label.shift = 0;
  for(int l = labelnumber-1; l >=0; l--){
    //printf("Label %d repeted at most %d times \n",l,label_max[l]*mapsize*2);
    label_max[l] *= mapsize*2*label_max[l+1]; //could be divided by two, but need to modify the generation of path and trees with an additionnal check
    label.shift += label_max[l]/2;
  } //it is a bound on the maximal positive values
  //compute the labelvalue of each vertex by adding the value of the label of each edge
  label.shift*=2;
  for(int i=0; i < size; i++){ 
    vert[i].labelvalue = 0;
    for(int j=0; j < vert[i].degree; j++) { 
      if(vert[i].edges[j] < labelnumber) {
        vert[i].labelvalue += label_max[vert[i].edges[j]+1];
      }
      else {
        vert[i].labelvalue -= label_max[vert[i].edges[j]-labelnumber+1];
      }
    }
  }
  free(label_max);
  //printf(" Max value of a tree %d\n",label.shift*2);
}


int non_isomorph(int *list, int size, int sh) { //return false if the list of edges shifted by sh is isomorph to the list shifted by less
  int j;
  for(int i=0; i < sh; i++) {
    for(j = 0; j < size && list[(j+i) % size] == list[(j + i + sh) % size];j++);
    if(j==size) return 0;
  }
  return 1;
}

 //Add to the list of vertices all vertices with a rotation on their edges (and do not add isomorphic copies of a vertex), return the number of vertices
//Could allocate the exact number of vertices, to spare some memory
Vertex *create_rotated_vertices(int *size, Vertex *vert){
  int i,j,k,maxverticesnumber=0,verticesnumber=*size;
  for(i=0;i<*size;i++)
  {
    maxverticesnumber+= vert[i].degree;
  }
  vert = realloc(vert, maxverticesnumber*sizeof(Vertex));
  for(i=0;i<*size;i++)//for each vertex add a rotated version if it is not already there 
  {
    for(j=1; j < vert[i].degree;j++)
    {
     if(non_isomorph(vert[i].edges, vert[i].degree,j))
     {
        vert[verticesnumber] =  vert[i];
        vert[verticesnumber].edges = malloc(sizeof(int)*vert[i].degree);
        for(k=0; k < vert[i].degree; k++)
        {
          vert[verticesnumber].edges[k] =  vert[i].edges[(j+k) % vert[i].degree]; 
        }
        verticesnumber++;
      }
    }
  }
  *size = verticesnumber;
  return vert;
}

//Create an helper array with for each label the vertices which can be connected by their first edge to this label
void create_concatenation_helper(int size, Vertex * vert)
{ 
  int i,j;
  int opposite = label.size/2;
  connection = malloc(sizeof(CompatibleVertices)*label.size);
  for(i=0;i<label.size;i++)
  {
    connection[i].list = malloc(sizeof(int)*size);
    connection[i].vertexnumber = 0;
    for(j=0;j<size;j++)
    {
     if( abs(vert[j].edges[0] - i) ==  opposite) {
       connection[i].list[connection[i].vertexnumber] = j;
       connection[i].vertexnumber++;
      }
    }
  }
}


//Create a multidimensional table which contains the information about the almost foldable trees
//could be improved by ensuring that the elements built are trees and not forest by storing the set of free
//edges as in almost foldable tree

void almost_foldable_tree(int vertexnumber, Vertex *vert)
{
  int maxsize = 2*label.shift + 1;
  almostfoldabletree = malloc(sizeof(unsigned int*)*mapsize);
  for(int i=0;i<mapsize;i++)//initialise mat to 0
  {
    almostfoldabletree[i] = calloc(maxsize,sizeof(unsigned int));
  }
  for(int i=0;i<vertexnumber;i++) almostfoldabletree[mapsize-1][vert[i].labelvalue + label.shift] = 1;
  for(int i=1;i<mapsize;i++) {
    for(int j=0;j<maxsize;j++){
      if(almostfoldabletree[mapsize -i][j]){
        for(int k=0;k<vertexnumber;k++) almostfoldabletree[mapsize-i-1][vert[k].labelvalue + j] = 1;
      }
    }
  }
  //printmatrixtree(mat,size,maxsize,MAGICNUMBER,alphabetsize);
}

//Create a multidimensional table which contains the information about the almost foldable paths

void almost_foldable_path(int vertexnumber, Vertex *vert)
{
  /////// Declaration and allocation ///////////////////////

  int i,j,k,index,value,l;
  pair **add_after_a_label = malloc(sizeof(pair*)*label.size); //will be used to add all possible vertices after an edge of given label
  for(i=0;i<label.size;i++) {
    add_after_a_label[i] = malloc(sizeof(pair)*vertexnumber);
  }
  int *sizes = calloc(label.size,sizeof(int));// sizes[i] is the size of add_after_a_label[i]
  int alphabetsize = label.size/2;
  int maxsize = 2*label.shift + 1;//third dimension of the matrix, correspond to the sum of values of the free edges in the map
  almostfoldablepath = malloc(sizeof(unsigned int**)*mapsize);
  //this matrix has three dimensions
  //the first is the maximum size of a path minus the size of the path it encodes information about
  //the second is the label of the first edge of the first vertex
  //the third is number of each type of free edge encoded in an integer
  //the value of a coefficient is a bivector encoded in an integer, which represent the labels
  //of the free edges of the last element

  //TODO: give the meaning of almost foldablepath[0] -> could be used in signature computation
  

  /////// Initialization ///////////////////////////////////

  for(i=0;i<mapsize;i++)//initialize almostfoldablepath to 0
  {
    almostfoldablepath[i] = malloc(sizeof(unsigned int*)*label.size);
    for(j=0;j<label.size;j++)
    {
      almostfoldablepath[i][j] = calloc(maxsize,sizeof(unsigned int));
    }
  }

  //Compute add_after_a_label and initialize almostfoldablepath[0]
  for(i=0;i<vertexnumber;i++)
  {
    value = vert[i].edges[0] + (vert[i].edges[0] >= alphabetsize ? -alphabetsize : alphabetsize); //label of the first edge (we index by its complement to facilitate the use of almostfoldablepath in concat)
    for(j=1;j<vert[i].degree;j++)//j=0 is used for the beginning of the path
    {
      almostfoldablepath[mapsize-1][value][vert[i].labelvalue + label.shift] |= 1<<vert[i].edges[j];//add a free edge to the possible end of the path of size 1
    }
    for(k=0;k < sizes[value] && add_after_a_label[value][k].first != vert[i].labelvalue; k++);
    if(k == sizes[value]){//a new labelvalue is found
      add_after_a_label[value][sizes[value]].first = vert[i].labelvalue;//first is the labelvalue
      add_after_a_label[value][sizes[value]].second = almostfoldablepath[mapsize-1][value][vert[i].labelvalue + label.shift];//second is the set of labels of ending edges we have just computed
      sizes[value]++;
      //printf("add labelvalue %d label %d bitset %d vertexnumber %d\n",vert[i].labelvalue,value,almostfoldablepath[mapsize-1][value][vert[i].labelvalue+label.shift],i);
    }
    else{//the same labelvalue is stored at position k, update the bitset only (by union)
      add_after_a_label[value][k].second |= almostfoldablepath[mapsize-1][value][vert[i].labelvalue + label.shift]; 
    }
  }

  ///// Incremental build of almostfoldablepath ////////////////////////////
  for(i = mapsize-1;i>0;i--)
  {
    for(j=0;j<label.size;j++)//for all first label of the path
    {
      for(index=0; index < maxsize;index++)//for all values of Cg (vector of number of free elements in the alphabet)
      {
        if(almostfoldablepath[i][j][index])//if there is a compatible path
        {
		      for(k=0;k<label.size;k++)//for all possible last edge of the path add the possible end motifs
          {
		        if(1<<k & almostfoldablepath[i][j][index]){//if the label k is a valid end label
              for(l=0;l < sizes[k]; l++)
              {
                almostfoldablepath[i - 1][j][index + add_after_a_label[k][l].first] |=  add_after_a_label[k][l].second; 
              }
            }
          }
        }
      }
    }
  }
  //printmatrix(almostfoldablepath,mapsize,label.size,maxsize);
  //free the intermediate structures
  for(i=0;i<label.size;i++){free(add_after_a_label[i]);}
  free(add_after_a_label);
  free(sizes);
}
