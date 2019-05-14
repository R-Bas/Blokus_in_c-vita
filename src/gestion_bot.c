#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <SDL2/SDL.h>

#include <gestion_bot.h>
#include <gestion_tour.h>
#include <gestion_tour_sdl.h>
#include <couleur.h>
#include <joueur.h>
#include <commun.h>
#include <affichage_sdl.h>

extern SDL_Renderer* renderer;

/* Accesseurs */
/**
	\fn Piece* coup_piece (Coup* coup);
	\brief Permet de récupérer la copie de la Piece du Coup

	\param coup Le Coup du bot
	\return La copie de la Piece
*/
Piece* coup_piece (Coup* coup) {
	if (coup != NULL)
		return coup->piece_copie;
	return NULL;
}

/**
	\fn Piece* coup_piece_origine (Coup* coup);
	\brief Permet de récupérer le pointeur de la Piece d'origine du Coup

	\param coup Le Coup du bot
	\return La Piece d'origine (pointeur dans la liste du Joueur)
*/
Piece* coup_piece_origine (Coup* coup) {
	if (coup != NULL)
		return coup->piece_origine;
	return NULL;
}

/**
	\fn Couleur coup_couleur (Coup* coup);
	\brief Permet de récupérer la Couleur de la Piece (correspondant à la Couleur du Joueur)

	\param coup Le Coup du bot
	\return La Couleur de la Piece
*/
Couleur coup_couleur (Coup* coup) {
	return coup->c;
}

/**
	\fn int coup_coord_x (Coup* coup);
	\brief Permet de récupérer la coordonnée x du Coup

	\param coup Le Coup du bot
	\return La coordonnée x de la Piece
*/
int coup_coord_x (Coup* coup) {
	return coup->x;
}

/**
	\fn int coup_coord_y (Coup* coup);
	\brief Permet de récupérer la coordonnée y du Coup

	\param coup Le Coup du bot
	\return La coordonnée y de la Piece
*/
int coup_coord_y (Coup* coup) {
	return coup->y;
}

/**
	\fn int coup_valeur (Coup* coup);
	\brief Permet de récupérer la valeur du Coup

	La valeur est une suite d'évaluation sur l'emplacement de la Piece, le nombre de coins débloqués pour soi et le nombre de coins bloqués adverses.

	\param coup Le Coup du bot
	\return La valeur du Coup
*/
int coup_valeur (Coup* coup) {
	return coup->valeur_coup;
}

/**
	\fn void coup_detruire (Coup** coup);
	\brief Supprime un Coup (et la copie de la Piece d'origine)

	\param coup L'adresse du Coup du bot
*/
void coup_detruire(Coup** coup) {
	if ((*coup) != NULL) {
		if ((*coup)->piece_copie != NULL)
			liste_piece_detruire(&((*coup)->piece_copie));

		free(*coup);
	}

	*coup = NULL;
}

/**
	\fn Coup* coup_copie (Coup* coup);
	\brief Copie un Coup et ses attributs

	\param coup Le Coup du bot
	\return La copie du Coup
*/
Coup* coup_copie(Coup* coup) {
	if (coup == NULL)
		return NULL;

	Coup* copie = malloc(sizeof(*copie));

	/* Copie la Piece (car elle peut être modifiée plus tard. On ne veut pas) */
	copie->piece_copie = piece_copie(coup_piece(coup));

	/* Enregistrement de son adresse pour la supprimer si on la pose */
	copie->piece_origine = coup_piece_origine(coup);
	copie->c = coup_couleur(coup);
	copie->x = coup_coord_x(coup);
	copie->y = coup_coord_y(coup);
	copie->valeur_coup = coup_valeur(coup);

	return copie;
}

/**
	\fn void coup_afficher (Coup* coup);
	\brief Permet d'afficher un Coup et tous ses attributs

	\param coup Le Coup du bot
*/
void coup_afficher(Coup* coup) {
	printf("Couleur: %s\nX: %d, Y: %d\n", couleur_tostring(coup_couleur(coup)), coup_coord_x(coup), coup_coord_y(coup));
	printf("Valeur: %d\n", coup_valeur(coup));

	printf("Piece copie\n");
	carre_afficher(piece_liste_carre(coup_piece(coup)));
	printf("Piece origine\n");
	carre_afficher(piece_liste_carre(coup_piece_origine(coup)));
}

static int poser_piece_bot(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Coup* coup) {
    if(!piece_hors_liste(coup_piece(coup)))
    {
		int x = coup_coord_x(coup);
		int y = coup_coord_y(coup);

        Carre* c = piece_liste_carre(coup_piece(coup));

	/* Pose la Piece sur le plateau aux coordonnées saisies */
        do
        {
            pl[x+carre_get_x(c)][y+carre_get_y(c)] = coup_couleur(coup);
            c = carre_get_suiv(c);
        } while(c != piece_liste_carre(coup_piece(coup)));
    }

	//afficher_plateau(pl);

	return 0;
}

/* Retourne l'indice du meilleur Coup dans le tableau */
static int meilleur_coup(Coup** tab, int compteur) {
	/* S'il n'y a qu'un seul coup possible */
	if (compteur == 1) {
//		fprintf(stderr, "un seul coup possible\n");
		return 0;
	}

	int index_max = 0; /* L'index dans le tableau du meilleur Coup */
	int i = 0;
	int compteur_tab = 0; /* Le nombre de "meilleur coup" */
	int tab_index[compteur]; /* Le tableau contenant les "meilleurs coups" */

	/* Pour tous les Coup */
	for (i = 0; i < compteur; i++)
		/* Si on trouve un Coup avec une meilleure valeur */
		if (coup_valeur(tab[i]) > coup_valeur(tab[index_max])) {
//fprintf(stderr, "meilleur coup trouvé: %d\n", coup_valeur(tab[i]));
			/* "Réinitialisation" du tableau */
			compteur_tab = 0;
			index_max = i;
		}
		/* Si on trouve un Coup équivalent au meilleur Coup */
		else if (coup_valeur(tab[i]) == coup_valeur(tab[index_max]))
			tab_index[compteur_tab++] = i;


	int random = 0;

	/* S'il y a plus d'un meilleur coup (== compteur_tab n'est pas à 0) */
	if (compteur_tab) {
		srand(time(NULL));
		random = rand() % compteur_tab;
//	fprintf(stderr, "%d valeurs possibles, rand = %d\n", compteur_tab, random);
	}
	return tab_index[random];
}

static int coord_dans_plateau(int coord)
{
    return (coord >= 0 && coord < TAILLE_PLATEAU);
}

static int nb_coups_dispo_2(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Joueur* joueur) {
	/* Si le Joueur a abandonné, il n'a théoriquement plus de coin dispo */
	if (joueur_a_abandonne(joueur))
		return 0;

	int i, j, k;

    Piece * p = joueur_liste_piece(joueur);
    Piece * init = p;

    int compteur = 0;

    /* Pour chaque position de la matrice */
    for(i = 0; i < TAILLE_PLATEAU; i++)
    {
        for(j = 0; j < TAILLE_PLATEAU; j++)
        {
            /* Pour chaque pièces disponibles */
            do
            {
                /* Pour chaque orientation possible */
                for(k = 0; k < 4; k++)
                {
                    /* Si la pièce est posable */
                    if(verifier_coordonnees(pl, p, i, j, joueur))
			compteur++;

                    changer_orientation(p);
                }

                p = piece_suivant(p);

            } while (p != init);
        }
    }

	/* On retourne le nombre de Coup qu'il peut jouer */
    return compteur;
}

static int verif_coin_libre(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], int x, int y, Couleur col) {
	/* Vérifie qu'il n'y a aucun Carre adjacant aux Carre que le Joueur pose */
	if ((coord_dans_plateau(x - 1) && coord_dans_plateau(y) && pl[x - 1][y] == col) || /* A gauche */
		(coord_dans_plateau(x + 1) && coord_dans_plateau(y) && pl[x + 1][y] == col) || /* A droite */
		(coord_dans_plateau(x) && coord_dans_plateau(y - 1) && pl[x][y - 1] == col) || /* En haut */
		(coord_dans_plateau(x) && coord_dans_plateau(y + 1) && pl[x][y + 1] == col)) /* En bas */
		return 0;
	else
		return 1;
}

static int nb_coups_dispo(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Coup* coup, Joueur* joueur) {
	Couleur couleur = joueur_couleur(joueur);

	Carre* c = piece_liste_carre(coup_piece(coup));
	Carre* init = c;
	int coord_x = coup_coord_x(coup), coord_y = coup_coord_y(coup);
	int x, y;
	int nb = 0;
//int cmp = 1;
	/* Pour tous les Carre de la Piece */
	do {
		x = coord_x + carre_get_x(c);
		y = coord_y + carre_get_y(c);

		/* Vérifie pour TOUTES les diagonales de CHAQUE Carre, si le coin est libre pour nous */
		if ((coord_dans_plateau(x - 1) && coord_dans_plateau(y - 1) && pl[x - 1][y  - 1] == VIDE)) /* Diagonale Bas - Gauche */
			if (verif_coin_libre(pl, x - 1, y - 1, couleur)) {
//				fprintf(stderr, "Coin libre en %d %d - Diag bas gauche\n", x - 1, y - 1);
				nb++;
			}

		if ((coord_dans_plateau(x + 1) && coord_dans_plateau(y - 1) && pl[x + 1][y - 1] == VIDE)) /* Diagonale Bas - Droit */
			if (verif_coin_libre(pl, x + 1, y - 1, couleur)) {
//				fprintf(stderr, "Coin liber en %d %d - Diag bas droit\n", x + 1, y - 1);

				nb++;
}
		if ((coord_dans_plateau(x - 1) && coord_dans_plateau(y + 1) && pl[x - 1][y + 1] == VIDE)) /* Diagonale Haut - Gauche */
			if (verif_coin_libre(pl, x - 1, y + 1, couleur)) {
//				fprintf(stderr, "Coin liber en %d %d - Diag haut gauche\n", x - 1, y + 1);
				nb++;
}
		if ((coord_dans_plateau(x + 1) && coord_dans_plateau(y + 1) && pl[x + 1][y + 1] == VIDE)) /* Diagonale Haut - Droit */
			if (verif_coin_libre(pl, x + 1, y + 1, couleur)){
//				fprintf(stderr, "Coin liber en %d %d - Diag haut droit\n", x + 1, y + 1);
				nb++;
}
//fprintf(stderr, "compteur: %d\n", cmp++);
	} while ((c = carre_get_suiv(c)) != init);

	return nb;
}

/**
	\fn int eval_coup_bot(Coup* coup);
	\brief Evalue un Coup selon s'il se débloque des coins, s'il bloque des coins adverses et combien de Carre il pose

	Contient également des coefficients pour chaque évaluation

	\param coup Le Coup du bot
	\return La valeur du Coup
*/
int eval_coup_bot(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Coup* coup, Joueur* bot) {
/*if (piece_id(coup_piece_origine(coup)) == 1) {
	return -100;
}*/

	Couleur pl2[TAILLE_PLATEAU][TAILLE_PLATEAU];

	int i, j;
	float eval = 0;
//	int nb_coin_bot = 0, nb_coin_adversaire = 0;

	/* Recopie du plateau */
	for (i = 0; i < TAILLE_PLATEAU; i++)
		for (j = 0; j < TAILLE_PLATEAU; j++)
			pl2[i][j] = pl[i][j];

	/* Calcul de tous les coins disponibles pour le Joueur actuel et les autres avant la pose de la Piece */
//	nb_coin_bot = nb_coups_dispo(pl2, coup, bot);

	poser_piece_bot(pl2, coup);

	/* Evalue le nombre de cases disponibles (== VIDE) autour de la nouvelle Piece posée */
	eval += eval_nb_carres_poses(coup) * COEF_CARRES_POSES;

	if (joueur_nb_piece_restantes(bot) > NB_PIECES - 4) {
		eval += eval_emplacement_piece(coup) * COEF_EMPLACEMENT_PIECE;
		eval += eval_nb_nouveaux_coups(pl2, coup, bot);
	}
	else {
		eval += eval_emplacement_piece(coup) * COEF_EMPLACEMENT_PIECE / 4;

	/* Evalue le nombre de cases disponibles (== VIDE) autour de la nouvelle Piece posée */
//	eval += eval_cases_dispo(pl2, bot, nb_coin_bot) * COEF_CASES_DISPO;
		Joueur* tmp = bot;

		while ((tmp = joueur_suivant(tmp)) != bot) {
	//		nb_coin_adversaire += nb_coups_dispo(pl2, tmp);

			eval += eval_nb_coups_bloques(pl2, coup) * COEF_COINS_BLOQUES;
		}

		eval += eval_nb_nouveaux_coups(pl2, coup, bot) * COEF_NOUVEAUX_COINS;
	}
//	eval += eval_nb_nouveaux_coups(pl2, bot, nb_coin_bot) * COEF_NOUVEAUX_COINS;

	/* Si la Piece est le petit carré, prend un malus (car on veut le garder pour la fin) */
	if (piece_id(coup_piece_origine(coup)) == 1) {
		//fprintf(stderr, "Petit carré posé\n");
		eval /= 10;
	}

	return eval;

}

/**
	\fn int eval_nb_carres_poses(Coup* coup);
	\brief Calcule le nombre de Carre de la Piece

	\param coup Le Coup à évaluer
	\return Le nombre de Carre de la Piece du Coup joué
*/
int eval_nb_carres_poses(Coup* coup) {
	return piece_nb_carre(coup_piece(coup));
}

/**
	\fn int eval_emplacement_piece (Coup* coup);
	\brief Calcule si l'emplacement de la Piece est plus ou moins bon

	\param coup Le Coup du bot
	\return La valeur de l'emplacement du Coup
*/
int eval_emplacement_piece(Coup* coup) {
	int coord_x = coup_coord_x(coup);
	int coord_y = coup_coord_y(coup);
	int mauvais = 0, moyen = 0, bon = 0, centre = 0; /* Regarde chaque Carre et l'évalue */
	Carre* init = piece_liste_carre(coup_piece(coup));
	Carre* c = init;
	int x = 0, y = 0;

	/* Regarde tous les Carre et leur position */
	do {
		/* Récupère les coordonnées du Carre sur le plateau */
		x = coord_x + carre_get_x(c);
		y = coord_y + carre_get_y(c);

		if (x <= COUP_MAUVAIS || x >= TAILLE_PLATEAU - COUP_MAUVAIS || y <= COUP_MAUVAIS || y >= TAILLE_PLATEAU - COUP_MAUVAIS)
			mauvais++;
		else if (x <= COUP_MOYEN || x >= TAILLE_PLATEAU - COUP_MOYEN || y <= COUP_MOYEN || y >= TAILLE_PLATEAU - COUP_MOYEN)
			moyen++;
		else if (x <= COUP_BON || x >= TAILLE_PLATEAU - COUP_BON || y <= COUP_BON || y >= TAILLE_PLATEAU - COUP_BON)
			bon++;
		else
			centre++;

	} while ((c = carre_get_suiv(c)) != init);

	return (bon * COEF_COUP_BON + moyen * COEF_COUP_MOYEN + mauvais * COEF_COUP_MAUVAIS + centre * COEF_COUP_CENTRE);
}

/**
	\fn int eval_nb_nouveaux_coups(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Coup* coup, Joueur* joueur);
	\brief Calcule le nombre de nouveaux coins disponibles

	\param pl Le plateau "fictif" où l'on peut poser les Piece sans crainte
	\param coup Le Coup du bot
	\param Le Joueur qui pose la Piece
	\return Le nombre de nouveaux coins s'il pose la Piece du Coup
*/
int eval_nb_nouveaux_coups(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Coup* coup, Joueur* joueur) {
	/* Calcul combien il y a de coins disponibles autour de chaque Carre */
	int nb = nb_coups_dispo(pl, coup, joueur);

	return nb;
}

/**
	\fn int eval_nb_coups_bloques(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Coup* coup);
	\brief Calcule si l'emplacement de la Piece est plus ou moins bon

	\param pl Le plateau "fictif" où l'on peut poser les Piece sans crainte
	\param coup Le Coup du bot
	\return La valeur de l'emplacement du Coup
*/
int eval_nb_coups_bloques(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Coup* coup) {
	int nb = 0;

	Piece* p = coup_piece(coup);

	Carre* c = piece_liste_carre(p);
	Carre* c2 = c;

	do
	{
		c = carre_get_suiv(c);

		if(coup_coord_x(coup) + carre_get_x(c) < 19 && coup_coord_y(coup) + carre_get_y(c) < 19)
		{
			if(pl[coup_coord_x(coup) + carre_get_x(c) + 1][coup_coord_y(coup) + carre_get_y(c) + 1] != VIDE && pl[coup_coord_x(coup) + carre_get_x(c) + 1][coup_coord_y(coup) + carre_get_y(c) + 1] != coup_couleur(coup))
			{
				Couleur coul = pl[coup_coord_x(coup) + carre_get_x(c) + 1][coup_coord_y(coup) + carre_get_y(c) + 1];

				if(pl[coup_coord_x(coup) + carre_get_x(c) + 1][coup_coord_y(coup) + carre_get_y(c)] != coul && pl[coup_coord_x(coup) + carre_get_x(c)][coup_coord_y(coup) + carre_get_y(c) + 1] != coul &&
				pl[coup_coord_x(coup) + carre_get_x(c) - 1][coup_coord_y(coup) + carre_get_y(c)] != coul && pl[coup_coord_x(coup) + carre_get_x(c)][coup_coord_y(coup) + carre_get_y(c) - 1] != coul)
					nb++;
			}
		}

		if(coup_coord_x(coup) + carre_get_x(c) > 0 && coup_coord_y(coup) + carre_get_y(c) < 19)
		{
			if(pl[coup_coord_x(coup) + carre_get_x(c) - 1][coup_coord_y(coup) + carre_get_y(c) + 1] != VIDE && pl[coup_coord_x(coup) + carre_get_x(c) - 1][coup_coord_y(coup) + carre_get_y(c) + 1] != coup_couleur(coup))
			{
				Couleur coul = pl[coup_coord_x(coup) + carre_get_x(c) - 1][coup_coord_y(coup) + carre_get_y(c) + 1];

				if(pl[coup_coord_x(coup) + carre_get_x(c) + 1][coup_coord_y(coup) + carre_get_y(c)] != coul && pl[coup_coord_x(coup) + carre_get_x(c)][coup_coord_y(coup) + carre_get_y(c) + 1] != coul &&
				pl[coup_coord_x(coup) + carre_get_x(c) - 1][coup_coord_y(coup) + carre_get_y(c)] != coul && pl[coup_coord_x(coup) + carre_get_x(c)][coup_coord_y(coup) + carre_get_y(c) - 1] != coul)
					nb++;
			}
		}

		if(coup_coord_x(coup) + carre_get_x(c) < 19 && coup_coord_y(coup) + carre_get_y(c) > 0)
		{
			if(pl[coup_coord_x(coup) + carre_get_x(c) + 1][coup_coord_y(coup) + carre_get_y(c) - 1] != VIDE && pl[coup_coord_x(coup) + carre_get_x(c) + 1][coup_coord_y(coup) + carre_get_y(c) - 1] != coup_couleur(coup))
			{
				Couleur coul = pl[coup_coord_x(coup) + carre_get_x(c) + 1][coup_coord_y(coup) + carre_get_y(c) - 1];

				if(pl[coup_coord_x(coup) + carre_get_x(c) + 1][coup_coord_y(coup) + carre_get_y(c)] != coul && pl[coup_coord_x(coup) + carre_get_x(c)][coup_coord_y(coup) + carre_get_y(c) + 1] != coul &&
				pl[coup_coord_x(coup) + carre_get_x(c) - 1][coup_coord_y(coup) + carre_get_y(c)] != coul && pl[coup_coord_x(coup) + carre_get_x(c)][coup_coord_y(coup) + carre_get_y(c) - 1] != coul)
					nb++;
			}
		}

		if(coup_coord_x(coup) + carre_get_x(c) > 0 && coup_coord_y(coup) + carre_get_y(c) > 0)
		{
			if(pl[coup_coord_x(coup) + carre_get_x(c) - 1][coup_coord_y(coup) + carre_get_y(c) - 1] != VIDE && pl[coup_coord_x(coup) + carre_get_x(c) - 1][coup_coord_y(coup) + carre_get_y(c) - 1] != coup_couleur(coup))
			{
				Couleur coul = pl[coup_coord_x(coup) + carre_get_x(c) - 1][coup_coord_y(coup) + carre_get_y(c) - 1];

				if(pl[coup_coord_x(coup) + carre_get_x(c) + 1][coup_coord_y(coup) + carre_get_y(c)] != coul && pl[coup_coord_x(coup) + carre_get_x(c)][coup_coord_y(coup) + carre_get_y(c) + 1] != coul &&
				pl[coup_coord_x(coup) + carre_get_x(c) - 1][coup_coord_y(coup) + carre_get_y(c)] != coul && pl[coup_coord_x(coup) + carre_get_x(c)][coup_coord_y(coup) + carre_get_y(c) - 1] != coul)
					nb++;
			}
		}

	} while(c != c2);

	/* Retourne le nombre de cases bloquées */
	return nb;
}

/**
	\fn int gestion_tour_bot(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Joueur* bot);
	\brief Permet à un bot de jouer son tour

	Calcule tous les Coup possibles, les évalues et renvoie le potentiel meilleur Coup

	\param pl Le plateau dans lequel la Piece sera posée
	\param coup Le Coup du bot
	\return L'id de la Piece posée en négatif. Si aucune Piece posée, retourne 1 (abandon).
*/
int gestion_tour_bot(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Joueur* bot) {
	int retour = 0;

	/* MAJ de l'interface graphique */
    Reserves* r = init_afficher_pieces_dispo_sdl(bot);
    SDL_RenderClear(renderer);

    afficher_plateau_sdl(pl);

    afficher_pieces_dispo_sdl(r, bot, NULL);

    afficher_scores_sdl(bot);

    afficher_tour_sdl(bot);

    SDL_RenderPresent(renderer);

    free_afficher_pieces_dispo_sdl(&r);

	Coup* c = bot_jouer_tour(pl, bot);

	/* Si aucun Coup n'a été trouvé */
	if (c == NULL)
		return 1;

	retour = -piece_id(coup_piece_origine(c));

	/* Permet de tourner la Piece originale dans la même direction que la copie pour la poser */
	while (!piece_meme_orientation(coup_piece(c), coup_piece_origine(c)))
		piece_pivoter(1, piece_liste_carre(coup_piece_origine(c)));

	/* Pose la Piece et la supprime de la liste du Joueur */
	poser_piece_sdl(pl, coup_piece_origine(c), bot, coup_coord_x(c), coup_coord_y(c));

	/* Suppression du Coup qu'on a joué */
	coup_detruire(&c);

	/* Attente après le tour d'un bot */
	//sleep(TEMPS_ATTENTE_BOT);

	return retour;
}

static void free_tab_coup(Coup*** tab, int taille)
{
    int i;

    if (*tab != NULL) {
        for(i = taille - 1; i >= 0; i--)
		{
            coup_detruire(&((*tab)[i]));
        }

        free(*tab);
    }

    *tab = NULL;
}

/**
	\fn int bot_jouer(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Joueur* bot, int profondeur);
	\brief Fonction min-max où le bot joue récursivement

	\param pl Plateau "fictif" permettant de jouer et d'évaluer les Coup
	\param coup Le Coup du bot
	\param profondeur La profondeur d'évaluation de la fonction récursive min-max
	\return La valeur récursive du meilleur Coup.
*/
int bot_jouer(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Joueur* bot, int profondeur)
{
	if (joueur_a_abandonne(bot))
		return -1;

	int i, j, k, nb;

    srand(time(NULL));

    Piece * p = joueur_liste_piece(bot);
    Piece * init = p;

    Coup** tab = NULL;
    int compteur = 0;

    /* Pour chaque position de la matrice */
    for(i = 0; i < TAILLE_PLATEAU; i++)
    {
        for(j = 0; j < TAILLE_PLATEAU; j++)
        {
            /* Pour chaque pièces disponibles */
            do
            {
                /* Pour chaque orientation possible */
                for(k = 0; k < 4; k++)
                {
                    /* Si la pièce est posable */
                    if(verifier_coordonnees(pl, p, i, j, bot))
                    {
                        /* On enregistre le coup puis on estime sa valeur */
                        if (tab == NULL) {
                            tab = malloc(sizeof(*tab));
						}
                        else
                            tab = realloc(tab, sizeof(*tab) * (compteur + 1));


                        tab[compteur] = malloc(sizeof(*tab[compteur]));

                        tab[compteur]->piece_origine = p;

                        tab[compteur]->piece_copie = piece_copie(p);

						/* Affecte le Coup dans le tableau */
                        tab[compteur]->x = i;
                        tab[compteur]->y = j;

						tab[compteur]->c = joueur_couleur(bot);
						/*tab[compteur]->valeur_coup = 0;*/

						//if(!profondeur || tab[compteur]->piece_origine == tab[compteur]->piece_origine->suiv)
						tab[compteur]->valeur_coup = eval_coup_bot(pl, tab[compteur], bot);

						if (profondeur > 0 && (tab[compteur]->piece_origine != tab[compteur]->piece_origine->suiv))
						{
							/* Enlève la Piece actuelle de la liste temporairement */
							tab[compteur]->piece_origine->prec->suiv = tab[compteur]->piece_origine->suiv;
							bot->liste_piece = piece_suivant(p);

							/* Recréé un plateau fictif pour émuler les coups */
							Couleur pl2[TAILLE_PLATEAU][TAILLE_PLATEAU];

							int i, j;

							/* Recopie du plateau */
							for (i = 0; i < TAILLE_PLATEAU; i++)
								for (j = 0; j < TAILLE_PLATEAU; j++)
									pl2[i][j] = pl[i][j];

							/* Pose la Piece et NE la supprime PAS de la liste du Joueur */
							poser_piece_bot(pl2, tab[compteur]);

							tab[compteur]->valeur_coup += adversaire_jouer(pl2, bot, joueur_suivant(bot), profondeur);
							//fprintf(stderr, "Profondeur: %d, nb_coup jouables: %d, valeur du coup: %d\n", profondeur, compteur, tab[compteur]->valeur_coup);

							/* Remet la Piece dans la liste */
							tab[compteur]->piece_origine->prec->suiv = tab[compteur]->piece_origine;
							bot->liste_piece = p;
						}

						compteur++;
                    }
                    changer_orientation(p);
                }

                p = piece_suivant(p);

            } while (p != init);
        }
    }

	int val_coup = -1;

    if (compteur && tab != NULL) {
		nb = meilleur_coup(tab, compteur);

		val_coup = coup_valeur(tab[nb]);
		//fprintf(stderr, "Recursion terminée\n\tProfondeur: %d, nb_coup jouables: %d, valeur du coup: %d\n", profondeur, compteur, val_coup);

    }

    free_tab_coup(&tab, compteur);

    /* On retourne le coup estimé comme étant le meilleur. NULL si aucun coup n'est possible */
    return val_coup;
}

/**
	\fn int adversaire_jouer(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Joueur* bot, Joueur* joueur, int profondeur);
	\brief Fonction min-max où un adversaire joue récursivement

	\param pl Plateau "fictif" permettant de jouer et d'évaluer les Coup
	\param bot Le bot pour qui on doit évaluer les Coup
	\param joueur L'adversaire qui joue
	\param profondeur La profondeur d'évaluation de la fonction récursive min-max
	\return La valeur récursive du meilleur Coup.
*/
int adversaire_jouer(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Joueur* bot, Joueur* joueur, int profondeur)
{
	/* Si l'adversaire a abandonné OU s'il n'a plus de Piece */
	if (joueur_a_abandonne(joueur) || joueur_nb_piece_restantes(joueur) == 0) {
		if(joueur_suivant(joueur) == bot)
			return bot_jouer(pl, bot, profondeur - 1);
		else
			return adversaire_jouer(pl, bot, joueur_suivant(joueur), profondeur);
	}

	int i, j, k, nb;

    srand(time(NULL));

    Piece * p = joueur_liste_piece(joueur);
    Piece * init = p;

    Coup** tab = NULL;
    int compteur = 0;

    /* Pour chaque position de la matrice */
    for(i = 0; i < TAILLE_PLATEAU; i++)
    {
        for(j = 0; j < TAILLE_PLATEAU; j++)
        {
            /* Pour chaque pièces disponibles */
            do
            {
                /* Pour chaque orientation possible */
                for(k = 0; k < 4; k++)
                {
                    /* Si la pièce est posable */
                    if(verifier_coordonnees(pl, p, i, j, joueur))
                    {
                        /* On enregistre le coup puis on estime sa valeur */
                        if (tab == NULL) {
                            tab = malloc(sizeof(*tab));
						}
                        else
                            tab = realloc(tab, sizeof(*tab) * (compteur + 1));


                        tab[compteur] = malloc(sizeof(*tab[compteur]));

                        tab[compteur]->piece_origine = p;

                        tab[compteur]->piece_copie = piece_copie(p);

						/* Affecte le Coup dans le tableau */
                        tab[compteur]->x = i;
                        tab[compteur]->y = j;

						tab[compteur]->c = joueur_couleur(joueur);
						/*tab[compteur]->valeur_coup = 0;*/

						/* Enlève la Piece actuelle de la liste temporairement */
						tab[compteur]->piece_origine->prec->suiv = tab[compteur]->piece_origine->suiv;
						joueur->liste_piece = piece_suivant(p);

						tab[compteur]->valeur_coup = eval_coup_bot(pl, tab[compteur], joueur);

						/* Remet la Piece dans la liste */
						tab[compteur]->piece_origine->prec->suiv = tab[compteur]->piece_origine;
						joueur->liste_piece = p;

						compteur++;
                    }
                    changer_orientation(p);
                }

                p = piece_suivant(p);

            } while (p != init);
        }
    }

	Coup* coup = NULL;

	int val_coup = -1;

    if (compteur && tab != NULL) {
		nb = meilleur_coup(tab, compteur);

		coup = coup_copie(tab[nb]);

		/* Recréé un plateau fictif pour émuler les coups */
		Couleur pl2[TAILLE_PLATEAU][TAILLE_PLATEAU];

		int i, j;

		/* Recopie du plateau */
		for (i = 0; i < TAILLE_PLATEAU; i++)
			for (j = 0; j < TAILLE_PLATEAU; j++)
				pl2[i][j] = pl[i][j];

		/* Pose la Piece et NE la supprime PAS de la liste du Joueur */
		poser_piece_bot(pl2, coup);

		/* Si le prochain Joueur est le bot */
		if(joueur_suivant(joueur) == bot) {
			/* Retourne la valeur du Coup suivant du bot - la valeur du Coup de l'adversaire actuel */
			val_coup = bot_jouer(pl2, bot, profondeur - 1) - coup_valeur(coup);
//			fprintf(stderr, "Coup adverse: %d\ncoup du bot: %d\n", coup_valeur(coup), val_coup + coup_valeur(coup));
		}
		else
			/* Retourne la valeur du Coup suivant de l'adversaire suivant - la valeur du Coup de l'adversaire actuel */
			val_coup = adversaire_jouer(pl2, bot, joueur_suivant(joueur), profondeur) - coup_valeur(coup);

		/* Suppression du Coup qu'on a joué */
		coup_detruire(&coup);
    }

    free_tab_coup(&tab, compteur);

    /* On retourne le coup estimé comme étant le meilleur. NULL si aucun coup n'est possible */
    return val_coup;
}

/* Gestion du premier tour du bot */
/**
	\fn Coup* bot_jouer_tour(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Joueur* bot);
	\brief Premier tour du bot

	Si le bot arrive vers la fin du jeu, on calcule une valeur supplémentaire avec la fonction min-max

	\param pl Le plateau de jeu actuel
	\param bot Le bot qui doit jouer son tour

	\return Le meilleur Coup évalué
*/
Coup* bot_jouer_tour(Couleur pl[TAILLE_PLATEAU][TAILLE_PLATEAU], Joueur* bot)
{
    int i, j, k, nb;

    srand(time(NULL));

    Piece * p = joueur_liste_piece(bot);
    Piece * init = p;

    Coup** tab = NULL;
    int compteur = 0;

    /* Pour chaque position de la matrice */
    for(i = 0; i < TAILLE_PLATEAU; i++)
    {
        for(j = 0; j < TAILLE_PLATEAU; j++)
        {
            /* Pour chaque pièces disponibles */
            do
            {
                /* Pour chaque orientation possible */
                for(k = 0; k < 4; k++)
                {
                    /* Si la pièce est posable */
                    if(verifier_coordonnees(pl, p, i, j, bot))
                    {
                        /* On enregistre le coup puis on estime sa valeur */
                        if (tab == NULL) {
                            tab = malloc(sizeof(*tab));
						}
                        else
                            tab = realloc(tab, sizeof(*tab) * (compteur + 1));


                        tab[compteur] = malloc(sizeof(*tab[compteur]));

                        tab[compteur]->piece_origine = p;

                        tab[compteur]->piece_copie = piece_copie(p);

						/* Enlève la Piece actuelle de la liste temporairement */
						tab[compteur]->piece_origine->prec->suiv = tab[compteur]->piece_origine->suiv;
						bot->liste_piece = piece_suivant(p);

						/* Affecte le Coup dans le tableau */
                        tab[compteur]->x = i;
                        tab[compteur]->y = j;

						tab[compteur]->c = joueur_couleur(bot);

						/* Recréé un plateau fictif pour émuler les coups */
						Couleur pl2[TAILLE_PLATEAU][TAILLE_PLATEAU];

						int i, j;

						/* Recopie du plateau */
						for (i = 0; i < TAILLE_PLATEAU; i++)
							for (j = 0; j < TAILLE_PLATEAU; j++)
								pl2[i][j] = pl[i][j];

						/* Pose la Piece et NE la supprime PAS de la liste du Joueur */
						poser_piece_bot(pl2, tab[compteur]);

						/*tab[compteur]->valeur_coup = 0;*/
						tab[compteur]->valeur_coup = eval_coup_bot(pl, tab[compteur], bot);

						if (PROFONDEUR > 0)
                        	tab[compteur]->valeur_coup += adversaire_jouer(pl2, bot, joueur_suivant(bot), PROFONDEUR);
/*
						if (joueur_nb_piece_restantes(bot) <= 5 && PROFONDEUR == 0)
							tab[compteur]->valeur_coup += adversaire_jouer(pl2, bot, joueur_suivant(bot), 2 - joueur_nb_piece_restantes(bot) / 2);
*/

						/* Remet la Piece dans la liste */
						tab[compteur]->piece_origine->prec->suiv = tab[compteur]->piece_origine;
						bot->liste_piece = p;

						compteur++;
                    }
                    changer_orientation(p);
                }

                p = piece_suivant(p);

            } while (p != init);
        }
    }

    Coup* coup = NULL;

    if (compteur && tab != NULL) {
		nb = meilleur_coup(tab, compteur);

		/*nb = rand() % compteur;*/
        coup = coup_copie(tab[nb]);

/*	if (piece_id(coup_piece_origine(coup)) == 1) {
		fprintf(stderr, "pose petit carre\nvaleur: %d\nindex:%d\n", coup_valeur(coup), nb);
		int i;
		for (i = 0; i < compteur; i++) {
			fprintf(stderr, "indice: %d\n", i);
			coup_afficher(tab[i]);
		}
	}
//fprintf(stderr, "valeur: %d\n", coup_valeur(coup));
*/    }

    free_tab_coup(&tab, compteur);

    /* On retourne le coup estimé comme étant le meilleur. NULL si aucun coup n'est possible */
    return coup;
}
