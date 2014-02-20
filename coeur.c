#include "coeur.h"

int continuerCoeur = 1;

void coeur(pid_t parentPID)
{
	int numProchain = 0, 			/* Numéro du prochain fils créé si la table de vide est vide. Il n'existe pas de fils ayant un num plus grand */
	fdOrder,                    	/* File descriptor du tube entre le donneur d'ordre et le coeur */
	recu1, recu2,                	/* Paramètres entiers reçus */
	tailleBuffer;                	/* taille du buffer */
	char *buffer;                	/* Paramètre char* reçu */
	fils_t* tabFils[NB_MAX_FILS] = {NULL};	/* Tableau qui contient les fils */
	ListeVide *listeVide = NULL; 	/* Liste qui contient les numéros des fils qui ont été supprimés afin de réutiliser ces numéros */
	CellVide * vide = NULL;
	union sigval valeur;


	listeVide = creerListeVide();
	if((fdOrder = open(NOM_TUBE, O_RDONLY)) == -1)
	{
		fprintf(stderr, ERREUR"Coeur - Impossible d'ouvrir le tube entre le coeur et le donneur d'ordres en lecture\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	numProchain = chargerEtat("tubeTella.bin", tabFils, listeVide);
	printf("Max = %d\n", numProchain);

	while(continuerCoeur)
	{
		/*printf(INFO"Coeur - boucle\n");
		sleep(1);*/
		if(read(fdOrder, &recu1, sizeof(int)) == -1)
		{
			fprintf(stderr, ERREUR"Coeur - Erreur lors de la lecture de l'ordre\n");
			perror(" ");
			exit(EXIT_FAILURE);
		}

		valeur.sival_int = recu1;
		if(sigqueue(parentPID, SIGRTMIN, valeur) == -1) {
			fprintf(stderr, ERREUR"Coeur - Erreur lors de l'envoi du signal vers le donneur d'ordre\n");
			perror(" ");
			exit(EXIT_FAILURE);
		}
		switch(recu1)
		{
			case 0:
				sauverEtat("tubeTella.bin", tabFils, numProchain);
				stoperCoeur(tabFils, listeVide, numProchain);
				break;
			case 1:
				if(listeVide->cell != NULL)
				{
					vide = listeVide->cell;
					tabFils[vide->num] = creerFils(vide->num);
					supprimerVide(listeVide, vide);
				}
				else
				{
					tabFils[numProchain] = creerFils(numProchain);
					numProchain++;
				}
				break;
			case 2:
				if(read(fdOrder, &recu1, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la lecture de l'ordre\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}
				supprimerFils(recu1, tabFils, listeVide);
				break;
			case 3:
				if(read(fdOrder, &recu1, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la lecture de l'ordre\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}

				if(read(fdOrder, &recu2, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la lecture de l'ordre\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}

				ajoutConnexion(tabFils, recu1, recu2);
				break;
			case 4:
				if(read(fdOrder, &recu1, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la lecture de l'ordre\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}

				if(read(fdOrder, &recu2, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la lecture de l'ordre\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}
				supprimerConnexion(tabFils, recu1, recu2);
				break;
			case 5:
				if(read(fdOrder, &recu1, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la lecture de l'ordre\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}
				if(read(fdOrder, &recu2, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la lecture de l'ordre\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}
				if(read(fdOrder, &tailleBuffer, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la lecture de l'ordre\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}
				buffer = (char*)malloc(sizeof(char) * tailleBuffer);
				if(read(fdOrder, buffer, sizeof(char) * tailleBuffer) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la lecture de l'ordre\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}
				distribuerMessage(tabFils, recu1, recu2, buffer);
				break;
		}
	}

	if(close(fdOrder) == -1)
	{
		fprintf(stderr, ERREUR"Coeur - Impossible de fermer le fd du tube %s\n", NOM_TUBE);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	printf(INFO"Coeur - Arrêt.\n");
	exit(EXIT_SUCCESS);
}

fils_t* creerFils(int num)
{
	fils_t *fils = (fils_t*)malloc(sizeof(fils_t));

	fils->connexions = creerListe();

	/* Création de la connexion entre le fils et le père */
	if(pipe(fils->tube) == -1)
	{
		fprintf(stderr, ERREUR "Coeur - Impossible de créer le tube n°%d\n", num);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* Création du fils */
	if((fils->pid = fork()) == -1)
	{
		fprintf(stderr, ERREUR "Coeur - Impossible de créer le fils n°%d\n", num);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* Démarrage du fils */
	if(fils->pid == 0)
	{
		filsDemarrer(num, fils->tube[TUBE_LECTURE], fils->tube[TUBE_ECRITURE], fils->connexions);
	}

	/* Fermeture de la connexion fils vers père */
	if(close(fils->tube[TUBE_LECTURE]) == -1)
	{
		fprintf(stderr, ERREUR "Coeur - Impossible de fermer le tube n°%d en lecture\n", num);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	return fils;
}

void supprimerFils(int num, fils_t **tabFils, ListeVide *listeVide)
{
	Cell* cell = NULL;
	fils_t *fils = NULL;
	CellVide* cellVide = NULL;
	union sigval valeur;
	valeur.sival_int = 0;
	if(tabFils[num] == NULL)
	{
		printf(WARNING"Coeur - Le fils n°%d n'existe pas.\n", num);
	}
	else
	{
		fils = tabFils[num];
		/* On arrête toutes les connexions entre le fils et ses correspondants */
		cell = fils->connexions->cell;
		while(cell != NULL)
		{
			supprimerConnexion(tabFils, num, cell->num);
			cell = cell->next;
		}
		sleep(1); /* Pour éviter que le fils reçoive le signal pendant la suppression de ses connexions */
		/* Envoi du signal au fils à supprimer */
		if(sigqueue(fils->pid, SIGRTMIN, valeur) == -1)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors de l'envoi du signal temps réel au fils n°%d\n", num);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		/* On ferme la connexion entre le coeur et le fils */
		if(close(fils->tube[TUBE_ECRITURE]) == -1)
		{
			fprintf(stderr, ERREUR "Coeur - Impossible de fermer le tube n°%d en ecriture\n", num);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		/* Attente de que le fils ait fini de s'exécuté */
		if(waitpid(fils->pid, NULL, 0) == -1)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors de l'attente du fils n°%d\n", num);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		cellVide = creerCelluleVide(num);
		insererVide(listeVide, cellVide);
		detruireListe(&((fils)->connexions));
		free(fils);
		tabFils[num] = NULL;
		fils = NULL;
	}
}

void ajoutConnexion(fils_t ** tabFils, int fils1, int fils2)
{
	char tubeName[15];
	char ajout = 'A';
	Cell *connexion, *connexion1, *connexion2;

	if(tabFils[fils1] == NULL)
	{
		printf(WARNING"Coeur - Le fils n°%d n'existe pas.\n", fils1);
		return;
	}
	else if(tabFils[fils2] == NULL)
	{
		printf(WARNING"Coeur - Le fils n°%d n'existe pas.\n", fils2);
		return;
	}

	connexion1 = rechercher(tabFils[fils1]->connexions, fils2);
	connexion2 = rechercher(tabFils[fils2]->connexions, fils1);
	
	if(connexion1 != NULL && connexion2 != NULL)
	{
		printf(WARNING"Coeur - La connexion existe déjà, elle ne sera donc pas recrée.\n");
		return;
	}
	
	/* On crée les connexions entre les 2 fils */
	sprintf(tubeName, "tube_%d_%d", fils1, fils2);
	if(mkfifo(tubeName, S_IRUSR | S_IWUSR) == -1)
	{
		if(errno != EEXIST)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors de la création du tube nommé %s\n", tubeName);
			perror(" ");
			exit(EXIT_FAILURE);	
		}
		else
		{
			fprintf(stderr, WARNING"Coeur - tube nommé %s déjà existant.\n", tubeName);
		}
		
	}

	sprintf(tubeName, "tube_%d_%d", fils2, fils1);
	if(mkfifo(tubeName, S_IRUSR | S_IWUSR) == -1)
	{
		if(errno != EEXIST)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors de la création du tube nommé %s\n", tubeName);
			perror(" ");
			exit(EXIT_FAILURE);	
		}
		else
		{
			fprintf(stderr, WARNING"Coeur - tube nommé %s déjà existant.\n", tubeName);
		}
	}
	/* Informer le fils 1 */
	if(write(tabFils[fils1]->tube[TUBE_ECRITURE], &ajout, sizeof(char)) == -1)
	{
		fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", fils1);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(write(tabFils[fils1]->tube[TUBE_ECRITURE], &fils2, sizeof(int)) == -1)
	{
		fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", fils1);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	/* Informer le fils 2 */
	if(write(tabFils[fils2]->tube[TUBE_ECRITURE], &ajout, sizeof(char)) == -1)
	{
		fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", fils1);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(write(tabFils[fils2]->tube[TUBE_ECRITURE], &fils1, sizeof(int)) == -1)
	{
		fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", fils1);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	connexion = creerCellule(fils2);
	inserer(tabFils[fils1]->connexions, connexion);
	connexion = creerCellule(fils1);
	inserer(tabFils[fils2]->connexions, connexion);
	
	connexion = NULL;
}

void supprimerConnexion(fils_t ** tabFils, int fils1, int fils2)
{
	char temp[15];
	char suppression = 'S';
	Cell *cellASupprimer = NULL, *connexion1, *connexion2;

	connexion1 = rechercher(tabFils[fils1]->connexions, fils2);
	connexion2 = rechercher(tabFils[fils2]->connexions, fils1);

	if(connexion1 == NULL && connexion2 == NULL)
	{
		printf(WARNING"Coeur - La connexion n'existe pas, elle ne sera donc pas supprimée.\n");
		return ;
	}
	if(tabFils[fils1] == NULL)
	{
		printf(WARNING"Coeur - Le fils n°%d n'existe pas.\n", fils1);
	}
	else if(tabFils[fils2] == NULL)
	{
		printf(WARNING"Coeur - Le fils n°%d n'existe pas.\n", fils2);	
	}
	else
	{
		/* Informer le fils 1 */
		if(write(tabFils[fils1]->tube[TUBE_ECRITURE], &suppression, sizeof(char)) == -1)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", fils1);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		if(write(tabFils[fils1]->tube[TUBE_ECRITURE], &fils2, sizeof(int)) == -1)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", fils1);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		/* Informer le fils 2 */
		if(write(tabFils[fils2]->tube[TUBE_ECRITURE], &suppression, sizeof(char)) == -1)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", fils1);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		if(write(tabFils[fils2]->tube[TUBE_ECRITURE], &fils1, sizeof(int)) == -1)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", fils1);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		/* Détruire les connexions */
		sprintf(temp, "tube_%d_%d", fils1, fils2);
		if(unlink(temp) == -1)
		{
			fprintf(stderr, ERREUR"Coeur - Erreur lors de la suppression du tube annonyme '%s'\n", temp);
			perror(" ");
			exit(EXIT_FAILURE);
		}

		sprintf(temp, "tube_%d_%d", fils2, fils1);
		if(unlink(temp) == -1)
		{
			fprintf(stderr, ERREUR"Coeur - Erreur lors de la suppression du tube annonyme '%s'\n", temp);
			perror(" ");
			exit(EXIT_FAILURE);
		}

		cellASupprimer = rechercher(tabFils[fils1]->connexions, fils2);
		supprimer(tabFils[fils1]->connexions, cellASupprimer);

		cellASupprimer = rechercher(tabFils[fils2]->connexions, fils1);
		supprimer(tabFils[fils2]->connexions, cellASupprimer);
	}
	
}

void distribuerMessage(fils_t **tabFils, int emmeteur, int destinataire, char* message)
{
	char cmdeMsg = 'M';
	int tailleMess;

	tailleMess = strlen(message);
	if(tabFils[emmeteur] == NULL)
	{
		printf(WARNING"Coeur - Le fils n°%d n'existe pas.\n", emmeteur);
	}
	else if(tabFils[destinataire] == NULL)
	{
		printf(WARNING"Coeur - Le fils n°%d n'existe pas.\n", destinataire);	
	}
	else
	{
		/* Informer le fils 1 */
		if(write(tabFils[emmeteur]->tube[TUBE_ECRITURE], &cmdeMsg, sizeof(char)) == -1)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", emmeteur);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		if(write(tabFils[emmeteur]->tube[TUBE_ECRITURE], &destinataire, sizeof(int)) == -1)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", emmeteur);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		if(write(tabFils[emmeteur]->tube[TUBE_ECRITURE], &tailleMess, sizeof(int)) == -1)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", emmeteur);
			perror(" ");
			exit(EXIT_FAILURE);
		}
		if(write(tabFils[emmeteur]->tube[TUBE_ECRITURE], message, sizeof(char) * tailleMess) == -1)
		{
			fprintf(stderr, ERREUR "Coeur - Erreur lors d'un envoi vers le fils n°%d\n", emmeteur);
			perror(" ");
			exit(EXIT_FAILURE);
		}
	}

	free(message);
	message = NULL;
}

void stoperCoeur(fils_t **tabFils, ListeVide* listeVide, int numMax)
{
	int i;
	union sigval valeur;

	valeur.sival_int = 0;

	for(i = 0; i < numMax; i++)
	{
		if(tabFils[i] != NULL)
		{
			supprimerFils( i, tabFils, listeVide);
		}
	}

	if(sigqueue(getppid(), SIGRTMAX, valeur) == -1) 
	{
		fprintf(stderr, ERREUR"Coeur - Erreur lors de l'envoi du signal vers le donneur d'ordre\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}
	detruireListeVide(&listeVide);
	continuerCoeur = 0;
}

void sauverEtat(char* filename, fils_t **filsTab, int numMax)
{
	int fd, i;
	Cell *corresp;

	if((fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR)) == -1)
	{
		fprintf(stderr, ERREUR"Coeur - Erreur lors de l'ouverture du fichier de sauvegarde\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(write(fd, &numMax, sizeof(int)) == -1)
	{
		fprintf(stderr, ERREUR"Coeur - Erreur lors de la sauvegarde du numero max\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	for(i=0; i < numMax; i++)
	{
		if(filsTab[i] != NULL)
		{
			if(write(fd, &i, sizeof(int)) == -1)
			{
				fprintf(stderr, ERREUR"Coeur - Erreur lors de la sauvegarde du numero du fils courant\n");
				perror(" ");
				exit(EXIT_FAILURE);
			}
		}
	}

	i = -1;
	if(write(fd, &i, sizeof(int)) == -1)
	{
		fprintf(stderr, ERREUR"Coeur - Erreur lors de la sauvegarde du numero du fils courant\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < numMax; i++)
	{
		if(filsTab[i] != NULL)
		{
			corresp = filsTab[i]->connexions->cell;
			while(corresp != NULL)
			{
				if(write(fd, &i, sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors du numero du fils courant\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}

				if(write(fd, &(corresp->num), sizeof(int)) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la sauvegarde du numero du fils correspondant (n°%d)\n", corresp->num);
					perror(" ");
					exit(EXIT_FAILURE);
				}

				corresp = corresp->next;
			}
		}			
	}

	if(close(fd) == -1)
	{
		fprintf(stderr, ERREUR"Coeur - Erreur lors de la fermeture du fichier de sauvegarde\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}
}

int chargerEtat(char* filename, fils_t **tabFils, ListeVide *listeVide)
{
	int fd, i, max = 0, intLu = 0, intLu2 = 0, trouve = 1, resLect = -1;
	CellVide *vide;

	if((fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR)) == -1)
	{
		fprintf(stderr, ERREUR"Coeur - Erreur lors de l'ouverture du fichier de sauvegarde\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if((resLect = read(fd, &max, sizeof(int))) == -1)
	{
		fprintf(stderr, ERREUR"Coeur - Erreur lors de la récupération du numéro max\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}
	if(max == 0)
	{
		return 0;
	}

	i = 0;
	while(intLu != -1 && resLect != 0)
	{
		if(trouve)
		{
			if((resLect = read(fd, &intLu, sizeof(int))) == -1)
			{
				fprintf(stderr, ERREUR"Coeur - Erreur lors de la récupération du numéro du fils\n");
				perror(" ");
				exit(EXIT_FAILURE);
			}
		}
		
		if(intLu == i)
		{
			tabFils[i] = creerFils(i);
			trouve = 1;
		}
		else if(intLu != -1)
		{
			vide = creerCelluleVide(i);
			insererVide(listeVide, vide);
			trouve = 0;
		}

		if(intLu != -1)
		{
			i++;	
		}
	}
	max = i;
	if(intLu != -1)
	{
		fprintf(stderr, ERREUR"Coeur - Erreur lors de la récupération des fils\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		resLect = -1;
		while(resLect != 0)
		{
			if((resLect = read(fd, &intLu, sizeof(int))) == -1)
			{
				fprintf(stderr, ERREUR"Coeur - Erreur lors de la récupération du numéro du fils\n");
				perror(" ");
				exit(EXIT_FAILURE);
			}
			else if( resLect != 0 )
			{
				if((resLect = read(fd, &intLu2, sizeof(int))) == -1)
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la récupération du numéro du fils\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}
				else if(resLect != 0)
				{
					ajoutConnexion(tabFils, intLu, intLu2);
				}
				else
				{
					fprintf(stderr, ERREUR"Coeur - Erreur lors de la récupération des connexions : ");
					fprintf(stderr, "Nombre de fils invalide\n");
					exit(EXIT_FAILURE);
				}
			}	
		}
	}

	return max;	
}