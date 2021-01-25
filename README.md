# Gygès
Vous trouverez ici le jeu de Gygès créé durant notre DUT informatique.

Le Gygès est un jeu de plateau très peu connu. Cette vidéo explique rapidement les règles : [https://www.youtube.com/watch?v=zPjWHtUu6pY](https://www.youtube.com/watch?v=zPjWHtUu6pY).

Notre `board.c` devait respecter la documentation décrite [ici](https://dorbec.users.greyc.fr/M1106/doc/gyges/board_8h.html).

## Installation et compilation

Ce jeu nécessite l'installation de SDL2. La première commande est donnée à titre indicatif, elle permet de l'installer sur les distributions basées sur Debian.

```
$ sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
$ git clone https://github.com/ribt/gyges/
$ cd gyges
$ make
```

Il est également possible de jouer dans le terminal si vous n'avez pas SDL2 en faisant `make cli` puis `./gyges_cli`.

## Utilisation

```
$ ./gyges
```
### Contrôles

Le jeu se joue entièrement à la souris. L'écran d'accueil permet d'activer ou non la variante autorisant le *swap* (déplacer une pièce lorsqu'on arrive dessus) et de choisir le niveau de l'ordinateur (on peut également jouer sans ordinateur, à deux sur le même ordi). Le positionnement des pièces fonctionne par glisser-déposer. Ensuite, il suffit de cliquer sur une pièce pour la sélectionner et de cliquer sur les boutons de direction pour la déplacer.

## TO DO
- [x] Ajouter un bot pour jouer en solo.
- [ ] Ajouter un mode multijoueur en LAN.
