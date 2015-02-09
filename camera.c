


/* Cette fonction est définie dans "terrain.c". */
int terrain_contient_point(struct terrain*t, struct vecteur*pos);

/* Cette fonction est définie dans le fichier "perso.c". */
void attacher_cam_a_perso(struct perso*p, struct camera*c, float distance);

	
void deplacer_cam_local(struct camera*c, float dx, float dy, float dz){
	deplacer_empty_local(&(c->empty), dx, dy, dz);
}
	
void deplacer_cam_global(struct camera*c, float dx, float dy, float dz){
	/* afficher_empty(&(c->empty)); */
	deplacer_empty_global(&(c->empty), dx, dy, dz);
	/* afficher_empty(&(c->empty)); */
}

/* Déplacer la caméra d'une certaine distance selon l'axe des abscisses, des ordonnées et des cotes de l'empty passé en paramètre. */
void _deplacer_camera_selon_empty(struct camera*c, struct empty*e, float dx, float dy, float dz){
	
	struct vecteur direction;
	/* 1) Abscisses de l'empty. */
	clone_vecteur(e->cible, &direction);
	normer_vecteur(&direction, &direction);
	deplacer_empty_global(&(c->empty), dx*direction.x, dx*direction.y, dx*direction.z);
	/* 2) Ordonnées de l'empty. */
	clone_vecteur(e->gauche, &direction);
	normer_vecteur(&direction, &direction);
	deplacer_empty_global(&(c->empty), dy*direction.x, dy*direction.y, dy*direction.z);
	/* 3) Cotes de l'empty. */
	clone_vecteur(e->vertic, &direction);
	normer_vecteur(&direction, &direction);
	deplacer_empty_global(&(c->empty), dz*direction.x, dz*direction.y, dz*direction.z);
	
}


	
/* Faire tourner la caméra autour de la cible de son empty, càd autour du personnage.
 */
void tourner_cam_z(struct camera*c, float angle){
	/* On avance la caméra pour que la position de l'empty soit sur le perso; */
	somme_vecteurs(c->empty.pos, c->empty.cible, &(c->empty.pos));
	/* On fait tourner l'empty autour de sa position; cela doit modifier tous les vecteurs de l'empty sauf sa position. */
	tourner_empty_z(&(c->empty), angle);
	/* On recule la caméra (dans une autre direction que celle où on l'a avancée puisqu'elle a tourné entre-temps),
	   de manière à recaler la cible de la caméra sur le perso. */
	diff_vecteurs(c->empty.pos, c->empty.cible, &(c->empty.pos));
	
	c->empty.theta_z=c->empty.theta_z+angle;
}

/* Faire tourner la caméra autour de la cible de son empty. */
void tourner_cam_y_local(struct camera*c, float angle){
	
	/* On avance la caméra jusqu'à la position actuelle de sa cible. */
	somme_vecteurs(c->empty.pos, c->empty.cible, &(c->empty.pos));
	/* On fait tourner la caméra autour de son centre provisoire. */
	tourner_empty_y_local(&(c->empty), angle);
	/* On replace la cible là où elle était avant les deux mouvements précédents. */
	diff_vecteurs(c->empty.pos, c->empty.cible, &(c->empty.pos));
}


void avancer_camera_vers_perso_selon_molette(struct camera*c, int*niveau_molette){
	float fact=0.9;
	float d0=c->distance_a_perso;
	if(*niveau_molette>0){
		while(*niveau_molette>0){
			c->distance_a_perso=(c->distance_a_perso)*fact;
			(*niveau_molette)--;
		}
	}
	else{
		while(*niveau_molette<0){
			c->distance_a_perso=(c->distance_a_perso)/fact;
			(*niveau_molette)++;
		}
	}
	
	
	deplacer_cam_local(c, d0-c->distance_a_perso, 0, 0);
	normer_vecteur(&(c->empty.cible), &(c->empty.cible));
	mult_vect_scal(&(c->empty.cible), c->distance_a_perso, &(c->empty.cible));
}


void _cam_to_string(struct camera*c){
	printf("cam{\n");
	empty_to_string(&(c->empty));
	printf("\n}");
}
