#ifndef _FILS_
#define _FILS_

	#include "constantes.h"
	#include <signal.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <errno.h>
	#include <fcntl.h>
	#include "liste.h"
	#include <string.h>


	typedef struct fils_type
	{
		pid_t pid;
		int tube[2];
		List* connexions;
	}fils_t;

	void filsDemarrer(int, int, int, List*);
	void stop(int, siginfo_t*, void*);

	void lirePere(int, List*, int);
	void lireFreres(List*);
	void lireMessage(Cell*);
	void ouvrirConnexionFrere(int, List*);
	void supprimerConnexionFrere(int, List*);
	void envoyerMessage(int, List*, int);
#endif