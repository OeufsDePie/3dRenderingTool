#include"includes.c"

void lire_input(int**tab_appuis);
void deplacer_perso(struct perso*p, int*tab_appuis, float dt);
void rendu(struct terrain*le_terrain, struct camera*la_cam, struct perso*le_perso);
void evoluer_terrain();
void collisions_perso_terrain();

int main(int argc, char*argv[]){
	
	/* Fullscreen or not fullscreen (only for tests). */
	int fullscreen=0;
	
	
	/* Taille de la fenêtre. On règlera ces valeurs une fois que l'on sera passé en plein écran. */
	int largeur_fenetre=1000;
	int hauteur_fenetre=800;
	
	/* Dimensions du terrain. */
	int longueur=50;
	int largeur=50;
	int hauteur=50;
	/* Le point central du terrain. */
	float x_centre, y_centre, z_centre;
	
	/* Paramètres des cubes. */
	float taille_cubes=1.0;
	float restit_vertic=0.0;
	float restit_horiz=0.5;
	
	/* Caméra. */
	struct camera la_cam;
	float distance_a_perso_std=5;
	
	/* Perso. */
	struct perso le_perso;
	/* Coordonnées initiales du perso. */
	float x_perso_init=0;
	float y_perso_init=0;
	float z_perso_init=-3;
	float vitesse_de_saut=3;
	float vitesse_de_mvt=3;
	
	/* Fréquence d'affichage et de calcul. */
	float fps=24;
	float periode=1.0/fps;
	
	
	float fov=70; /* Champ de vision horizontal, en degrés. */
	/* Limites proche et lointaine du cône de vision. */
	float near=1;
	float far=1000;
	
	
	struct terrain le_terrain;
	
	int*tab_appuis;
	int i;
	
	enum mode mode=MODE_JEU;
	
	float angle_rotation=0.5;
	float distance_deplacement=0.35;
	
	SDL_Surface*surface_pour_taille_ecran;
	
	
        char* fichier_ply = "";
        /* Read the arguments */
        if(argc != 2) {
          /* Error */
          fprintf(stderr, "Usage : %s fichier.ply\n", argv[0]);
          return 1;
        } else {
          fichier_ply = argv[1];
        }
        
	
	/* Début init globale. */
	
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_WM_SetCaption("SDL GL Application", NULL);
	
	if(fullscreen){
		/* Fullscreen. */
		
		SDL_WM_GrabInput(SDL_GRAB_ON);
		SDL_ShowCursor(0);
		
		SDL_SetVideoMode(0, 0, 32, SDL_OPENGL | SDL_RESIZABLE | SDL_FULLSCREEN);
		
		/* On cherche à connaître les dimensions de l'écran. */
		surface_pour_taille_ecran=SDL_GetVideoSurface();
		largeur_fenetre=surface_pour_taille_ecran->w;
		hauteur_fenetre=surface_pour_taille_ecran->h;
	}
	else{
		/* Non fullscreen. */
		SDL_SetVideoMode(largeur_fenetre, hauteur_fenetre, 32, SDL_OPENGL | SDL_RESIZABLE);
	}
	
	
	/* Initialiser la position du personnage. */
	init_perso(&le_perso,
		   x_perso_init, y_perso_init, z_perso_init,
		   angle_rotation, distance_deplacement,
		   vitesse_de_saut, vitesse_de_mvt);
	init_terrain(&le_terrain, fichier_ply, longueur, largeur, hauteur, taille_cubes, restit_vertic, restit_horiz, &mode);
	get_centre_terrain(&le_terrain, &x_centre, &y_centre, &z_centre);
	
	/* Lier le perso et la caméra, pour que chacun connaisse l'autre;
	   positionner la caméra derrière le perso. */
	la_cam.distance_a_perso=distance_a_perso_std;
	printf("toto_0\n");
	attacher_cam_a_perso(&le_perso, &la_cam, la_cam.distance_a_perso);
	printf("toto_1\n");
	

	/* Les constantes suivantes ont des valeurs qui vont de SDLK_LAST à SDLK_LAST+5;
	   elles sont définies dans le fichier "includes.c".
	   DX_SOURIS
	   DY_SOURIS
	   CONTINUER
	   NIVEAU_MOLETTE
	   CLIC_GAUCHE_ACTIF
	   CLIC_DROIT_ACTIF
	*/
	tab_appuis=(int*)malloc((SDLK_LAST+6)*sizeof(int));
	for(i=0; i<SDLK_LAST+6; i++){
		tab_appuis[i]=0;
	}
	
	/* Activation du Z-Buffer. */
	glEnable(GL_DEPTH_TEST);
	
	/* Activation des textures 2d. */
	glEnable(GL_TEXTURE_2D);
	
	/* Activation de la transparence. */
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	
	gluPerspective(fov, ((double)largeur_fenetre)/((double)hauteur_fenetre), near, far);
	
	
	
	
	(tab_appuis)[DX_SOURIS]=0;
	(tab_appuis)[DY_SOURIS]=0;
	
	/* Ce flag continuer servira maintenant à boucler tant que l'utilisateur ne quitte pas. */
	(tab_appuis)[CONTINUER]=1;
	
	/* Fin init globale. */
	
	
	/**********************/
	/* BOUCLE PRINCIPALE. */
	/**********************/
	i=0;
	while((tab_appuis)[CONTINUER]){
		
		i++;
		
		/* date=clock(); */
		
		lire_input(&tab_appuis);
		
		/* Quitter. */
		if(tab_appuis[SDL_QUIT]==1 || tab_appuis[SDLK_ESCAPE]==1){
			(tab_appuis)[CONTINUER]=0;
		}
		
		deplacer_perso(&le_perso, tab_appuis, periode);
		
		/* On modifie la distance standard entre la cam et le perso. */
		avancer_camera_vers_perso_selon_molette(&la_cam, &(tab_appuis[NIVEAU_MOLETTE]));
		
		
		rendu(&le_terrain, &la_cam, &le_perso);
		
	}
	
	/**************************/
	/* FIN BOUCLE PRINCIPALE. */
	/**************************/
	
	return 0;
}



void lire_input(int**tab_appuis){
	
	SDL_Event e;
	/*
	  printf("lire\n");
	*/
	while(SDL_PollEvent(&e)!=0){
		
		switch(e.type){
		case SDL_QUIT:
			printf("SDL_QUIT détecté\n");
			(*tab_appuis)[SDL_QUIT]=1;
			(*tab_appuis)[CONTINUER]=0;
			break;
		case SDL_MOUSEMOTION:
			/* Les cases tab_appuis[DX_SOURIS] et tab_appuis[DY_SOURIS] mémorisent le déplacement total de la souris
			   depuis la dernière lecture par l'autre thread. */
			(*tab_appuis)[DX_SOURIS]=(*tab_appuis)[DX_SOURIS]+e.motion.xrel;
			(*tab_appuis)[DY_SOURIS]=(*tab_appuis)[DY_SOURIS]+e.motion.yrel;
		
			/* Bidouille... qui fonctionne... */
			e.motion.xrel=0;
			e.motion.yrel=0;
			break;
			
		case SDL_MOUSEBUTTONDOWN:
			/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////// */
			
			switch(e.button.button){
			case SDL_BUTTON_LEFT:
				(*tab_appuis)[CLIC_GAUCHE_ACTIF]=1;
				break;
			case SDL_BUTTON_MIDDLE:
				break;
			case SDL_BUTTON_RIGHT:
				(*tab_appuis)[CLIC_DROIT_ACTIF]=1;
				break;
			case SDL_BUTTON_WHEELUP:
				(*tab_appuis)[NIVEAU_MOLETTE]++;
				printf("niveau molette: %d\n", (*tab_appuis)[NIVEAU_MOLETTE]);
				break;
			case SDL_BUTTON_WHEELDOWN:
				(*tab_appuis)[NIVEAU_MOLETTE]--;
				printf("niveau molette: %d\n", (*tab_appuis)[NIVEAU_MOLETTE]);
				break;
			}
			/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
			/*
		case SDL_MOUSEBUTTONUP:
			switch(e.button.button){
			case SDL_BUTTON_LEFT:
				(*tab_appuis)[CLIC_GAUCHE_ACTIF]=0;
				break;
			default:
				break;
			}
			*/
			
		case SDL_KEYDOWN:
			/* GESTION DES APPUIS SUR LES TOUCHES. */
			(*tab_appuis)[e.key.keysym.sym]=1;
			/* FIN DE LA GESTION DES APPUIS SUR LES TOUCHES. */
			break;

		case SDL_KEYUP:
			/* GESTION DES RELÂCHEMENTS DES TOUCHES. */
			(*tab_appuis)[e.key.keysym.sym]=0;
			/* FIN DE LA GESTION DES RELÂCHEMENTS DES TOUCHES. */
			break;
	
		case SDL_VIDEORESIZE:
			printf("resize\n");
			/* On actualise le mode vidéo, avec le nouveau format de la fenêtre. */
			SDL_SetVideoMode(e.resize.w, e.resize.h, 32, SDL_OPENGL | SDL_RESIZABLE | SDL_FULLSCREEN);
			break;
		
		default: break;
		}
	}

	/**************************************************************/
	/* Actions en fonction des événements enregistrés.            */
	/**************************************************************/
			
	/* Quitter. */
	if((*tab_appuis)[SDL_QUIT]==1){
		(*tab_appuis)[CONTINUER]=0;
	}
	if((*tab_appuis)[SDLK_ESCAPE]==1){
		(*tab_appuis)[CONTINUER]=0;
	}
	
}			     

/******************************************************/
/* Affichage.                                         */
/* On n'effectue l'affichage que si on a suffisamment */
/* attendu depuis l'affichage précédent.              */
/******************************************************/
void rendu(struct terrain*le_terrain, struct camera*la_cam, struct perso*le_perso){
	
	/* Début affichage. */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	afficher_terrain(le_terrain, *la_cam);
	afficher_perso(le_perso, *la_cam);
	
	
	/* Rafraîchir l'écran. */
	glFlush();
	SDL_GL_SwapBuffers();
	/* Fin affichage. */
}

