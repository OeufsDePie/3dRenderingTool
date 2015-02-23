
struct vecteur{
	float x, y, z;
};



void somme_vecteurs(struct vecteur u, struct vecteur v, struct vecteur*w);
void diff_vecteurs(struct vecteur u, struct vecteur v, struct vecteur*w);
void clone_vecteur(struct vecteur u, struct vecteur*v);
void rotation_vecteur(struct vecteur*v, float th_x, float th_y, float th_z);
void init_vecteur(struct vecteur*v, float x, float y, float z);
void normer_vecteur(struct vecteur*v, struct vecteur*resultat);
float get_norme_vecteur(struct vecteur*v);
float produit_scalaire(struct vecteur u, struct vecteur v);
void produit_vectoriel(struct vecteur u, struct vecteur v, struct vecteur*w);
void incrementer_vecteur(struct vecteur*v, float distance, struct vecteur direction);
float distance(struct vecteur*u, struct vecteur*v);
void mult_vect_scal(struct vecteur*u, float lambda, struct vecteur*v);
void vecteur_to_string(struct vecteur*v);
int sont_colineaires(struct vecteur*u, struct vecteur*v);
int est_vecteur_nul(struct vecteur*v);
int egalite_vecteurs(struct vecteur*u, struct vecteur*v);


void vecteur_to_string(struct vecteur*v){
	printf("vecteur{%f, %f, %f}",
	       v->x, v->y, v->z);
}


void clone_vecteur(struct vecteur u, struct vecteur*v){
	v->x=u.x;
	v->y=u.y;
	v->z=u.z;
}


/* Faire tourner un vecteur successivement autour des trois axes x, y puis z,
   ces trois rotations seront faites sur les angles th_x, th_y et th_z. */
void rotation_vecteur(struct vecteur*v, float th_x, float th_y, float th_z){
	
	/* Variables temporaires. */
	float x, y, z;
	
	/* Rotation selon l'axe x. */
	
	/* On extrait les valeurs concernées. */
	y=v->y;
	z=v->z;
	/* Puis on calcule les nouvelles valeurs. */
	v->y=y*cos(th_x)-z*sin(th_x);
	v->z=z*cos(th_x)+y*sin(th_x);
	
	
	/* Rotation selon l'axe y. */
	
	/* On extrait les valeurs concernées. */
	z=v->z;
	x=v->x;
	/* Puis on calcule les nouvelles valeurs. */
	v->z=z*cos(th_y)-x*sin(th_y);
	v->x=x*cos(th_y)+z*sin(th_y);
	
	
	/* Rotation selon l'axe z. */
	
	/* On extrait les valeurs concernées. */
	x=v->x;
	y=v->y;
	/* Puis on calcule les nouvelles valeurs. */
	v->x=x*cos(th_z)-y*sin(th_z);
	v->y=y*cos(th_z)+x*sin(th_z);
}



/* Initialiser un vecteur avec des valeurs passées en paramètre. */
void init_vecteur(struct vecteur*v, float x, float y, float z){
	v->x=x;
	v->y=y;
	v->z=z;
}


float get_norme_vecteur(struct vecteur*v){
	float x=v->x;
	float y=v->y;
	float z=v->z;
	return sqrt(x*x+y*y+z*z);
}


void normer_vecteur(struct vecteur*v, struct vecteur*resultat){
	float norme=get_norme_vecteur(v);
	if(norme!=0){
		resultat->x=v->x/norme;
		resultat->y=v->y/norme;
		resultat->z=v->z/norme;
	}
}




/* Calculer le produit vectoriel u^v et stocker le résultat dans w.
   NB: il faut que w corresponde à de la mémoire déjà allouée,
   aucune vérification n'est effectuée ici.
*/
void produit_vectoriel(struct vecteur u, struct vecteur v, struct vecteur*w){
	
	/* Composantes qui seront assignées à w.
	   On ne fait pas les modifs en direct car on veut pouvoir effectuer
	   un calcul du type produit_vectoriel(u, v, u) où l'un des paramètres
	   est repmplacé par le résultat. */
	float x, y, z;
	
	x=u.y*v.z-u.z*v.y;
	y=u.z*v.x-u.x*v.z;
	z=u.x*v.y-u.y*v.x;
	
	w->x=x;
	w->y=y;
	w->z=z;
}



/* Calculer le produit scalaire entre deux vecteurs. */
float produit_scalaire(struct vecteur u, struct vecteur v){
	return u.x*v.x + u.y*v.y + u.z*v.z;
}



/* Calculer la somme de deux vecteurs: w=u+v. */
void somme_vecteurs(struct vecteur u, struct vecteur v, struct vecteur*w){
	w->x=u.x+v.x;
	w->y=u.y+v.y;
	w->z=u.z+v.z;
}

/* Calculer la différence de deux vecteurs: w=u-v. */
void diff_vecteurs(struct vecteur u, struct vecteur v, struct vecteur*w){
	w->x=u.x-v.x;
	w->y=u.y-v.y;
	w->z=u.z-v.z;
}

/* Translater l'exrémité d'un vecteur sur une distance donnée, selon une direction donnée. */
void incrementer_vecteur(struct vecteur*v, float distance, struct vecteur direction){
	
	/* On normalise le vecteur qui donne la direction du déplacement. */
	struct vecteur direction_normee;
	
	normer_vecteur(&direction, &direction_normee);
	
	v->x=v->x+distance*direction_normee.x;
	v->y=v->y+distance*direction_normee.y;
	v->z=v->z+distance*direction_normee.z;
}





float distance(struct vecteur*u, struct vecteur*v){
	struct vecteur w;
	diff_vecteurs(*u, *v, &w);
	return get_norme_vecteur(&w);
}



void mult_vect_scal(struct vecteur*u, float lambda, struct vecteur*v){
	v->x=lambda*u->x;
	v->y=lambda*u->y;
	v->z=lambda*u->z;
}


/* Cette fonction renvoie 1 (vrai) si le vecteur est de norme nulle, 0 (faux) sinon. */
int est_vecteur_nul(struct vecteur*v){
	return v->x==0 && v->y==0 && v->z==0;
}

/* Deux vecteurs sont colinéaires quand leur produit vectoriel est nul. */
int sont_colineaires(struct vecteur*u, struct vecteur*v){
	struct vecteur w;
	produit_vectoriel(*u, *v, &w);
	return(est_vecteur_nul(&w));
}


int egalite_vecteurs(struct vecteur*u, struct vecteur*v){
	return u->x==v->x
		&&
		u->y==v->y
		&&
		u->z==v->z;
}
