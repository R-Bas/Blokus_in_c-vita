#include <stdio.h>
#include <stdlib.h>

#include <gestion_tour.h>
#include <carre.h>
#include <commun.h>


/**
 * \fn void initialiser_matrice(int matrice[5][5])
 * \brief Fonction d'initialisation d'une matrice
 * \details Fonction initialisant une matrice de taille 5*5 en la remplissant de 0
 * \param matrice La matrice à initialiser
 */
void initialiser_matrice(int matrice[5][5])
{
    int i, j;

    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < 5; j++)
            matrice[i][j] = 0;
    }
}

/* Fonction affectant une pièce à une matrice
     remplit la matrice de 1 en fonction des coordonnées des carres constituant la pièce */

/**
 * \fn void affecter_matrice(int matrice[5][5], Carre* c)
 * \brief Fonction d'initialisation d'une matrice
 * \details Fonction affectant une pièce à une matrice en remplissant la matrice de 1 en fonction des
    coordonnées des carres constituant la pièce
 * \param matrice La matrice à laquelle affecter la pièce
 * \param c Le premier carre de la pièce à affecter
 */
void affecter_matrice(int matrice[5][5], Carre* c)
{
    Carre* c2 = c;
    c2 = carre_get_suiv(c2);

    while(c2 != c)
    {
        matrice[carre_get_x(c2)][carre_get_y(c2)] = 1;
        c2 = carre_get_suiv(c2);
    }

    matrice[carre_get_x(c2)][carre_get_y(c2)] = 1;
}

/* Fonction permettant d'afficher une matrice 5*5

void afficher_matrice(int matrice[5][5])
{
    int i, j;

    for(i = 4; i >= 0; i--)
    {
        for(j = 0; j < 5; j++)
            printf("%d", matrice[i][j]);

        printf("\n");
    }

    printf("\n");
}
*/


/**
 * \fn int verification_position(Couleur pl[20][20], int x, int y, Piece* p)
 * \brief Vérifie si la pièce peut être placée
 * \details Vérifie si chacun des carrés constituant la pièce peuvent être placés (La case qu'ils occuperont
    doit être VIDE)
 * \param pl Plateau de jeu
 * \param x Coordonnée x du carre d'origine de la pièce (coordonnée entrée par l'utilisateur)
 * \param y Coordonnée y du carre d'origine de la pièce (coordonnée entrée par l'utilisateur)
 * \param p Pointeur sur la pièce que l'utilisateur souhaite jouer
 */
int verification_position(Couleur pl[20][20], int x, int y, Piece* p)
{
    Carre* c = piece_liste_carre(p);

    /* Vérifie que chaque carré d'une pièce se situent sur une case VIDE */
    do
    {
        if(pl[x+carre_get_x(c)][y+carre_get_y(c)] != VIDE)
        {
            return 0;
        }
        c = carre_get_suiv(c);

    } while(c != piece_liste_carre(p));

    return 1;
}

/* Fonction qui vérifie si aucun Carre de la Couleur du Joueur n'est adjacant aux Carre que le Joueur veut poser
	et qu'il y a au moins un Carre de la Couleur du Joueur qui est en diagonale d'un Carre que le Joueur veut poser */

/**
 * \fn int verification_couleur(Couleur pl[20][20], int x, int y, Couleur col, Piece* p)
 * \brief Vérifie la position des pièces d'une même couleur entre elles
 * \details Fonction qui vérifie si aucun carré de la couleur du Joueur n'est adjacant aux carrés que le joueur veut
    poser et qu'il y a au moins un carré de la couleur du Joueur qui est en diagonale d'un carré que le joueur veut
    poser
 * \param pl Plateau de jeu
 * \param x Coordonnée x du carre d'origine de la pièce entrée par le joueur
 * \param y Coordonnée y du carre d'origine de la pièce entrée par le joueur
 * \param col Couleur du joueur souhaitant jouer la pièce
 * \param p Pointeur sur la pièce que l'utilisateur souhaite jouer
 */
int verification_couleur(Couleur pl[20][20], int x, int y, Couleur col, Piece* p)
{
    Carre* c = piece_liste_carre(p);
    int angle = 0;

    do
    {
	/* Vérifie qu'il n'y a aucun Carre adjacant aux Carre que le Joueur pose */
        if (pl[x + carre_get_x(c) - 1][y + carre_get_y(c)] == col || /* Au dessus */
            pl[x + carre_get_x(c) + 1][y + carre_get_y(c)] == col || /* En dessous */
            pl[x + carre_get_x(c)][y + carre_get_y(c) - 1] == col || /* A gauche */
            pl[x + carre_get_x(c)][y + carre_get_y(c) + 1] == col) /* A droite */
        {
            return 0;
        }

	/* Vérifie qu'il y a au moins un Carre que le Joueur pose qui touche diagonalement un Carre déjà posé de même Couleur */
        if ((pl[x + carre_get_x(c) - 1][y + carre_get_y(c) - 1] == col) || /* Diagonale Haut - Gauche */
            (pl[x + carre_get_x(c) + 1][y + carre_get_y(c) - 1] == col) || /* Diagonale Bas - Gauche */
            (pl[x + carre_get_x(c) - 1][y + carre_get_y(c) + 1] == col) || /* Diagonale Haut - Droit */
            (pl[x + carre_get_x(c) + 1][y + carre_get_y(c) + 1] == col)) /* Diagonale Bas - Droit */
        {
            angle = 1;
        }

        c = carre_get_suiv(c);

    } while(c != piece_liste_carre(p));

    /* Si au moins un Carre est en diagonale d'un Carre de même Couleur déjà posé */
    if(angle)
    {
        return 1;
    }

    /* Si aucun return n'a été fait précédemment */
    return 0;
}



/**
 * \fn void poser_piece(Couleur pl[20][20], Piece* pi, Joueur* j, int x, int y)
 * \brief Pose une pièce
 * \details Pose la pièce que le joueur a choisi aux coordonnees indiquées
 * \param pl Plateau de jeu
 * \param pi Pointeur sur la pièce à poser
 * \param x Coordonnée x du carre d'origine de la pièce entrée par le joueur
 * \param y Coordonnée y du carre d'origine de la pièce entrée par le joueur
 */
void poser_piece(Couleur pl[20][20], Piece* pi, Joueur* j, int x, int y)
{
    Carre* c = piece_liste_carre(pi);
    Piece** p = &(j->liste_piece);
    Piece* pivot = *p;

    /* Remplit les cases où l'on pose la pièce avec la couleur du joueur */
    do
    {
        pl[x+carre_get_x(c)][y+carre_get_y(c)] = joueur_couleur(j);
        c = carre_get_suiv(c);
    } while(c != piece_liste_carre(pi));

    while ((*p) != pi)
    {
        *p = piece_suivant(*p);
    }

	if (pivot == *p)
		pivot = NULL;

    /* Si la dernière pièce est le petit carré, alors le joueur gagne 5 points supplementaires */
    if(joueur_nb_piece_restantes(j) == 1 && c == carre_get_suiv(c))
        j->score += 5;

    liste_piece_suppr_elem(p);

    /* On redécale la liste de pièce pour la remettre dans l'ordre */
    while (pivot != NULL && (*p) != pivot)
        *p = piece_suivant(*p);

    j->liste_piece = *p;
}
















/* */
