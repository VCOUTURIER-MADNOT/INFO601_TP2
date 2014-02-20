#ifndef _CONSTANTES_
#define _CONSTANTES_

	/* Nombre maximum de fils */
	#define NB_MAX_FILS 30

	/* Taille maximum du message */
	#define TAILLE_MAX_MESSAGE 255

	/* Constantes pour les tubes */
	#define TUBE_LECTURE  0
	#define TUBE_ECRITURE 1

	/* Constantes pour l'affichage */
	#define ERREUR "\033[31;01m[Erreur]\033[31;00m - "
	#define INFO "\033[34;01m[Info]\033[34;00m - "
	#define WARNING "\033[33;01m[Attention]\033[33;00m - "
	#define ACTION "\033[32;01m[Action]\033[32;00m - "

	/* Nom du tube nommé entre le donneur d'ordre et le coeur */
	#define NOM_TUBE "tube_order_coeur"

	#define _POSIX_C_SOURCE 200809L

#endif