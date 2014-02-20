#include "fils.h"

int continuer = 1;
int num = -1;

void stop(int numSig, siginfo_t * info, void* rien)
{
	continuer = 0;
}

void filsDemarrer(int numR, int fdLect, int fdEcr, List *connexions)
{
	int flagsPere, resSig;

	sigset_t ensemble;
	siginfo_t info;
	struct timespec timer;

	sigemptyset(&ensemble);
	sigaddset(&ensemble, SIGRTMIN);
	sigprocmask(SIG_BLOCK, &ensemble, NULL);
	timer.tv_sec = 0;
	timer.tv_nsec = 1;

	num = numR;

	/* Fermeture de la connexion Fils vers père */
	if(close(fdEcr) == -1)
	{
		fprintf(stderr, ERREUR "Fils n°%d - Impossible de fermer le tube en écriture\n", num);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* Récupération des flags actuels du fd de la connexion père vers fils */
	if((flagsPere = fcntl(fdLect, F_GETFL)) == -1)
	{
		fprintf(stderr, ERREUR "Fils n°%d - Impossible de récuperer les flags\n", num);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* On rend la lecture non bloquante */
	if(fcntl(fdLect, F_SETFL, flagsPere | O_NONBLOCK) == -1)
	{
		fprintf(stderr, ERREUR "Fils n°%d - Impossible de modifier les flags\n", num);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	printf(INFO"Fils n°%d - Démarré.\n", num);
	while(continuer)
	{
		/* Si on reçoit une commande venant du père */
		lirePere(fdLect, connexions, flagsPere);
		/* On regarde si on a reçu un signal */

		/* Si on reçoit une commande d'un frère */
		lireFreres(connexions);
		if((resSig = sigtimedwait(&ensemble, &info, &timer)) == -1)
		{
			if(errno != EAGAIN)
			{
				fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de l'attente d'un signal\n", num);
				perror(" ");
				exit(EXIT_FAILURE);
			}
		}
		if(resSig == SIGRTMIN)
		{
			stop(resSig, &info, NULL);
		}
	}

	if(close(fdLect) == -1)
	{
		fprintf(stderr, ERREUR "Fils n°%d - Erreur lors de la fermeture du tube en lecture\n", num);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	printf(INFO"Fils n°%d - Exécution terminé, je rends l'âme!\n", num);
	exit(EXIT_SUCCESS);
}

void lirePere(int fdLect, List *connexions, int flags)
{
	char cmde;
	int tailleLect, destinataire;

	tailleLect = read(fdLect, &cmde, sizeof(char));
	if(tailleLect == -1 && (errno != EAGAIN))
	{
		fprintf(stderr, ERREUR"Fils n°%d - Impossible de lire dans le tube venant du coeur\n", num );
		perror(" ");
		exit(EXIT_FAILURE);
	}
	else if(tailleLect == 1)
	{
		if(fcntl(fdLect, F_SETFL, flags) == -1)
		{
			fprintf(stderr, ERREUR "Fils n°%d - Impossible de modifier les flags\n", num);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		switch(cmde)
		{
			/* Ajout d'une connexion */
			case 'A':
				if(read(fdLect, &destinataire, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de la lecture du numéro du correspondant\n", num);
					perror(" ");
					exit(EXIT_FAILURE);
				}
				ouvrirConnexionFrere(destinataire, connexions);
				printf(INFO"Fils n°%d - Connexion avec le fils n°%d ajoutée.\n", num, destinataire);
				break;
			/* Suppression d'une connexion */
			case 'S':
				if(read(fdLect, &destinataire, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de la lecture du numéro du correspondant\n", num);
					perror(" ");
					exit(EXIT_FAILURE);
				}
				supprimerConnexionFrere(destinataire, connexions);
				printf(INFO"Fils n°%d - Connexion avec le fils n°%d supprimée.\n", num, destinataire);
				break;
			/* Envoi d'un message */
			case 'M':
				if(read(fdLect, &destinataire, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de la lecture du numéro du correspondant\n", num);
					perror(" ");
					exit(EXIT_FAILURE);
				}
				envoyerMessage(destinataire, connexions, fdLect);
				break;
			default :
				printf(INFO"Fils n°%d - La commande reçu n'est pas une commande valide\n", num);
				break;
		}
		if(fcntl(fdLect, F_SETFL, flags | O_NONBLOCK) == -1)
		{
			fprintf(stderr, ERREUR "Fils n°%d - Impossible de modifier les flags\n", num);
			perror(" ");
			exit(EXIT_FAILURE);
		}
	}


}

void ouvrirConnexionFrere(int dest, List *connexions)
{
	Cell *cellule = NULL;
	char temp[15];
	cellule = rechercher(connexions, dest);
	if(cellule == NULL)
	{
		cellule = creerCellule(dest);
		inserer(connexions, cellule);
	}
	cellule->fd[TUBE_ECRITURE] = 0;
	if(num < dest)
	{
		sprintf(temp, "tube_%d_%d", num, dest);
		if((cellule->fd[TUBE_ECRITURE] = open(temp, O_WRONLY)) == -1)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de l'ouverture en ecriture du tube '%s'\n", num, temp);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		sprintf(temp, "tube_%d_%d", dest, num);
		if((cellule->fd[TUBE_LECTURE] = open(temp, O_RDONLY)) == -1)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de l'ouverture du '%s'\n", num, temp);
			perror(" ");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		sprintf(temp, "tube_%d_%d", dest, num);
		if((cellule->fd[TUBE_LECTURE] = open(temp, O_RDONLY)) == -1)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de l'ouverture en lecture du tube '%s'\n", num, temp);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		sprintf(temp, "tube_%d_%d", num, dest);
		if((cellule->fd[TUBE_ECRITURE] = open(temp, O_WRONLY)) == -1)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de l'ouverture du tube '%s'\n", num, temp);
			perror(" ");
			exit(EXIT_FAILURE);
		}
	}
}

void supprimerConnexionFrere(int dest, List *connexions)
{
	Cell *cellule = NULL;
	char temp[15];

	cellule = rechercher(connexions, dest);

	if(cellule == NULL)
	{
		printf(ERREUR"Fils n°%d - Impossible de supprimer la connexion avec le destinataire n°%d\n", num, dest);
	}
	else
	{
		sprintf(temp, "tube_%d_%d", num, dest);
		if(close(cellule->fd[TUBE_ECRITURE]) == -1)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de la fermeture en ecriture du tube '%s'\n", num, temp);
			perror(" ");
			exit(EXIT_FAILURE);
		}

		sprintf(temp, "tube_%d_%d", dest, num);
		if(close(cellule->fd[TUBE_LECTURE]) == -1)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de la fermeture en lecture du tube '%s'\n", num, temp);
			perror(" ");
			exit(EXIT_FAILURE);
		}

		supprimer(connexions, cellule);
	}
}

void envoyerMessage(int dest, List* connexions, int fdLect)
{
	int tailleBuf, res;
	char * buffer;
	Cell * cellule;

	/* Réception du message en provenance du père */
	if(read(fdLect, &tailleBuf, sizeof(int)) == -1)
	{
		fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de la lecture de la taille du message\n", num);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	buffer = (char*) malloc(sizeof(char) * tailleBuf);
	if(read(fdLect, buffer, sizeof(char) * tailleBuf) == -1)
	{
		fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de la lecture du message\n", num);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	/* On récupère les informations sur le destinataire (les fd) */
	cellule = rechercher(connexions, dest);
	if(cellule != NULL)
	{
		/* On envoie le message */
		if(write(cellule->fd[TUBE_ECRITURE], &tailleBuf, sizeof(int)) == -1)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de l'envoi de la taille du message\n", num);
			perror(" ");
			exit(EXIT_FAILURE);
		}

		if(write(cellule->fd[TUBE_ECRITURE], buffer, sizeof(char) * tailleBuf) == -1)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de l'envoi du message\n", num);
			perror(" ");
			exit(EXIT_FAILURE);
		}

		/* On attends l'accusé de réception */
		if(read(cellule->fd[TUBE_LECTURE], &res, sizeof(int)) == -1)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de la lecture de l'accusé de réception\n", num);
			perror(" ");
			exit(EXIT_FAILURE);
		}

		printf(INFO"Fils n°%d - Accusé de réception venant du fils n°%d bien reçu\n", num, dest);	
	}
	else
	{
		printf(WARNING"Fils n°%d - Il n'existe pas de connexion avec le fils n°%d\n", num, dest);
	}

	free(buffer);
}

void lireFreres(List* connexions)
{
	Cell *cell = NULL;

	cell = connexions->cell;
	while(cell != NULL)
	{
		lireMessage(cell);
		cell = cell->next;
	}
}

void lireMessage(Cell *emetteur)
{
	int tailleBuf = 0, flags, res, rep = 1;
	char* buffer;
	/* Récupération des flags actuels du fdLect de la connexion entre les deux fils */
	if((flags = fcntl(emetteur->fd[TUBE_LECTURE], F_GETFL)) == -1)
	{
		fprintf(stderr, ERREUR "Fils n°%d - Impossible de récuperer les flags\n", num);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* On rend la lecture non bloquante */
	if(fcntl(emetteur->fd[TUBE_LECTURE], F_SETFL, flags | O_NONBLOCK) == -1)
	{
		fprintf(stderr, ERREUR "Fils n°%d - Impossible de modifier les flags\n", num);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	res = read(emetteur->fd[TUBE_LECTURE], &tailleBuf, sizeof(int));
	if(res == -1)
	{
		if(errno != EAGAIN)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Impossible de lire la taille du message provenant du fils n°%d\n",num, emetteur->num);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		else
		{
			if(fcntl(emetteur->fd[TUBE_LECTURE], F_SETFL, flags) == -1)
			{
				fprintf(stderr, ERREUR "Fils n°%d - Impossible de modifier les flags\n", num);
				perror(" ");
				exit(EXIT_FAILURE);
			}
		}
	}
	else if(res != sizeof(int) && (errno != EAGAIN))
	{
		printf(INFO"Fils n°%d - Le message reçu venant du fils n°%d ne correspond pas\n", num, emetteur->num);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	else
	{
		if(fcntl(emetteur->fd[TUBE_LECTURE], F_SETFL, flags) == -1)
		{
			fprintf(stderr, ERREUR "Fils n°%d - Impossible de modifier les flags\n", num);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		buffer = (char*) malloc(sizeof(char) * tailleBuf);
		if(read(emetteur->fd[TUBE_LECTURE], buffer, sizeof(char)*tailleBuf) == -1)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de la lecture du message venant du fils n°%d\n", num, emetteur->num);
			perror(" ");
			exit(EXIT_FAILURE);
		}

		printf(INFO"Fils n°%d - Le fils n°%d m'a envoyé le message suivant :\n\t'%s'\n", num, emetteur->num, buffer);

		if(write(emetteur->fd[TUBE_ECRITURE], &rep, sizeof(int)) == -1)
		{
			fprintf(stderr, ERREUR"Fils n°%d - Erreur lors de l'envoi de l'accusé de réception\n", num);
			perror(" ");
			exit(EXIT_FAILURE);
		}
	}
}