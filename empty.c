/* Le type empty permet de décrire un point muni de trois axes, en 3d,
   qui définissent un repère quelconque de l'espace.
   On peut effectuer des transformations sur un empty:
   translation, rotations.
   
   Le perso et la caméra viendront se fixer chacun sur un empty.
*/

struct cube;
struct empty;
void empty_to_string(struct empty*e);
void cloner_empty(struct empty*src, struct empty*dest);

/* L'empty contient les neuf variables dont on a besoin:
   La position du point de vue, du point observé
   et du point définissant la verticale de l'empty. */
struct empty{
	
	/* Origine de la caméra (point de vue). */
	struct vecteur pos;
	
	/* Les trois vecteurs qui définissent le référentiel lié à la caméra.
	   Lors d'une translation, ces vecteurs ne seront pas modifiés.
	   Lors d'une rotation, il faudra faire tourner ces vecteurs autour du centre (le point d'observation).
	   
	   NB: le vecteur gauche sera calculé comme le produit vectoriel (vertic^cible).
	   
	   NB: Attention, les vecteurs ne sont pas forcément normés!!!
	*/
	
	/* Vecteur qui va de l'origine de la caméra au point observé par la caméra.
	   La caméra ne regarde donc pas le point (cible.x, cible y, cible.z),
	   mais le point (cam.x+cible.x, ...);
	*/
	struct vecteur cible;
	
	/* Vecteur qui définit la verticale locale à la caméra. */
	struct vecteur vertic;
	
	/* Vecteur qui est calculé à partir des autres et qui porte l'axe y du repère local à la caméra. */
	struct vecteur gauche;
	
	/* On mémorise l'angle de rotation autour de l'axe Z. */
	float theta_z;
	
};




/* Initialiser un empty déjà existant avec des valeurs passées en paramètres.
   Cette fonction n'alloue pas d'espace mémoire. */
void init_empty(struct empty*e,
		float xe, float ye, float ze, /* Centre de l'empty. */
		float xCible, float yCible, float zCible, /* Coordonnées du point observé, exprimées par rapport au point de vue. */
		float xVertic, float yVertic, float zVertic){
	
	
	init_vecteur(&(e->pos), xe, ye, ze);
		
	init_vecteur(&(e->cible), xCible-xe, yCible-ye, zCible-ze);
	/* 
	   afficher_vecteur(&e->cible);
	*/
	init_vecteur(&(e->vertic), xVertic, yVertic, zVertic);
	
	produit_vectoriel(e->vertic, e->cible, &(e->gauche)); /* gauche=vertic^cible,
								 donc maintenant gauche est:
								 1) orthogonal à cible;
								 2) orthogonal à vertic. */
	
	produit_vectoriel(e->cible, e->gauche, &(e->vertic)); /* vertic=cible^gauche,
								 donc maintenant vertic est orthogonal à gauche et à cible.
								 
								 Donc le repère (cible, gauche, vertic) est orthogonal et direct. */
}

void cloner_empty(struct empty*src, struct empty*dest){
	init_empty(dest,
		   src->pos.x, src->pos.y, src->pos.z,
		   src->cible.x, src->cible.y, src->cible.z,
		   src->vertic.x, src->vertic.y, src->vertic.z);
}


void calculer_theta_z_empty(struct empty*e){
	float norme_cible=get_norme_vecteur(&(e->cible));
	struct vecteur x0;
	init_vecteur(&x0, 1, 0, 0);
	if(produit_scalaire(e->cible, x0) >= 0){
		/* L'angle est compris entre -Pi/2 et +Pi/2. */
		e->theta_z=asin((e->cible).y/norme_cible);
	}
	else{
		/* L'angle est compris entre -Pi et -Pi/2, ou entre +Pi/2 et +Pi. */
		e->theta_z=PI/2-asin((e->cible).y/norme_cible);
	}
}



/* Déplacer la caméra selon le vecteur (dx, dy, dz), exprimé dans le repère lié à la cam. */
void deplacer_empty_local(struct empty*e, float dx, float dy, float dz){
	/* La caméra se situe à une position donnée, notée A dans ce commentaire.
	   Elle observe un point noté ici B, et elle est positionnée
	   en rotation autour de l'axe (AB) pour que le point C soit à sa verticale.
	   
	   Il faut donc calculer les vecteurs AB, AC et (-AB^AC)
	   sur lesquels on se déplacera respectivement des distances dx, dy et dz. */
	
	
	/* 1) Faire avancer la caméra d'une longueur dx selon la direction du vecteur c->cible. */
	incrementer_vecteur(&(e->pos), dx, e->cible);
	
	/* 2) Faire avancer la caméra d'une longueur dy selon la direction du vecteur -(c->cible ^ c->vertic). */
	incrementer_vecteur(&(e->pos), dy, e->gauche);
	
	/* 3) Faire avancer la caméra d'une longueur dz selon la direction du vecteur c->vertic. */
	incrementer_vecteur(&(e->pos), dz, e->vertic);
}


/* Déplacer la caméra selon le vecteur (dx, dy, dz) exprimé dans le repère global lié au terrain. */
void deplacer_empty_global(struct empty*e, float dx, float dy, float dz){
	struct vecteur v;
	v.x=dx;
	v.y=dy;
	v.z=dz;
	somme_vecteurs(e->pos, v, &(e->pos));
}


/* Faire tourner l'empty autour de son centre, selon l'axe des cotes (z).
   On modifie tous les vecteurs de l'empty, sauf sa position. */
void tourner_empty_z(struct empty*e, float angle){
	/* Il faut faire tourner autour de l'axe de rotation
	   chacun des trois vecteurs qui caractérisent l'empty. */
	
	float x, y, angle_final;
	
	/* 1) Cible. */
	x=e->cible.x;
	y=e->cible.y;

	e->cible.x = x*cos(angle)-y*sin(angle);
	e->cible.y = x*sin(angle)+y*cos(angle);
	
	/* 2) Vertic. */
	x=e->vertic.x;
	y=e->vertic.y;
	
	e->vertic.x=x*cos(angle)-y*sin(angle);
	e->vertic.y=x*sin(angle)+y*cos(angle);
	
	/* 3) Gauche. */
	x=e->gauche.x;
	y=e->gauche.y;
	
	e->gauche.x=x*cos(angle)-y*sin(angle);
	e->gauche.y=x*sin(angle)+y*cos(angle);
	
	angle_final=e->theta_z+angle;
	
	if(angle_final>2*PI){
		angle_final=angle_final-2*PI;
	}
	else if(angle_final<0){
		angle_final=angle_final+2*PI;
	}
	e->theta_z=angle_final;
}
	





void empty_to_string(struct empty*e){
	printf("empty{\n\tpos=");
	vecteur_to_string(&(e->pos));
	printf("\n\tcible=");
	vecteur_to_string(&(e->cible));
	printf("\n\tvertic=");
	vecteur_to_string(&(e->vertic));
	printf("\n");
}




float distance_entre_emptys(struct empty*e0, struct empty*e1){
	float x0=e0->pos.x;
	float y0=e0->pos.y;
	float z0=e0->pos.z;
	float x1=e1->pos.x;
	float y1=e1->pos.y;
	float z1=e1->pos.z;
	
	return sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) + (z1-z0)*(z1-z0));
}






/* Faire tourner la caméra autour de son axe 'gauche';
   cela revient à regarder plus haut ou plus bas,
   donc à faire tourner la cible dans le plan qui contient
   l'œil, la cible et la verticale.
   
   On va donc exprimer les nouvelles valeurs de cible et vertic en fonction de leurs anciennes valeurs,
   sans modifier pos ou gauche.
*/
void tourner_empty_y_local(struct empty*e, float angle){
	
	struct vecteur ancien_cible, ancien_vertic;
	
	float cosa=cos(angle);
	float sina=sin(angle);
	
	/* Les vecteurs cible et vertic ne sont pas normés.
	   Pour faciliter la rotation, on les norme avant de faire tourner l'empty,
	   donc on mémorise leur norme pour la leur rendre à la fin de l'opération. */
	float norme_cible=get_norme_vecteur(&(e->cible));
	float norme_vertic=get_norme_vecteur(&(e->vertic));
	
	normer_vecteur(&(e->cible), &(e->cible));
	normer_vecteur(&(e->vertic), &(e->vertic));
	
	/* On travaille maintenant sur des vecteurs normés. */
	
	clone_vecteur(e->cible, &ancien_cible);
	clone_vecteur(e->vertic, &ancien_vertic);
	
	/* On calcule les nouvelles valeurs:
	   cible  = - vertic*sin() + cible*cos();
	   vertic = vertic*cos() + cible*sin();
	*/
	
	init_vecteur(&(e->cible),
		     -ancien_vertic.x*sina + ancien_cible.x*cosa,
		     -ancien_vertic.y*sina + ancien_cible.y*cosa,
		     -ancien_vertic.z*sina + ancien_cible.z*cosa);
	
	init_vecteur(&(e->vertic),
		     ancien_vertic.x*cosa + ancien_cible.x*sina,
		     ancien_vertic.y*cosa + ancien_cible.y*sina,
		     ancien_vertic.z*cosa + ancien_cible.z*sina);
	
	/* On peut ici rendre leur norme initiale aux vecteurs. */
	mult_vect_scal(&(e->cible), norme_cible, &(e->cible));
	mult_vect_scal(&(e->vertic), norme_vertic, &(e->vertic));
}






/* Afficher les trois axes de l'empty. */
void afficher_empty(struct empty*e){
	
	/* On affiche des triangles isocèles dont la base est centrée sur l'origine du triangle
	   et dont le sommet est sur les extrémités de l'empty. */
	
	glBegin(GL_TRIANGLES);
	/* printf("afficher_empty %f, %f, %f\n",
	   e->pos.x, e->pos.y, e->pos.z);
	*/
	/* Premier vecteur, formé avec la cible de l'empty. */
	glColor4ub(255, 255, 255, 0);
	/* Premier vecteur, premier triangle. */
	glVertex3d(e->pos.x+e->cible.x,
		   e->pos.y+e->cible.y,
		   e->pos.z+e->cible.z);
	glVertex3d(e->pos.x+e->gauche.x/10,
		   e->pos.y+e->gauche.y/10,
		   e->pos.z+e->gauche.z/10);
	glVertex3d(e->pos.x-e->gauche.x/10,
		   e->pos.y-e->gauche.y/10,
		   e->pos.z-e->gauche.z/10);
	/* Premier vecteur, deuxième triangle (orthogonal au premier). */
	glVertex3d(e->pos.x+e->cible.x,
		   e->pos.y+e->cible.y,
		   e->pos.z+e->cible.z);
	glVertex3d(e->pos.x+e->vertic.x/10,
		   e->pos.y+e->vertic.y/10,
		   e->pos.z+e->vertic.z/10);
	glVertex3d(e->pos.x-e->vertic.x/10,
		   e->pos.y-e->vertic.y/10,
		   e->pos.z-e->vertic.z/10);

	/* Deuxième vecteur, formé avec la gauche de l'empty. */
	glColor4ub(255, 255, 255, 0);
	/* Deuxième vecteur, premier triangle. */
	glVertex3d(e->pos.x+e->gauche.x,
		   e->pos.y+e->gauche.y,
		   e->pos.z+e->gauche.z);
	glVertex3d(e->pos.x+e->cible.x/10,
		   e->pos.y+e->cible.y/10,
		   e->pos.z+e->cible.z/10);
	glVertex3d(e->pos.x-e->cible.x/10,
		   e->pos.y-e->cible.y/10,
		   e->pos.z-e->cible.z/10);
	/* Deuxième vecteur, deuxième triangle (orthogonal au premier). */
	glVertex3d(e->pos.x+e->gauche.x,
		   e->pos.y+e->gauche.y,
		   e->pos.z+e->gauche.z);
	glVertex3d(e->pos.x+e->vertic.x/10,
		   e->pos.y+e->vertic.y/10,
		   e->pos.z+e->vertic.z/10);
	glVertex3d(e->pos.x-e->vertic.x/10,
		   e->pos.y-e->vertic.y/10,
		   e->pos.z-e->vertic.z/10);

	/* Troisième vecteur, formé avec la vertic de l'empty. */
	glColor4ub(255, 255, 255, 0);
	/* Troisième vecteur, premier triangle. */
	glVertex3d(e->pos.x+e->vertic.x,
		   e->pos.y+e->vertic.y,
		   e->pos.z+e->vertic.z);
	glVertex3d(e->pos.x+e->cible.x/10,
		   e->pos.y+e->cible.y/10,
		   e->pos.z+e->cible.z/10);
	glVertex3d(e->pos.x-e->cible.x/10,
		   e->pos.y-e->cible.y/10,
		   e->pos.z-e->cible.z/10);
	/* Troisième vecteur, deuxième triangle (orthogonal au premier). */
	glVertex3d(e->pos.x+e->vertic.x,
		   e->pos.y+e->vertic.y,
		   e->pos.z+e->vertic.z);
	glVertex3d(e->pos.x+e->gauche.x/10,
		   e->pos.y+e->gauche.y/10,
		   e->pos.z+e->gauche.z/10);
	glVertex3d(e->pos.x-e->gauche.x/10,
		   e->pos.y-e->gauche.y/10,
		   e->pos.z-e->gauche.z/10);
	
	
	glEnd();
}
