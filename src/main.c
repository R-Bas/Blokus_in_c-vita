#include "../include/gestion_partie_sdl.h"
#include "../include/sdl.h"

int main(){

	/* S'il y a eu un problème dans l'ouverture de la SDL */
	if (!sdl_init(1))
		return 1;

	jouer_partie_sdl();

	sdl_close();

	return 0;
}
