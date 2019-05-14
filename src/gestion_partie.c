#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <commun.h>
#include <joueur.h>
#include <gestion_tour.h>
#include <gestion_partie.h>

/**
	*\fn void initialisation_manche(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU],Joueur** j)
	*\brief Initialise une manche.
	*\details Permet de réinitialiser le plateau de jeu et une liste de pièce d'un Joueur.
	*\param pl Plateau de jeu à vider.
	*\param j Pointeur sur une liste de Joueur afin de réinitialiser la liste de pièce de chaque Joueur.
*/

/* Permets de réinitialisé le plateau de jeu et une liste de piece d'un Joueur.*/

void initialisation_manche(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU],Joueur** j){
	int i,x;
	/* Remet les valeurs du tableau à 0*/
	for(i=0;i < TAILLE_PLATEAU;i++){

		for(x=0;x < TAILLE_PLATEAU;x++){
			pl[i][x]=VIDE;
		}
	}

	joueur_liste_reinit(*j);
	/*Vue que le Blokus commence par le joueur bleu, on remets le pointeur sur le joueur bleu*/
	while(joueur_couleur(*j) != BLEU)
		*j=joueur_suivant(*j);


}

/**
	*\fn void maj_scores(Joueur** j)
	*\details Permet de mettre à jour les scores à la fin de la partie: <br>
	* +15 si le Joueur a posé toutes ses Pieces. <br>
	* -1  pour chaque Carre d'une Piece.
	*\param j Prend une liste de Joueur.
*/

/*Permets de mettre à jour les scores à la fin de la partie:*/

void maj_scores(Joueur** j) {

    // On garde l'adresse du premier joueur pour
    // savoir quand arrêter

    Joueur * pivot = *j;
    do {
	/* Si le joueur n'à plus de piece */
        if (joueur_liste_piece(*j) == NULL) {
            (*j)->score += 15;
        }
	/*Alors on enleve -1 pour chaque petit carrée */
	 else {
		Piece * p=joueur_liste_piece(*j);
		Piece * pivot=p;
		do{
			(*j)->score -= piece_nb_carre(p);
			p=piece_suivant(p);
		} while(p != pivot);

        }

        *j = joueur_suivant(*j);

    } while ((*j) != pivot);
}


/**
	*\fn int joueur_abandon(Joueur* j)
	*\brief Vérifie si tous les joueurs ont abandonné.
	*\param j Reçoit la liste des joueurs et vérifie la valeur d'abandon de chaque Joueur.
	\return Renvoie 1 si tous les joueurs ont abandonné, sinon 0.

*/


/*Vérifie si tous les joueurs ont abandonné.*/

int joueur_abandon(Joueur* j){
	Joueur* pivot;
	pivot=j;
	j=joueur_suivant(j);
	/*Boucle pour savoir si tous les joueurs ont abandonne*/
	while(pivot != j && joueur_a_abandonne(j)){
		j=joueur_suivant(j);

	}

	return (pivot == j && joueur_a_abandonne(j));
}
