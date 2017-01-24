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
deuxième méthode: quand on calcule les almost foldable path/tree, on pourrait calculer le nombre de noeuds de chaque type.
Dans les graphes possibles, on regarde le couple noeud arrête qui apparaît le moins.

Ajouter une cible bench dans le makefile avec le test de cinq générateurs assez rapide et
représentatifs. 
Faire un clean à la fin d'un run et ajouter un gitignore pour éviter les fichiers inutiles.
Quand on teste IVK, il n'y a presque pas de rotation de sommet. C'est étrange, y-at-il un bug ? 

Peu de différence entre v3 et v4 car les deux doivent toujours sélectionner la même chose.
Sur d'autres exemples ou des graphes random, ça serait différent.

Sur la variante 5, peut être que calculer le minimum sur tout le tableau n'est pas si énorme que ça,
à condition de faire un tableau bien tight (et donc de réfléchir à la taille max d'un cycle).
Ça permet de simplifier le code en virant la liste. 




Écrire le plan d'un papier.



v1: version de base (on peut jouer éventuellement sur l'ordre des trois couleurs sur l'arête)
v2: on ajoute une nouvelle couleur, la taille de la face
v3: on ne calcule que les signatures commencant par un sommet et une arrête choisis intelligemment au début du programme
v4: on calcule que les signatures commencant par une paire (sommet, arête) qui apparaît un minimum de fois dans le graphe
v5: on calcule que les signatures commencant par un triplet (sommet, arête, taille de face) qui apparaît un minimum de fois dans le graphe
v6: on ne calcule que la signature faible pour tester l'existence et on insère toutes les signatures quand la map n'est pas encore stockée
v7: on commence par un couple (sommet,arête) fixé au début de l'algo