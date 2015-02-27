/* La structure de données qui contient l'ensemble des cubes et autres objets.
   
   À terme, il faudra pouvoir enregistrer le terrain dans un fichier,
   et charger un fichier pour créer un terrain.
*/

struct terrain{
	
	/* Le terrain contient un tableau tridimensionnel de cubes. */
	struct cube*tab;
	int nb_points;
	float taille_elem; /* Taille d'un cube. */
	enum mode*mode;
	struct vecteur gravite; /* Gravité de base dans tout le terrain, elle pourra être compensée ou modifiée par certains cubes. */
	struct vecteur gravite_sauvegarde; /* On mémorise la gravité quand on veut passer en mode vol libre. */
	/* Restitution de la vitesse après un choc vertical ou après un frottement tangentiel. */
	float restit_vertic;
	float restit_horiz;
	
	/* Nombre de cubes dans chaque direction que l'on affichera. */
	int distance_brouillard;
	
	/* Paramètres inutilisés. */
	float longueur, largeur, hauteur;
};

void trouver_cube_selon_coordonnees(struct terrain*t, float x, float y, float z, int*i, int*j, int*k);
struct cube*trouver_cube_selon_viseur(struct terrain*t, struct perso*p, int*indice_face);
void get_centre_terrain(struct terrain*t, float*x, float*y, float*z);
struct cube*get_cube(struct terrain*t, int i);
int point_est_dans_cube(struct terrain*t, float x, float y, float z);
void trouver_face_la_plus_proche(struct cube*c, struct vecteur*pos, int*indice_face);
struct cube*trouver_voisin_selon_face(struct cube*le_cube, struct terrain*t, int indice_face);

/* Cette équation permet de spécifier quels cubes existent au début.
   Un cube existe si la fonction equation_terrain renvoie vrai (çad 1). */
float equation_terrain(struct terrain*t, float x0, float y0, float z0){
	float x=x0-t->longueur/2;
	float y=y0-t->largeur/2;
	float z=z0-t->hauteur/2;
	float r2=x*x+y*y+z*z;
	
	int type_terrain=8;
	
	switch(type_terrain){
	case 0: /* Terrain vide. */
		return 0;
		break;
	case 1: /* Sol horizontal. */
		return (x*x + y*y)<50 && z*z<1;
		break;
	case 2: /* Deux murs. */
		return (z>=(t->taille_elem)*10 && z<(t->taille_elem)*11)
			||(x<=(t->taille_elem)*3 && x>(t->taille_elem)*2)
			||(y<=(t->taille_elem)*3 && y>(t->taille_elem)*2);
		break;
	case 3: /* Sphère. */
		return (r2<13*13) && (x*x+y*y+z*z > 12*12);
		break;
	case 4: /* Pente régulière. */
		return z<(x+2*y)/5-5;
		break;
	case 5: /* Paraboloïde. */
		return (x*x+y*y)<20*z && (x*x+y*y)>20*(z-2);
		break;
	case 6: /* Un seul cube au centre. */
		return x*x<1 && y*y<1 && z*z<1;
		break;
	case 7: /* Terrain plein avec des tunnels, avec un mur vertical au fond. */
		return 1;
		break;
	case 8: /* Terrain sinusoïdal. */
		return z<sin(0.1*x) + cos(0.3*y);
		break;
	default:
		return random_int_a_b(0, 3)==0;
		break;
	}
	 
}


void init_gravite_terrain(struct terrain*t, float gx, float gy, float gz){

}



/* L'argument fichier_ply contient le chemin vers le nom de fichier nuage de point
   La variable énumérée mode permet de savoir si on est en MODE_JEU, ou en MODE_MENU, ou dans d'autres modes.
   Les paramètres longueur, largeur, hauteur désignent le nombres de cubes
   dans chaque direction (x, y et z).
   Le paramètre taille_elem désigne la longueur des arêtes des cubes.
 */
void init_terrain(struct terrain*t, const char* fichier_ply,
                  int longueur, int largeur, int hauteur,
                  float taille_elem, float restit_vertic, float restit_horiz,
                  enum mode*mode){
	
	int i;
	
	/* Lecture du fichier. */
	FILE*fichier=fopen(fichier_ply, "r");
	char ligne[100];
	char*mot;
	struct cube*le_cube;
	
	/* Coordonnées et couleur du nouveau cube. */
	float x_c, y_c, z_c;
	int r_c, g_c, b_c;
	float taille=0.2;
	
	printf("Choix du fichier: terrain.c; on ouvre <%s>\n", fichier_ply);
	
	
	/* Lecture de l'en-tête. */
	i=0;
	do{
		fgets(ligne, 100, fichier);
		ligne[strlen(ligne)-1]='\0'; /* On remplace le '\n' fixé par fgets. */
		mot=strtok(ligne, " ");
		/* Si cette ligne contient le nombre de points du maillage... */
		if(strcmp(mot, "element")==0){
			mot=strtok(NULL, " ");
			if(strcmp(mot, "vertex")==0){
				/* Alors le troisième mot de la ligne est le nombre de points. */
				mot=strtok(NULL, " ");
				t->nb_points=atoi(mot);
			}
		}
		i++;
	}while(strcmp(ligne, "end_header")!=0);
	
	printf("On a donc %d points.\n", t->nb_points);
	t->tab=(struct cube*)malloc(t->nb_points*sizeof(struct cube));
	
	
	/* Lecture des points. */
	i=0;
	while(i<t->nb_points && ligne!=NULL){
		fgets(ligne, 100, fichier);
		ligne[strlen(ligne)-1]='\0';
		
		if(ligne!=NULL){
			le_cube=get_cube(t, i);
			/* ligne vaut "4.1399 -3.99534  19.1157 253 250 241"; */
			
			/* Coordonnées du cube. */
			mot=strtok(ligne, " ");
			x_c=atof(mot);
			mot=strtok(NULL, " ");
			y_c=atof(mot);
			mot=strtok(NULL, " ");
			z_c=atof(mot);
			/* Couleur. */
			mot=strtok(NULL, " ");
			r_c=atof(mot);
			mot=strtok(NULL, " ");
			g_c=atof(mot);
			mot=strtok(NULL, " ");
			b_c=atof(mot);
			
			
			le_cube->taille=0.1;
			init_cube(le_cube,
				  x_c, y_c, z_c,
				  taille);
			le_cube->r=r_c;
			le_cube->g=g_c;
			le_cube->b=b_c;
			le_cube->transparence=128;
			le_cube->existe=1;
		}
		else{
			printf("point %d inexistant.\n", i);
		}
		i++;
	}
	
	
	
	
	printf("fin init_terrain\n");
}


/* Si les valeurs i, j et k correspondent à un cube du terrain, même vide, alors on renvoie un pointeur vers ce cube;
   sinon, on renvoie NULL. */
struct cube*get_cube(struct terrain*t, int i){
	if(i>=0 && i<t->nb_points){
		return &(t->tab[i]);
	}
	else{
		return NULL;
	}
}


void afficher_terrain_float(struct terrain*t,
			    float xCam, float yCam, float zCam,
			    float xCible, float yCible, float zCible,
			    float xVertic, float yVertic, float zVertic){
	
	/* Indice pour les boucles. */
	int i;
	struct cube*le_cube;
	gluLookAt(xCam ,yCam, zCam,
		  xCam+xCible, yCam+yCible, zCam+zCible,
		  xVertic, yVertic, zVertic);
	for(i=0; i<t->nb_points; i++){
		le_cube=&(t->tab[i]);
		/*
		  printf("\taffichage cube %d sur %d; pos={%f, %f, %f}\n",
		  i,
		  t->nb_points,
		  le_cube->x, le_cube->y, le_cube->z);
		*/
		afficher_cube(le_cube);
	}
}





void afficher_terrain(struct terrain*t, struct camera c){
	
	
	afficher_terrain_float(t,
			       c.empty.pos.x, c.empty.pos.y, c.empty.pos.z,
			       c.empty.cible.x, c.empty.cible.y, c.empty.cible.z,
			       c.empty.vertic.x, c.empty.vertic.y, c.empty.vertic.z);
}





/* On cherche le cube contenant le point (x, y, z);
   on place ses indices dans les entiers désignés par les pointeurs i, j et k.
   ATTENTION!!! Cette fonction devra être modifiée si on change la façon dont on place les cubes
   lors de l'initialisation. */
void trouver_cube_selon_coordonnees(struct terrain*t, float x, float y, float z, int*i, int*j, int*k){
	float taille=t->taille_elem;
	*i=(int)(x/taille + 0.5);
	*j=(int)(y/taille + 0.5);
	*k=(int)(z/taille + 0.5);
}





/* Afficher les cubes du terrain t qui constituent la tranche d'indice i.
   On rappelle que la caméra se situe dans le cube (i0, j0, k0). */
void _afficher_tranche(struct terrain*t, int i, int j0, int k0){
}



/* Afficher les cubes du terrain t qui appartiennent à une colonne verticale,
   et dont les deux premiers indices sont i et j;
   on va du bas jusqu'à l'indice k0-1, puis du haut jusqu'à l'indice k0; */
void _afficher_colonne_de_cubes(struct terrain*t, int i, int j, int k0){
}


/* Trouver le point situé au centre du terrain. */
void get_centre_terrain(struct terrain*t, float*x, float*y, float*z){
	
	float x_min=((float)0)*t->taille_elem;
	float x_max=((float)(t->longueur))*t->taille_elem;
	float y_min=((float)0)*t->taille_elem;
	float y_max=((float)(t->largeur))*t->taille_elem;
	float z_min=((float)0)*t->taille_elem;
	float z_max=((float)(t->hauteur))*t->taille_elem;
	*x=(x_min+x_max)/2;
	*y=(y_min+y_max)/2;
	*z=(z_min+z_max)/2;
}



/* Appliquer au perso la gravité globale du terrain. */
void appliquer_gravite_globale_terrain_sur_perso(struct terrain*t, struct perso*p, float dt){
	struct vecteur increment; /* La vitesse qui sera ajoutée à la vitesse actuelle. */
	if(t!=NULL && p!=NULL){
		init_vecteur(&increment,
			     t->gravite.x*dt,
			     t->gravite.y*dt,
			     t->gravite.z*dt);
		somme_vecteurs(p->vitesse,
			       increment,
			       &(p->vitesse));
	}
}
	
	
