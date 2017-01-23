La version de base avec un simple DFS sans aucun raffinement.

À faire:  
-dans le code c, l'instrumentaion pour générer les informations utiles (nombres d'étapes totales, nombre de DFS, nombre d'étapes dans un DFS)
à écrire dans un fichier pour utiliser simplement un script après 
-différentes manières de calculer la valeur d'un noeud dans le DFS pour montrer que ça ne change pas vraiment
pour IVV l'ordre ne change pas et la supression de la couleur de l'arête non plus
-> dans certains cas la couleur de l'arête n'ajoute pas d'info
->pour YVV un tout petit peu de changement mais c'est dans le bruit (5%)
-ajouter le calcul de des faces pour enrichir la couleur -> ça ne change toujours pas vraiment
mais ça coute un peu (environ 15% de temps de calcul dans la signature en plus).

-différentes méthodes de sélection du premier noeud -> 
première méthode: sommet ayant l'arête de plus petite multiplicité, et de degré max. 
deuxième méthode: quand on calcule les almost foldable path/tree, on calcule le nombre de noeuds de chaque type.
On regarde le noeud qui apparaît le moins et sa plus petite arête.

on gagne un peu notamment en utilisant la taille des faces
-méthode de la signature faible + différentes sélection de premier noeud

Écrire le plan d'un papier.


