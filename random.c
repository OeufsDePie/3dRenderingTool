#include<time.h>
#include<stdlib.h>


/* Quand on utilise des fonctions définies dans ce fichier,
   il faut, au début du code, exécuter l'instruction suivante:
   srand(time(NULL));
*/



/************************************************************************/
/* Générer un nombre entier aléatoire entre deux entiers a et b donnés; */
/* Les valeurs générées appartiennent à l'intervalle [a; b[             */
/************************************************************************/
int random_int_a_b(int a, int b){
	int res=rand()%(b-a) + a ;

	return res;
}

/************************************************************************/
/* Fonction qui retourne un flottant aléatoire dans l'intervalle [a,b]. */
/************************************************************************/
float random_float_a_b(float a, float b){
  
	float x = 0.00001*(float)( random_int_a_b( 100000*a, 100000*b ) ); /* flottant entre a et b, avec 5 chiffres non nuls après la virgule. */

	return x;
}
  
