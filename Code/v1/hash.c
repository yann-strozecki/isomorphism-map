#include "hash.h"
#include <string.h>

unsigned int hash_basic(unsigned int* tab){//the size of the tab is written in the first element
  unsigned int res = tab[0];
  //printf("Size of the signature %d \n",res);
  for (int i = 1; i < tab[0]; i++){
    res ^= (tab[i] << i) | (tab[i] >> (sizeof(int)*8 - i));  //rotation
  }
  return (res%HASH_SIZE);
} 


Heap* initialize_heap(){//Pour que la structure fonctionne correctement, il faut HEAP_SIZE > element_max_size
  Heap* hp = malloc(sizeof(Heap));
  hp->heap_level = 0;
  hp->memory[0] = malloc(HEAP_SIZE * sizeof(elem));
  hp->avalaible_space = HEAP_SIZE;
  hp->next_position = hp->memory[0];
  hp->heap_level = 0;
  return hp;
}

Hasht *initialize_hasht(){
	Hasht *h = malloc(sizeof(Hasht));
	h->elements = 0;
	h->insertions = 0;
	h->nombre_map = 0;
	h->hash = calloc(sizeof(elem*),HASH_SIZE);
	h->hp = initialize_heap();
	return h;
}

void free_hasht(Hasht *h){
  for(int i = 0; i <= h->hp->heap_level ; i++){
    free(h->hp->memory[i]);
  }
  free(h->hash);
  free(h->hp);
  free(h);
}


void affiche_hasht(Hasht *h){
	printf("Insertions dans la table  %d, nombre de signatures dans la table %d, nombre de maps %d\n",h->insertions,h->elements,h->nombre_map);
	printf("Niveau dans le tas %d, Espace restant à ce niveau %d\n",h->hp->heap_level,h->hp->avalaible_space);
	int taille, max = 0;
	unsigned long int somme = 0;
	int nombre_hash = 0;
	elem* pos;
  	for(int i = 0; i < HASH_SIZE; i++ ){
    	pos = h->hash[i];
    	taille = 0;
    	while(pos != NULL){
    		taille++;
    		pos = pos->next;
    	}
      	//printf("Position %d occupée avec %d éléments \n",i,nombre);
      	if(max < taille) max = taille;
      	somme += taille;
      	nombre_hash++;
    }
  	printf(" Plus grande liste dans la table: %d éléments\n",max);
  	printf(" Taille moyenne des listes: %f éléments\n", (float)somme / (float)nombre_hash);
}


elem* write_signature_heap(Heap *hp, unsigned int* tab){
	
  int size = tab[0];  
  if(hp->avalaible_space < size + 1){//gestion dynamique de la mémoire
    hp->heap_level++;
    if(hp->heap_level == 16) {  //extension d'un niveau de la zone de stockage des signatures
      printf("Trop de mémoire utilisée, échec. \n");
      exit(22); 
    }
    printf("\n Allocation de mémoire pour le tas de niveau %d\n", hp->heap_level);
    hp->memory[hp->heap_level] = malloc((HEAP_SIZE << hp->heap_level) * sizeof(elem));
    hp->avalaible_space = HEAP_SIZE << hp->heap_level;
    hp->next_position = hp->memory[hp->heap_level];
  } 
	hp->avalaible_space -= size + 1; //on consomme de la place pour la signature + le pointeur sur l'élément suivant
  hp->next_position->next = NULL; //marque la fin de la liste de signatures
	memcpy(hp->next_position + 1, tab, sizeof(unsigned int)*size);
  elem* res = hp->next_position;
	hp->next_position += size + 1;
	hp->avalaible_space -= size + 1;	
  return res;  	
}

int insert_hasht(Hasht *h, unsigned int* tab){//return 0 when the element was already there, 1 when we insert it
  unsigned int key = hash_basic(tab);
  //printf("Clé %d \n",key);
  h->insertions++; 
  //insertion with a key not yet used
  if(h->hash[key] == NULL){
    h->hash[key] = write_signature_heap(h->hp,tab);
    h->elements++;
    return 1;
  }
  elem* pos = h->hash[key];
  //possible qu'il faille mettre + 1 pour la taille comparée
  while(memcmp(tab,pos + 1,tab[0]*sizeof(unsigned int))){//the element is different
    if(pos->next == NULL){ //an element is added at the end of the list
      pos->next = write_signature_heap(h->hp,tab);
      h->elements++;
      return 1; 
    }
    pos = pos->next; //address of the next element
  }
  return 0;// the element is found, nothing to insert
  //printf("insertion level %d\n",h->aux_level);
}

void hash_insert(Hasht* h, Map* M){
  signature_compute(M);
  insert_hasht(h, M->signature); 
}