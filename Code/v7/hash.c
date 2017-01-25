#include "hash.h"
#include <string.h>


//we could use the fact that every element of tab is in fact encoded on 32 bits
//to put the signature by block of 32 bits and do the hashing with 64 bits elements

unsigned int hash_basic(unsigned int* tab){//the size of the tab is written in the first element
  unsigned  int res = tab[0];
  //printf("Size of the signature %d \n",res);
  for (int i = 1; i< tab[0]; i++){
    res ^= (tab[i] << i) | (tab[i] >> (sizeof(int)*8 - i));  //rotation
  }
  return (res%HASH_SIZE);
} 


Heap* initialize_heap(){
  Heap* hp = malloc(sizeof(Heap));
  hp->elements_space = HASH_SIZE;
  hp->elements[0] = malloc(HASH_SIZE * sizeof(elem));
  hp->next_element = hp->elements[0];
  hp->elements_level = 0;
  hp->signatures_space = HEAP_SIZE;
  hp->signatures[0] = malloc(HEAP_SIZE * sizeof(unsigned int));
  hp->next_signature = hp->signatures[0];
  hp->signatures_level = 0;
  return hp;
}

Hasht *initialize_hasht(){
  Hasht *h = malloc(sizeof(struct hasht));
  h->elements=0;
  h->insertions=0;
  h->nombre_map = 0;
  h->hash = malloc(sizeof(elem)*HASH_SIZE);
  h->hp = initialize_heap();
  for(int i = 0; i < HASH_SIZE;i++){//could use a memset
    h->hash[i].data = NULL;
    h->hash[i].next = NULL;
  }
  return h;
}


unsigned int* write_signature_heap(Heap *hp, unsigned int* tab){
  int size = tab[0] ;
  if(size > hp->signatures_space){
    hp->signatures_level++;
    if(hp->signatures_level == 8) {  //extension d'un niveau de la zone de stockage des signatures
      printf("Trop de mémoire utilisée, échec. \n");
      exit(22); }
    printf("\nAllocation de mémoire pour les signatures de niveau %d\n", hp->signatures_level);
    hp->signatures[hp->signatures_level] = malloc((HEAP_SIZE << hp->signatures_level) * sizeof(int));
    hp->signatures_space = HEAP_SIZE << hp->signatures_level;
    hp->next_signature = hp->signatures[hp->signatures_level] ;
  }
  hp->signatures_space -= size;
  memcpy(hp->next_signature, tab, sizeof(int)*size);
  hp->next_signature += size;
  return (hp->next_signature - size);
}


elem* write_element_heap(Heap *hp){
  if(hp->elements_space == 0){
    hp->elements_level++;
    if(hp->elements_level == 8) {  //extension d'un niveau de la zone de stockage des signatures
      printf("Trop de mémoire utilisée, échec. \n");
      exit(22); }
    printf("\nAllocation de mémoire pour les elements de niveau %d\n", hp->elements_level);
    hp->elements[hp->elements_level] = malloc((HASH_SIZE << hp->elements_level) * sizeof(elem));
    hp->elements_space = HASH_SIZE << hp->elements_level;
    hp->next_element = hp->elements[hp->elements_level] ;
  }
  hp->elements_space --;
  (hp->next_element)->next = NULL;
  hp->next_element ++;
  return (hp->next_element - 1);
}

int insert_hasht(Hasht *h, unsigned int* tab){//return 0 when the element was already there, 1 when we insert it
  unsigned int key = hash_basic(tab);
  //printf("Clé %d \n",key);
  h->insertions++; 
  //insertion with a key not yet used
  if(h->hash[key].data == NULL){
    h->hash[key].data = write_signature_heap(h->hp,tab);
    h->hash[key].next = NULL;//end the list
    h->elements++;
    return 1;
  }
  elem* current_element =&h->hash[key];
  while(memcmp(tab,current_element->data,tab[0]*sizeof(int))){//the element is different
    if(current_element->next == NULL){ //an element is added at the end of the list
      current_element->next = write_element_heap(h->hp);
      (current_element->next)->data = write_signature_heap(h->hp,tab);
      h->elements++;
      return 1; 
    }
    current_element = current_element->next; 
  }
  return 0;// the element is found, nothing to insert
  //printf("insertion level %d\n",h->aux_level);
}

void fast_insert_hasht(Hasht *h, unsigned int* tab){//optimized version when we know that the signature is not in the hashtable
  unsigned int key = hash_basic(tab);
  //printf("Clé %d \n",key);
  h->insertions++; 
  h->elements++;
  if(h->hash[key].data == NULL){
    h->hash[key].data = write_signature_heap(h->hp,tab);
    h->hash[key].next = NULL;//fin de liste à 0
    return;
  }
  elem* current_element =&h->hash[key];
  while(current_element->next != NULL){current_element = current_element->next;}
  current_element->next = write_element_heap(h->hp);  
  (current_element->next)->data = write_signature_heap(h->hp,tab);
  //printf("insertion level %d\n",h->aux_level);
}

void affiche_hasht(Hasht *h){
  printf("Insertions dans la table  %d, nombre de signatures dans la table %d, nombre de maps %d\n",h->insertions,h->elements,h->nombre_map);
  printf("Level de signatures %d, Espace restant %d\n",h->hp->signatures_level,h->hp->signatures_space);
  printf("Level d'éléments %d, Espace restant %d\n",h->hp->elements_level,h->hp->elements_space);
  int taille, max = 0;
  unsigned long int somme = 0;
  int nombre_hash = 0;
  elem* current_element;
  for(int i = 0; i < HASH_SIZE; i++ ){
    if(h->hash[i].data != NULL){
      taille = 0;
      current_element = &h->hash[i];
      do{
        taille++;
        current_element = current_element->next;
      }while(current_element != NULL );
      //printf("Position %d occupée avec %d éléments \n",i,nombre);
      if(max < taille) max = taille;
      somme += taille;
      nombre_hash++;
    }
  }
  printf(" Plus grande liste dans la table: %d éléments\n",max);
  printf(" Taille moyenne des listes: %f éléments\n", (float)somme / (float)nombre_hash);
}

void free_hasht(Hasht *h){
  for(int i = 0; i <= h->hp->signatures_level ; i++){
    free(h->hp->signatures[i]);
  }
  for(int i = 0; i <= h->hp->elements_level ; i++){
    free(h->hp->elements[i]);
  }
  free(h->hash);
  free(h->hp);
  free(h);
}

void hash_insert(Hasht* h, Map* M){
  M->signature[0] = M->edgenumber; 
  int vertex , edge;
  for(vertex = 0; vertices[M->vertexarray[vertex].type].id != rarest_vertex_id; vertex++);
  for(edge=0 ; vertices[M->vertexarray[vertex].type].edges[edge] != rarest_edge_label; edge++);
  signature_vertex_edge(M,vertex,edge);
  if(insert_hasht(h, M->signature)){
    h->nombre_map++;
    for(int i = edge+1; i < M->vertexarray[vertex].degree ; i++){
      if(vertices[M->vertexarray[vertex].type].edges[i] == rarest_edge_label){
        signature_vertex_edge(M,vertex,i);
        fast_insert_hasht(h, M->signature);
      }
    }
    for (int i = vertex+1 ; i<M->vertexnumber; i++){
      if(vertices[M->vertexarray[i].type].id == rarest_vertex_id){
        for (int j = 0 ; j < M->vertexarray[i].degree ; j++){
          if(vertices[M->vertexarray[i].type].edges[j] == rarest_edge_label){
            signature_vertex_edge(M,i,j);
            fast_insert_hasht(h, M->signature);
          }
        }
      }
    }
  }
}
