#ifndef _COEUR_
#define _COEUR_

	#include "constantes.h"
	#include <sys/types.h> /* pid_t */
	#include <signal.h> /* sigaction, sigqueue... */
	#include <unistd.h> /* fork, pause */
	#include <stdlib.h> /* exit */
	#include <stdio.h> /* printf */
	#include <sys/stat.h> /* mkfifo */
	#include <sys/wait.h> /* wait */
	#include <errno.h> /* errno */
	#include "fils.h"
	#include "cellule.h"
	#include "liste.h"
	#include "celluleVide.h"
	#include "listeVide.h"


	void coeur();
	fils_t* creerFils(int);
	void supprimerFils(int, fils_t**, ListeVide*);
	void ajoutConnexion(fils_t **, int, int);
	void supprimerConnexion(fils_t **, int, int);
	void distribuerMessage(fils_t **, int, int, char*);
	void sauverEtat(char*, fils_t**, int);
	void stoperCoeur(fils_t **, ListeVide *, int);
	int chargerEtat(char*, fils_t**, ListeVide*);

#endif