#ifndef COMMUN_H
#define COMMUN_H

#define TAILLE_PLATEAU 20
#define TAILLE_PSEUDO 15
#define TAILLE_NOM_FICHIER 15
#define EXTENSION_SAVE_FICHIER ".blk"
#define DOSSIER_SAVE_FICHIER "save/"
#define NB_PIECES 21
#define NB_JOUEUR_MIN 2
#define NB_JOUEUR_MAX 4

#define L_FENETRE 960
#define H_FENETRE 544
#define LARG_T_VJ 38
#define LONG_T_VJ 9
#define LARG_T_BR 18
#define LONG_T_BR 18

#define BLUE_X 0
#define BLUE_Y 0
#define RED_X TAILLE_PLATEAU - 1
#define RED_Y TAILLE_PLATEAU - 1
#define GREEN_X 0
#define GREEN_Y TAILLE_PLATEAU - 1
#define YELLOW_X TAILLE_PLATEAU - 1
#define YELLOW_Y 0

#define PORT_DEFAUT 5555
#define TAILLE_BUFF 2000

/* Define pour le BOT */
#define PROFONDEUR 0

/* Entre 0 et 9 (0 == bord, 9 == 1 case du centre) */
#define COUP_BORD 1
#define COUP_MAUVAIS 3
#define COUP_MOYEN 5
#define COUP_BON 7

#define COEF_COUP_CENTRE 5
#define COEF_COUP_BON 4
#define COEF_COUP_MOYEN 3
#define COEF_COUP_MAUVAIS 2
#define COEF_COUP_BORD 1

#define COEF_CARRES_POSES 5
#define COEF_EMPLACEMENT_PIECE 2
#define COEF_CASES_DISPO 0.15
#define COEF_NOUVEAUX_COINS 3
#define COEF_COINS_BLOQUES 3

#define RAYON_CASE_DISPO 3

#define TEMPS_ATTENTE_BOT 1

#endif
