/* Ce fichier décrit le personnage commandé au clavier et à la souris par le joueur.
 */

void init_perso(struct perso*p, float x, float y, float z, float angle, float distance, float vitesse_saut, float vitesse_mvt);
void deplacer_perso_local(struct perso*p, float dx, float dy, float dz);
void tourner_perso_z(struct perso*p, float angle);
void aligner_cam_sur_perso(struct camera*c, struct perso*p);
void calculer_pave_perso(struct perso*p);
void dessiner_pave_englobant(struct perso*p);
void placer_perso_global(struct perso*p, float x, float y, float z);
void deplacer_perso_global(struct perso*p, float dx, float dy, float dz);
void deplacer_perso_selon_vitesse_globale(struct perso*p, float dt);
void calculer_vitesse_perso(struct perso*le_perso, int*tab_appuis, float dt, struct vecteur*vitesse_locale);
void attacher_cam_a_perso(struct perso*p, struct camera*c, float distance);
void tourner_perso_y_local(struct perso*p, float angle);
void dessiner_vecteurs(struct perso*p);
void deplacer_perso(struct perso*p, int*tab_appuis, float dt);
void appliquer_vitesse_perso(struct perso*le_perso, float dt, struct vecteur*v);
void perso_to_string(struct perso*p);

void perso_to_string(struct perso*p){
	printf("perso{\n");
	empty_to_string(&(p->empty));
	printf("\n}");
}

	

void changer_type(struct perso*p){
	printf("changer_type\n");
	switch(p->type){
	case PIETON:
		p->type=VEHICULE;
		break;
	case VEHICULE:
		p->type=PIETON;
		break;
	default:
		p->type=PIETON;
	}
}



/* Ouvrir le fichier contenant le maillage et l'enregistrer dans la structure de données.
   On supppose que lors de l'initialisation, l'empty du perso est centré sur le repère,
   et que les coordonnées des points sont exprimés par rapport à ce centre.
 */
void charger_maillage(struct perso*p, FILE*f){
	char s[100];
	char*mot;
	int i, j;
	
	/* Nombre de points. */
	fgets(s, 100, f);
	p->nb_points=atoi(s);
	/* Nombre de faces. */
	fgets(s, 100, f);
	p->nb_faces=atoi(s);
	
	/* Une ligne en trop: "liste des sommets"; */
	fgets(s, 100, f);
	
	p->tab_points=(struct vecteur*)malloc(p->nb_points*sizeof(struct vecteur));
	for(i=0; i<p->nb_points; i++){
		fgets(s, 100, f);
		/* Début de la ligne: indice du point (inutile ici). */
		mot=strtok(s, " ");
		/* Suite: coordonnées des points, séparées par des espaces. */
		mot=strtok(NULL, " ");
		p->tab_points[i].x=atof(mot)+p->empty.pos.x;
		
		mot=strtok(NULL, " ");
		p->tab_points[i].y=atof(mot)+p->empty.pos.y;
		
		mot=strtok(NULL, " ");
		p->tab_points[i].z=atof(mot)+p->empty.pos.z;
	}
	
	/* Une ligne en trop: "liste de couleurs"; */
	fgets(s, 100, f);
	
	p->tab_couleurs=(int**)malloc(p->nb_points*sizeof(int*));
	for(i=0; i<p->nb_points; i++){
		p->tab_couleurs[i]=(int*)malloc(3*sizeof(int));
		fgets(s, 100, f);
		/* Début de la ligne: indice du point (inutile ici). */
		mot=strtok(s, " ");
		
		mot=strtok(NULL, " ");
		p->tab_couleurs[i][0]=atoi(mot);
		
		for(j=1; j<3; j++){
			mot=strtok(NULL, " ");
			p->tab_couleurs[i][j]=atoi(mot);
		}
	}
	
	/* Une ligne en trop: "liste de faces"; */
	fgets(s, 100, f);
	
	p->tab_faces=(int**)malloc(p->nb_faces*sizeof(float*));
	for(i=0; i<p->nb_faces; i++){
		p->tab_faces[i]=(int*)malloc(3*sizeof(int));
		fgets(s, 100, f);
		/* Début de la ligne: indice du point (inutile ici). */
		mot=strtok(s, " ");
		
		mot=strtok(NULL, " ");
		p->tab_faces[i][0]=atoi(mot);
		
		for(j=1; j<3; j++){
			mot=strtok(NULL, " ");
			p->tab_faces[i][j]=atoi(mot);
		}
	}
	
	
	
	/* Calcul du rayon du perso: la distance entre le centre de l'empty et le point le plus éloigné. */
	calculer_pave_perso(p);
	
	
}

void afficher_coords_sommets(struct perso*p){
	int i;
	
	printf("\n\ntab_points:\n");
	for(i=0; i<p->nb_points; i++){
		printf("%f %f %f\n",
		       p->tab_points[i].x,
		       p->tab_points[i].y,
		       p->tab_points[i].z);
		
		printf("\n");
	}
}

void calculer_pave_perso(struct perso*p){
	
	int i;
	float x_min=p->tab_points[0].x;
	float x_max=p->tab_points[0].x;
	float y_min=p->tab_points[0].y;
	float y_max=p->tab_points[0].y;
	float z_min=p->tab_points[0].z;
	float z_max=p->tab_points[0].z;
	
	
	for(i=1; i<p->nb_points; i++){
		x_min=min(x_min, p->tab_points[i].x);
		x_max=max(x_max, p->tab_points[i].x);
		y_min=min(y_min, p->tab_points[i].y);
		y_max=max(y_max, p->tab_points[i].y);
		z_min=min(z_min, p->tab_points[i].z);
		z_max=max(z_max, p->tab_points[i].z);
	}
	/* Centre du pavé englobant. */
	p->empty.pos.x=(x_min+x_max)/2;
	p->empty.pos.y=(y_min+y_max)/2;
	p->empty.pos.z=(z_min+z_max)/2;
	/* Taille de ce pavé. */
	p->hauteur=2*(z_max-p->empty.pos.z);
	p->longueur=max( (x_max-x_min), (y_max-y_min) );
}




void init_perso(struct perso*p,
		float x, float y, float z,
		float angle, float distance, float vitesse_saut, float vitesse_mvt){
	FILE*f;
	init_empty(&(p->empty),
		   x, y, z,
		   1, 0, 0,
		   0, 0, 1);
	p->r=255;
	p->g=100;
	p->b=25;
	
	printf("init_perso (x=%f, y=%f, z=%f)\n", p->empty.pos.x, p->empty.pos.y, p->empty.pos.z);
	afficher_empty(&(p->empty));
	
	
	f=fopen("perso.mesh", "r");
	if(f!=NULL){
		charger_maillage(p, f);
	}
	else{
		printf("erreur chargement perso.\n");
	}
	init_vecteur(&(p->vitesse), 0, 0, 0);
	p->vitesse_de_saut=vitesse_saut;
	
	p->vitesse_de_mvt=vitesse_mvt;
	
	init_vecteur(&(p->acc), 1.0, 1.0, 1.0);
	
	p->angle_rotation=angle;
	p->distance_deplacement=distance;
	
	/* afficher_empty(&(p->empty)); */
}



void afficher_face(struct perso*p, float dx, float dy, float dz, int transparence){
	int r, g, b;
	if(dy>0){
		r=255; g=0; b=0;
	}
	else{
		r=0; g=0; b=255;
	}
	glColor4ub(r, g, b, transparence);
	glVertex3d(p->empty.pos.x-dx, p->empty.pos.y+dy, p->empty.pos.z-dz);
	glVertex3d(p->empty.pos.x-dx, p->empty.pos.y+dy, p->empty.pos.z+dz);
	glVertex3d(p->empty.pos.x+dx, p->empty.pos.y+dy, p->empty.pos.z+dz);
	glVertex3d(p->empty.pos.x+dx, p->empty.pos.y+dy, p->empty.pos.z-dz);
}
			

void afficher_perso(struct perso*p, struct camera c){
	int i;
	
	/* float h, b, g, d, av, ar; */
	struct vecteur*tab_p=p->tab_points;
	int**tab_f=p->tab_faces;
	int**tab_c=p->tab_couleurs;
	int n0, n1, n2; /* Indices des sommets. */
	
	int transparence;
	
	
	/* Afficher ou non la "peau" du perso. */
	int flag_affichage_maillage=0;
	
	/* Pour les tests. */
	int flag_affichage_pave=0;
	float dx=p->longueur/2.1;
	float dy=p->longueur/2.1;
	float dz=p->hauteur/2.1;
	/* Fin pour les tests. */
	
	/* Pour le viseur. */
	float fact_x=0.02;
	float fact_z=0.1;
	
	struct vecteur centre_visee;
	init_vecteur(&centre_visee, p->empty.pos.x, p->empty.pos.y, p->empty.pos.z+p->hauteur/2);
	
	
	/* Afficher le curseur de visée, situé au centre du perso.
	*/
	transparence=0;

	
	glColor4ub(255, 255, 255, transparence);
	glBegin(GL_QUADS);{
		
		glVertex3d(p->empty.pos.x + fact_x*c.empty.gauche.x,
			   p->empty.pos.y + fact_x*c.empty.gauche.y,
			   p->empty.pos.z + fact_x*c.empty.gauche.z);
		
		glVertex3d(p->empty.pos.x + fact_z*c.empty.vertic.x,
			   p->empty.pos.y + fact_z*c.empty.vertic.y,
			   p->empty.pos.z + fact_z*c.empty.vertic.z);
		
		glVertex3d(p->empty.pos.x - fact_x*c.empty.gauche.x,
			   p->empty.pos.y - fact_x*c.empty.gauche.y,
			   p->empty.pos.z + fact_x*c.empty.gauche.z);
		
		glVertex3d(p->empty.pos.x - fact_z*c.empty.vertic.x,
			   p->empty.pos.y - fact_z*c.empty.vertic.y,
			   p->empty.pos.z - fact_z*c.empty.vertic.z);
	}
	glEnd();
	
	transparence=127;
	
	if(flag_affichage_pave){
		/* On affiche un simple pavé, celui qui définit l'enveloppe de collision du perso. */
		glBegin(GL_QUADS);{
				afficher_face(p, dx, +dy, dz, transparence);
				afficher_face(p, dx, -dy, dz, transparence);
		}
		glEnd();
	}
	if(flag_affichage_maillage){
		/* On affiche le maillage réel. */
		glBegin(GL_TRIANGLES);{
		
			for(i=0; i<p->nb_faces; i++){
				/* Afficher la face numéro i. */
				n0=tab_f[i][0];
				n1=tab_f[i][1];
				n2=tab_f[i][2];
			
				glColor4ub(tab_c[n0][0], tab_c[n0][1], tab_c[n0][2], transparence);
				glVertex3d(tab_p[n0].x, tab_p[n0].y, tab_p[n0].z);
				glColor4ub(tab_c[n1][0], tab_c[n1][1], tab_c[n1][2], transparence);
				glVertex3d(tab_p[n1].x, tab_p[n1].y, tab_p[n1].z);
				glColor4ub(tab_c[n2][0], tab_c[n2][1], tab_c[n2][2], transparence);
				glVertex3d(tab_p[n2].x, tab_p[n2].y, tab_p[n2].z);
			}
		}
		glEnd();
	}
}

void dessiner_vecteurs(struct perso*p){
	float x_centre=p->empty.pos.x;
	float y_centre=p->empty.pos.y;
	float z_centre=p->empty.pos.z;
	
	float x_cible=p->empty.cible.x;
	float y_cible=p->empty.cible.y;
	float z_cible=p->empty.cible.z;
	
	float x_vertic=p->empty.vertic.x;
	float y_vertic=p->empty.vertic.y;
	float z_vertic=p->empty.vertic.z;
	
	/*
	  float c=cos(p->empty.theta_z);
	  float s=sin(p->empty.theta_z);
	*/
	
	/* ecrire_empty(&(p->empty)); */
	
	glBegin(GL_TRIANGLES);{
		glColor4ub(255, 255, 255, 0);
		glVertex3d(x_centre+x_vertic, y_centre+y_vertic, z_centre+z_vertic); /* Pointe. */
		glVertex3d(x_centre-y_vertic, y_centre+x_vertic, z_centre);
		glVertex3d(x_centre+y_vertic, y_centre-x_vertic, z_centre);
		
		glColor4ub(255, 0, 0, 0);
		glVertex3d(x_centre+x_cible, y_centre+y_cible, z_centre+z_cible); /* Pointe. */
		glVertex3d(x_centre-y_cible, y_centre+x_cible, z_centre);
		glVertex3d(x_centre+y_cible, y_centre-x_cible, z_centre);
		
		
	}
	glEnd();
		
}


void dessiner_pave_englobant(struct perso*p){
	int k;
	float x_centre=p->empty.pos.x;
	float y_centre=p->empty.pos.y;
	float z_centre=p->empty.pos.z;
	float longueur=p->longueur;
	float hauteur=p->hauteur;
	
	for(k=-1; k<=+1; k=k+2){
		glBegin(GL_QUADS);{
			glVertex3d(x_centre+longueur/2, y_centre+longueur/2, z_centre+k*hauteur/2);
			glVertex3d(x_centre+longueur/2, y_centre-longueur/2, z_centre+k*hauteur/2);
			glVertex3d(x_centre-longueur/2, y_centre-longueur/2, z_centre+k*hauteur/2);
			glVertex3d(x_centre-longueur/2, y_centre+longueur/2, z_centre+k*hauteur/2);
		}glEnd();
	}
}


/* Calculer la vitesse locale du personnage en fonction des commandes envoyées par le clavier. */
void calculer_vitesse_perso(struct perso*le_perso, int*tab_appuis, float dt, struct vecteur*vitesse_locale){
	
	
	
	/* Avancer sur l'axe X local. */
	if(tab_appuis[SDLK_UP] || tab_appuis[SDLK_z]){
		/* vitesse_locale->x = vitesse_locale->x+le_perso->vitesse_de_saut; */
		vitesse_locale->x = le_perso->vitesse_de_mvt;
	}
	/* Reculer sur l'axe X local. */
	if(tab_appuis[SDLK_DOWN] || tab_appuis[SDLK_s]){
		/* vitesse_locale->x = vitesse_locale->x-le_perso->vitesse_de_saut; */
		vitesse_locale->x = -le_perso->vitesse_de_mvt;
	}
	/* Avancer sur l'axe Y local. */
	if(tab_appuis[SDLK_q]){
		/* vitesse_locale->y = vitesse_locale->y+le_perso->vitesse_de_saut; */
		vitesse_locale->y = le_perso->vitesse_de_mvt;
	}
	/* Reculer sur l'axe Y local. */
	if(tab_appuis[SDLK_d]){
		/* vitesse_locale->y = vitesse_locale->y-le_perso->vitesse_de_saut; */
		vitesse_locale->y = -le_perso->vitesse_de_mvt;
	}
	/* Sauter verticalement sur l'axe Z. */
	if(tab_appuis[SDLK_SPACE]){
		tab_appuis[SDLK_SPACE]=0; /* On ne déclenche le saut qu'une seule fois. */
		vitesse_locale->z += le_perso->vitesse_de_saut;
	}
	/* Descendre sur l'axe Z. */
	if(tab_appuis[SDLK_LSHIFT]){
		tab_appuis[SDLK_LSHIFT]=0;
		vitesse_locale->z += -le_perso->vitesse_de_saut;
	}
		
	if(tab_appuis[SDLK_LEFT]){
		tourner_perso_z(le_perso, le_perso->angle_rotation*dt);
		tourner_cam_z(le_perso->la_cam, le_perso->angle_rotation*dt);
	}
	if(tab_appuis[SDLK_RIGHT]){
		tourner_perso_z(le_perso, -le_perso->angle_rotation*dt);
		tourner_cam_z(le_perso->la_cam, -le_perso->angle_rotation*dt);
	}
	
	/* Les cases DX_SOURIS et DY_SOURIS contiennent les coordonnées du dernier déplacement de la souris. */
	/* DÉBUG: On ne fait tourner le perso que si les touches CTRL sont relâchées. */
	if(!tab_appuis[SDLK_LCTRL] && !tab_appuis[SDLK_RCTRL]){
		tourner_perso_z(le_perso, -le_perso->angle_rotation*((float)(tab_appuis[DX_SOURIS]))/100);
		tourner_cam_y_local(le_perso->la_cam, -le_perso->angle_rotation*((float)(tab_appuis[DY_SOURIS]))/100);
		tourner_cam_z(le_perso->la_cam, -le_perso->angle_rotation*((float)(tab_appuis[DX_SOURIS]))/100);
	}
	tab_appuis[DX_SOURIS]=0; /* RAZ. */
	tab_appuis[DY_SOURIS]=0; /* RAZ. */
	
	
	/* Pour les tests: raz a vitesse. */
	if(tab_appuis[SDLK_KP0] || tab_appuis[SDLK_LCTRL]){
		init_vecteur(vitesse_locale, 0, 0, 0);

	}
	
	/* Ici on a fini de lire le clavier et la souris, et on a modifié la vitesse du perso; cette vitesse est exprimée dans le repère local au perso. */
	
	
	/* Outils pour le développement. */
	
	/* Changer de vue. */
	/* 7: vue de dessus. */
	if(tab_appuis[SDLK_KP7]){
		attacher_cam_a_perso(le_perso, le_perso->la_cam, le_perso->la_cam->distance_a_perso);
		/* Remonter la caméra pour la placer au-dessus du perso. */
		tourner_cam_y_local(le_perso->la_cam, PI/2);
	}
	
	/* 1: vue de face. */
	if(tab_appuis[SDLK_KP1]){
		attacher_cam_a_perso(le_perso, le_perso->la_cam, le_perso->la_cam->distance_a_perso);
	}
	
}





void placer_perso_global(struct perso*p, float x, float y, float z){
	float xp=p->empty.pos.x;
	float yp=p->empty.pos.y;
	float zp=p->empty.pos.z;
	
	deplacer_perso_global(p, x-xp, y-yp, z-zp);
	/* deplacer_cam_global(p->la_cam, x-xp, y-yp, z-zp); */
}




/* On déplacer le perso en exprimant son mouvement dans le repère global,
   et on déplace en même temps la caméra de la manière adaptée. */

void deplacer_perso_global(struct perso*p, float dx, float dy, float dz){
	int i;
	
	/* Position du perso avant déplacement. */
	/*
	  float x, y, z;
	  x=p->empty.pos.x;
	  y=p->empty.pos.y;
	  z=p->empty.pos.z;
	*/
	deplacer_empty_global(&(p->empty), dx, dy, dz);
	for(i=0; i<p->nb_points; i++){
		/* Déplacer le i-ème sommet. */
		p->tab_points[i].x=p->tab_points[i].x + dx;
		p->tab_points[i].y=p->tab_points[i].y + dy;
		p->tab_points[i].z=p->tab_points[i].z + dz;
	}
	
	
	/* Quand le perso se déplace, la caméra se déplace aussi.
	 */
	switch(p->type){
	case PIETON:
		/* En mode piéton, la caméra conserve la même orientation. */
		deplacer_cam_global(p->la_cam, dx, dy, dz);		
		break;
	case VEHICULE:
		/* En mode véhicule, la caméra se déplace en direction de l'ancienne position du perso puis se tourne pour s'aligner sur lui. */
		deplacer_cam_local(p->la_cam, sqrt(dx*dx+dy*dy+dz*dz), 0, 0);
		break;
	default:
		break;
	}	
}
	
void deplacer_perso_local(struct perso*p, float dx, float dy, float dz){
	int i;
	/*
	  printf("cible de l'empty: %f, %f, %f\n",
	  p->empty.cible.x,
	  p->empty.cible.y,
	  p->empty.cible.z);
	*/
	deplacer_empty_local(&(p->empty), dx, dy, dz);
	for(i=0; i<p->nb_points; i++){
		/* Déplacer le i-ème sommet. */
		incrementer_vecteur(&(p->tab_points[i]), dx, p->empty.cible);
		incrementer_vecteur(&(p->tab_points[i]), dy, p->empty.gauche);
		incrementer_vecteur(&(p->tab_points[i]), dz, p->empty.vertic);
	}
}

/* Il faut faire tourner l'empty pour que les prochains déplacements se fassent dans la bonne direction.
   Il faut aussi faire tourner chaque point autour de l'axe Z passant par l'empty.
*/
void tourner_perso_z(struct perso*p, float angle){
	int i;
	
	/* 1) On ramène le perso au centre du repère. */
	float x=p->empty.pos.x;
	float y=p->empty.pos.y;
	float z=p->empty.pos.z;
	deplacer_perso_global(p, -x, -y, -z);
			      
	/* 2) On fait tourner le perso autour de l'axe Z. */
	tourner_empty_z(&(p->empty), angle);
	for(i=0; i<p->nb_points; i++){
		rotation_vecteur(&(p->tab_points[i]), 0, 0, angle);
	}
	
	/* 3) On replace le perso à sa position initiale. */
	deplacer_perso_global(p, x, y, z);
	
	/* 4) On mémorise la rotation dans l'empty du perso. */
	p->empty.theta_z=p->empty.theta_z+angle;
}

void tourner_perso_y_local(struct perso*p, float angle){
	tourner_empty_y_local(&(p->empty), angle);
	tourner_cam_y_local(p->la_cam, angle);
}








/* Faire en sorte que le personnage soit situé au centre du champ de vision de la caméra,
   mais sans modifier la position de la caméra. */
void aligner_cam_sur_perso(struct camera*c, struct perso*p){
	/* On fait tourner la cam sur l'axe Z pour que son theta_z corresponde à celui du perso. */
	/*
	  float distance=distance_entre_emptys(&(c->empty), &(p->empty));
	  deplacer_cam_local(c, distance, 0, 0);
	  tourner_cam_z(c, p->empty.theta_z - c->empty.theta_z);
	  deplacer_cam_local(c, -distance, 0, 0);
	*/
	/*
	  init_vecteur(&(c->empty.cible),
	  p->empty.pos.x,
	  p->empty.pos.y,
	  p->empty.pos.z);
	*/

	
}




/* Exprimer la vitesse du perso dans le repère global en fonction de son expression dans le repère local. */
void convertir_vitesse_locale_vers_globale(struct perso*p, struct vecteur*vitesse_locale){
	/* Composantes de la vitesse locale. */
	float vxl=vitesse_locale->x;
	float vyl=vitesse_locale->y;
	float vzl=vitesse_locale->z;
	
	/* Les trois vecteurs définissant l'empty du perso. */
	float xcib=p->empty.cible.x;
	float ycib=p->empty.cible.y;
	float zcib=p->empty.cible.z;
	
	float xg=p->empty.gauche.x;
	float yg=p->empty.gauche.y;
	float zg=p->empty.gauche.z;
	
	float xv=p->empty.vertic.x;
	float yv=p->empty.vertic.y;
	float zv=p->empty.vertic.z;
	
	/* La vitesse du perso exprimée dans le repère global. */
	float vxg = vxl*xcib + vyl*xg + vzl*xv;
	float vyg = vxl*ycib + vyl*yg + vzl*yv;
	float vzg = vxl*zcib + vyl*zg + vzl*zv;
	
	init_vecteur(&(p->vitesse), vxg, vyg, vzg);
}
/* Exprimer la vitesse du perso dans le repère local en fonction de son expression dans le repère global. */
void convertir_vitesse_globale_vers_locale(struct perso*p, struct vecteur*vitesse_locale){
	/* Composantes de la vitesse globale. */
	float vxg=p->vitesse.x;
	float vyg=p->vitesse.y;
	float vzg=p->vitesse.z;
	
	/* Les trois vecteurs définissant l'empty du perso. */
	float xcib=p->empty.cible.x;
	float ycib=p->empty.cible.y;
	float zcib=p->empty.cible.z;
	
	float xg=p->empty.gauche.x;
	float yg=p->empty.gauche.y;
	float zg=p->empty.gauche.z;
	
	float xv=p->empty.vertic.x;
	float yv=p->empty.vertic.y;
	float zv=p->empty.vertic.z;
	
	/* La vitesse du perso exprimée dans le repère local. */
	float vxl = vxg*xcib + vyg*ycib + vzg*zcib;
	float vyl = vxg*xg   + vyg*yg   + vzg*zg  ;
	float vzl = vxg*xv   + vyg*yv   + vzg*zv  ;
	
	init_vecteur(vitesse_locale, vxl, vyl, vzl);
}



/* La cible de la caméra devra toujours être le personnage. */
void attacher_cam_a_perso(struct perso*p, struct camera*c, float distance){
	printf("attacher_cam_a_perso\n");
	/* Chacun connaît l'autre. */
	p->la_cam=c;
	c->le_perso=p;
	
	
	/* On positionne la caméra derrière le perso. */
	init_vecteur(&(c->empty.pos),
		     p->empty.pos.x - distance,
		     p->empty.pos.y,
		     p->empty.pos.z);
	
	/* On fait pointer la cam vers le perso, et on règle donc les trois vecteurs de l'empty de la cam. */
	init_vecteur(&(c->empty.cible),
		     p->empty.pos.x - c->empty.pos.x,
		     p->empty.pos.y - c->empty.pos.y,
		     p->empty.pos.z - c->empty.pos.z);
	
	init_vecteur(&(c->empty.vertic),
		     0, 0, 1);
	
	produit_vectoriel(c->empty.vertic,
			  c->empty.cible,
			  &(c->empty.gauche));
	
	/* On recopie vers le perso l'orientation de l'empty de la caméra,
	   et on norme ces vecteurs pour le perso. */
	clone_vecteur(c->empty.cible, &(p->empty.cible));
	clone_vecteur(c->empty.gauche, &(p->empty.gauche));
	clone_vecteur(c->empty.vertic, &(p->empty.vertic));
	normer_vecteur(&(p->empty.cible), &(p->empty.cible));
	normer_vecteur(&(p->empty.gauche), &(p->empty.gauche));
	normer_vecteur(&(p->empty.vertic), &(p->empty.vertic));
	
	calculer_theta_z_empty(&(p->empty));
	tourner_perso_z(p, -p->empty.theta_z);
	
	calculer_theta_z_empty(&(c->empty));
	/* Ici la caméra et le perso sont entièrement paramétrés. */
	
}



void deplacer_perso_selon_vitesse_globale(struct perso*p, float dt){
	
	struct vecteur deplacement;
	
	init_vecteur(&deplacement,
		     p->vitesse.x*dt,
		     p->vitesse.y*dt,
		     p->vitesse.z*dt);
	deplacer_perso_global(p, deplacement.x, deplacement.y, deplacement.z);
	deplacer_cam_global(p->la_cam, deplacement.x, deplacement.y, deplacement.z);
}


void placer_perso_au_contact(struct perso*p, struct cube*c){
	
	/* TODO */
	
	
	
}


/* On appelle cette fonction pour modifier la vitesse du perso lorsqu'il est en contact avec un cube.
   Dans la version finale, il faudra prendre en compte la forme cubique de la zone d'influence du cube;
   pour l'instant, on considère la collision entre deux sphères. */
void calculer_rebond_perso(struct perso*p, struct vecteur*vect_n, struct vecteur*ux, float coef_rebond, float coef_frott){
	
	/* La composante selon ux de la vitesse est amortie avec le coefficient coeff_frott;
	   coeff_frott=0 <-> pas de modification;
	   coeff_frott=0 <-> vitesse tangentielle totalement anéantie.
	   La composante normale est amortie selon coef_rebond, mais elle change de signe;
	   Il faut que coef_rebond soit positif.
	   
	   
	   v1 = cfrott*(v0> . ux>).ux>   -   creb*(v0> . n>).n>
	   .    \------ comp_ux ----/        \--- comp_n ----/
	*/
	
	struct vecteur comp_ux, comp_n;
	struct vecteur*v0=&(p->vitesse);
	
	float val_comp_ux=coef_frott*produit_scalaire(*v0, *ux);
	float val_comp_n =-coef_rebond*produit_scalaire(*v0, *vect_n);
	
	/* On initialise les valeurs des vecteurs comp_ux et comp_z; */
	mult_vect_scal(ux, val_comp_ux, &comp_ux);
	mult_vect_scal(vect_n, val_comp_n, &comp_n);
	
	/* On fait la somme des deux vecteurs pour obtenir la nouvelle vitesse. */
	somme_vecteurs(comp_ux, comp_n, v0);
}




			
			
void deplacer_perso(struct perso*p, int*tab_appuis, float dt){
	
	/* Le perso tourne sur lui-même selon les mouvements de la souris.
	   La caméra se déplace en restant centrée sur le perso.
	   Le perso se déplace selon les touches du clavier en entraînant la caméra. */
	
	struct vecteur nouvelle_vitesse_locale;
	convertir_vitesse_globale_vers_locale(p, &nouvelle_vitesse_locale);
	calculer_vitesse_perso(p, tab_appuis, dt, &nouvelle_vitesse_locale);
	appliquer_vitesse_perso(p, dt, &nouvelle_vitesse_locale);
}


/* Appliquer la vitesse locale nouvellement calculée, et faire bouger le perso. */
void appliquer_vitesse_perso(struct perso*le_perso, float dt, struct vecteur*v_locale){
	
	/* Il faut convertir cette vitesse locale pour l'exprimer dans le repère global,
	   puisque la vitesse du perso est exprimée dans le repère global. */
	float dx_glob;
	float dy_glob;
	float dz_glob;
	
	/* Donner au perso la valeur de sa vitesse exprimée dans le repère global. */
	convertir_vitesse_locale_vers_globale(le_perso, v_locale);
	dx_glob=le_perso->vitesse.x*dt;
	dy_glob=le_perso->vitesse.y*dt;
	dz_glob=le_perso->vitesse.z*dt;
	
	deplacer_perso_global(le_perso, dx_glob, dy_glob, dz_glob);
}
