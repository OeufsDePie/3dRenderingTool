#include<SDL/SDL.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<math.h>
#include<time.h>
#include<unistd.h>

/* Indices des cases où l'on stocke les variations de la position de la souris. */
#define DX_SOURIS SDLK_LAST
#define DY_SOURIS SDLK_LAST+1
#define CONTINUER SDLK_LAST+2
#define NIVEAU_MOLETTE SDLK_LAST+3
#define CLIC_GAUCHE_ACTIF SDLK_LAST+4
#define CLIC_DROIT_ACTIF SDLK_LAST+5

#define PI 3.141592653
enum mode{MODE_MENU, MODE_JEU};

//#include"afficher_tab.c"
//#include"puissance.c"
#include"minmax.c"
//#include"attendre.c"
#include"random.c"
#include"vecteur.c"
#include"empty.c"
//#include"diamant.c"
struct terrain;
#include"structures.c"
#include"camera.c"
struct cube;
#include"perso.c"
//#include"sphere.c"
#include"cube.c"
#include"terrain.c"
//#include"data_aff.c"
//#include"reception_evenements.c"
