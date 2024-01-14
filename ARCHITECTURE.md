# projet-sy5

Le fichier affichage.c rassemble les fonctions liées à l'affichage que ce soit l'état des jobs ou le prompt.

Le fichier commandes.c contient les fonction cd et ls.

Comme son nom l'indique redirections.c gère les redirections.

Tout le reste de la logique est géré dans jsh.c avec la fonction execute qui s'occupe en fonction de ce qui ets écrit de redistribuer le travail.
Pour les jobs nous avons choisi une structure avec un statut, un index, un pid qui est aussi le pid du groupe et la commande qui l'a crée.