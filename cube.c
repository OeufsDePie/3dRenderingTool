
struct cube;
void cube_to_string(struct cube*c);
void init_cube(struct cube*c, float xP, float yP, float zP, float t);

struct cube{
	
	/* Centre. */
	float x, y, z;
	
	/* Côté. */
	float taille;
	
	/* Couleur. */
	/* NB: il faudrait que chaque face ait une couleur basée sur la couleur du cube, mais très légèrement différente.
	   Par exemple, on imagine qu'une source de lumière est placée très loin et qu'elle éclaire dans la direction
	   du vecteur {2, 1, -5}. Donc pour chaque cube, la face du haut est très bien éclairée, la face avant l'est un peu moins
	   et la face droite encore moins.
	   Ou alors, on place une source de lumière et on fait du ray tracing sur les sommets des cubes pour savoir comment
	   ils sont éclairés. Ce calcul devra être fait à chaque fois qu'on ajoute ou retire un cube ou une source de lumière.
	*/
	int r, g, b;
	
	/* Un cube peut être existant ou inexistant.
	   NB: à l'avenir, un cube inexistant ne sera pas stocké et le terrain aura une structure modifiable,
	   çad qu'on pourra ajouter ou supprimer des cubes. Pour l'instant on change juste ce paramètre. */
	int existe;
	
	int transparence; /* 0<->opaque, 255<->transparent. */
	
	/* La gravité locale dans tout le cube. */
	struct vecteur gravite;
	
	
	
};


void init_gravite_cube(struct cube*c, float gx, float gy, float gz){
	if(c!=NULL){
		init_vecteur(&(c->gravite), gx, gy, gz);
	}
}
	
	
void changer_couleur_cube(struct cube*c){
	c->r=random_int_a_b(25, 255);
	c->b=random_int_a_b(25, 255);
	c->g=random_int_a_b(25, 255);
}

void init_cube(struct cube*c, float xP, float yP, float zP, float t){
	if(c!=NULL){
		c->x=xP;
		c->y=yP;
		c->z=zP;
		c->taille=t;
		
		changer_couleur_cube(c);
		
		c->transparence=0;
		
	}
}


void afficher_cube(struct cube*c){
	
	/* Coordonnées des points situés en haut du cube, en bas,
	   à gauche, à droite, sur la face avant, sur la face arrière. */
	float h, b, g, d, av, ar;
	float demi_hauteur=c->taille/2;
	
	/* On affiche le cube seulement s'il n'est pas déclaré comme inexistant. */
	if(c->existe){
		

	
		/* Code "normal: on affiche vraiment le cube. */
		/* NB: Pour accélérer l'étape d'affichage, il faudra que chaque cube sache quelles sont
		   les faces qui sont visibles; à chaque affichage, on ne traitera que celles-là.
		   Donc à chaque modification d'un cube du terrain (ajout ou retrait), il faudra
		   mettre à jour ces flags parmi tous les cubes voisins directs. */
		h=c->z+demi_hauteur;
		b=c->z-demi_hauteur;
		d=c->y+demi_hauteur;
		g=c->y-demi_hauteur;
		ar=c->x+demi_hauteur;
		av=c->x-demi_hauteur;
       
		glBegin(GL_QUADS);{
			
			/* Couleur du cube. */
			/* glColor3ub(c->r, c->g, c->b); */
			glColor4ub(c->r, c->g, c->b, c->transparence);
			
			/* Face de droite. */
			glVertex3d(av, d, h);
			glVertex3d(av, d, b);
			glVertex3d(ar, d, b);
			glVertex3d(ar, d, h);
		
			/* Face de gauche. */
			glVertex3d(av, g, h);
			glVertex3d(av, g, b);
			glVertex3d(ar, g, b);
			glVertex3d(ar, g, h);
		
			/* Face du haut. */
			glVertex3d(av, d, h);
			glVertex3d(av, g, h);
			glVertex3d(ar, g, h);
			glVertex3d(ar, d, h);
		
			/* Face du bas. */
			glVertex3d(av, d, b);
			glVertex3d(av, g, b);
			glVertex3d(ar, g, b);
			glVertex3d(ar, d, b);
		
			/* Face avant. */
			glVertex3d(av, d, b);
			glVertex3d(av, g, b);
			glVertex3d(av, g, h);
			glVertex3d(av, d, h);
		
			/* Face arrière. */
			glVertex3d(ar, d, b);
			glVertex3d(ar, g, b);
			glVertex3d(ar, g, h);
			glVertex3d(ar, d, h);
		}
		glEnd();
		
	}
}



void cube_to_string(struct cube*c){
	
	printf("cube{\n\tcentre: {%f, %f, %f}, taille=%f\n}\n", c->x, c->y, c->z, c->taille);
}
