La version de base avec un simple DFS sans aucun raffinement.


Remarques après test:

les graphes ont très peu d'automorphismes -> étonnant,
il faudrait que je reteste à avec mon nouveau code de table de hachage.
Si c'est vrai que presque aucun graphe n'a d'automorhisme ça explique des choses sur les performances de certaines méthode d'isomorphisme.


À faire:  
-dans le code c, l'instrumentaion pour générer les informations utiles (nombres d'étapes totales, nombre de DFS, nombre d'étapes dans un DFS)
à écrire dans un fichier pour utiliser simplement un script après 
-différentes manières de calculer la valeur d'un noeud dans le DFS pour montrer que ça ne change pas vraiment
pour IVV l'ordre ne change pas et la supression de la couleur de l'arête non plus
-> dans certains cas la couleur de l'arête n'ajoute pas d'info
->pour YVV un tout petit peu de changement mais c'est dans le bruit (5%)
-ajouter le calcul de des faces pour enrichir la couleur -> ça ne change toujours pas vraiment
mais ça coute un peu (environ 15% de temps de calcul dans la signature en plus).
-M->signature[0] = M->edgenumber; on devrait avoir seulement M->signature[0] et pas M->edgenumber qui ne sert à rien
-on pourrait coder les éléments de la signature sur 32 bits pour accélérer les lectures et copies et calcul de hash,
et diviser la taille de stockage par 2. Pas sûr qu'il y ait une amélioration de performance.
-relire ce readme pour voir ou on en est (tout n'est pas à jour)
-dans certains cas, on peut ne pas mettre le premier noeud dans la signature (couleur du premier noeud et premiere arête fixée)
dans d'autre cas on peut sans doute éviter le dernier, voir plus
-au début on pourrait allouer des tableaux de mémoire d'exactement la taille dont on a besoin,
et mieux organiser les infos dont on a besoin pour mettre plus facilement tout ça en cache
-je ne comprends plus l'optimisation sur la deuxième boucle de la génération de chemin


-différentes méthodes de sélection du premier noeud -> 
première méthode: sommet ayant l'arête de plus petite multiplicité, et de degré max. (implémenté dans v3) 
deuxième méthode: quand on calcule les almost foldable path/tree, on pourrait calculer le nombre de noeuds de chaque type.
Dans les graphes possibles, on regarde le couple noeud arrête qui apparaît le moins.
La version v3alt précalcule et explore les combinaisons de noeuds qui respectent les critères de compatibilité sur les arêtes (chaque arête doit être compensée par son arête complémentaire), et détermine le couple noeud arrête qui apparait le moins. Cette version "intelligente" de la première méthode ne tient juste pas compte des arrangements possibles (i.e. il s'agit d'une moyenne arithmétique, et non pondérée)

Ajouter une cible bench dans le makefile avec le test de cinq générateurs assez rapide et
représentatifs. 
Faire un clean à la fin d'un run et ajouter un gitignore pour éviter les fichiers inutiles.
Quand on teste IVK, il n'y a presque pas de rotation de sommet. C'est étrange, y-at-il un bug ? 

Peu de différence entre v3 et v4 car les deux doivent toujours sélectionner la même chose.
Sur d'autres exemples ou des graphes random, ça serait différent.

Sur la variante 5, peut être que calculer le minimum sur tout le tableau n'est pas si énorme que ça,
à condition de faire un tableau bien tight (et donc de réfléchir à la taille max d'un cycle).
Ça permet de simplifier le code en virant la liste. 

Changer dans la table de hash avalaible space de int en unsigned int fait tout crasher, pourquoi ?

Écrire le plan d'un papier.



v1: version de base (on peut jouer éventuellement sur l'ordre des trois couleurs sur l'arête)
v2: on ajoute une nouvelle couleur, la taille de la face
v3/v3alt: on ne calcule que les signatures commencant par un sommet et une arrête choisis intelligemment au début du programme
v4: on calcule que les signatures commencant par une paire (sommet, arête) qui apparaît un minimum de fois dans le graphe
v5: on calcule que les signatures commencant par un triplet (sommet, arête, taille de face) qui apparaît un minimum de fois dans le graphe
v6: on ne calcule que la signature faible pour tester l'existence et on insère toutes les signatures quand la map n'est pas encore stockée
v7/v7 alt: on commence par un couple (sommet,arête) fixé au début de l'algo (choisi de manière naïve ou intelligente)
