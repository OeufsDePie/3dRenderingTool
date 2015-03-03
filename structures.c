struct camera{
	
	struct empty empty;
	struct perso*le_perso;
	/* La caméra se positionne à une certaine distance_std du personnage.
	   Cette distance est modifiable avec le scroll de la souris;
	   Quand la caméra se retrouve coincée dans un mur, on utilise alors distance_a_perso,
	   que l'on réduit jusqu'à ce que la caéra sorte du terrain. */
	float distance_a_perso;
	float distance_compensation_mur;
};

enum type_perso{PIETON, VEHICULE};


struct perso{
	
	/* La variable empty décrit un simple point muni d'axes en 3d,
	   sur lequel on peut appliquer des transformations.
	   Les collisions seront effectuées grâce à une "boîte d'influence",
	   le plus petit parallélépipède rectangle aligné sur les axes du repère
	   et contenant l'ensemble des points.
	*/
	struct empty empty;
	
	/* La vitesse instantanée du personnage.
	   Elle doit toujours être exprimée dans le repère global.
	 */
	struct vecteur vitesse;
	
	/* La valeur de la composante verticale de la vitesse lors du début d'un saut. */
	float vitesse_de_saut;
	/* La vitesse à laquelle le perso se déplace horizontalement. */
	float vitesse_de_mvt;
	
	/* Valeurs de l'accélération maxi du perso, utilisée pour modifier la vitesse. */
	struct vecteur acc;
	
	/* Le personnage connaît la caméra qui lui est attachée. */
	struct camera*la_cam;
	
	/* Type du perso. */
	enum type_perso type;
	
	int r, g, b;
	
	int nb_points, nb_faces;
	
	/* Tableau de points (vertices). C'est un tableau dont chaque élément est un vecteur (donc un triplet de coordonnées). */
	struct vecteur*tab_points;
	int**tab_couleurs;
	/* Tableau de faces triangulaires. C'est un tableau dont chaque élément est la liste des indices des trois sommets. */
	int**tab_faces;
	
	/* Valeurs pour le déplacement et la rotation. */
	float distance_deplacement;
	float angle_rotation;
	
	/* Le personnage est muni d'une capsule de collision, centrée sur l'empty, contenant tous les points du maillage
	   (ou presque tous les points; cela est à développer; penser notamment au principe des points chauds?). */
	float longueur; /* Taille en x et y du pavé englobant. */
	float hauteur; /* Hauteur du pavé englobant. */
	
	/* Rayon de la sphère de collision. */
	float rP;
};
	
