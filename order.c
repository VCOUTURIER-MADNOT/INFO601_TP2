#include "constantes.h"
#include "coeur.h"
#include "fils.h"


int continuerOrder = 1;

void accuseReception(int numSig, siginfo_t *info, void* rien)
{
	char *cmde = NULL;
	cmde = (char*)malloc(sizeof(char) * 30);
	switch(info->si_value.sival_int)
	{
		case 0:
			strcpy(cmde,"Arrêt du coeur");
			break;
		case 1:
			strcpy(cmde,"Création d'un fils");
			break;
		case 2:
			strcpy(cmde,"Arrêt d'un fils");
			break;
		case 3:
			strcpy(cmde,"Ajout d'une connexion");
			break;
		case 4:
			strcpy(cmde,"Suppression d'une connexion");
			break;
		case 5:
			strcpy(cmde,"Envoi d'un message");
			break;
		default:
			strcpy(cmde,"Commande inconnue");
			break;
	}

	printf(INFO"Donneur d'ordres - Le coeur a correctement reçu la commande '%s'.\n", cmde);
	free(cmde);
}

void stopOrder(int numSig, siginfo_t *info, void* rien)
{
	continuerOrder = 0;
}

void afficherMenu()
{
	printf("\n\nVoici la liste des commandes :\n");
	printf("\t1. Ajouter un fils\n");
	printf("\t2. Supprimer un fils\n");
	printf("\t3. Ajouter une connexion entre 2 fils\n");
	printf("\t4. Supprimer une connexion entre 2 fils\n");
	printf("\t5. Envoyer un message\n");
	printf("\t0. Arreter le programme\n");
}

int main(int argc, char** argv)
{
	pid_t pid, parentPID;
	int cmde, tailleMessage, resSig;
	int fdEcr, statut;
	char c;
	char buffer[TAILLE_MAX_MESSAGE];
	char *rectification = NULL;

	sigset_t ensemble;
	siginfo_t info;
	struct timespec timer;

	struct sigaction action;
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = stopOrder;
	sigaction(SIGRTMAX, &action, NULL);

	sigemptyset(&ensemble);
	sigaddset(&ensemble, SIGRTMIN);
	/*sigaddset(&ensemble, SIGRTMAX);*/
	sigprocmask(SIG_BLOCK, &ensemble, NULL);
	timer.tv_nsec = 1;


	if(mkfifo(NOM_TUBE, S_IRUSR | S_IWUSR) == -1)
	{
		if(errno == EEXIST)
		{
			fprintf(stderr, WARNING"Donneur d'ordres - tube nommé %s déjà existant.\n", NOM_TUBE);
		}
		else
		{
			fprintf(stderr, ERREUR"Donneur d'ordres - Impossible de créer le tube %s\n", NOM_TUBE);
			perror(" ");
			exit(EXIT_FAILURE);
		}
	}
	parentPID = getpid();
	if((pid = fork()) == -1)
	{
		fprintf(stderr, ERREUR"Donneur d'ordres - Impossible de créer le coeur\n");
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(pid == 0)
	{
		coeur(parentPID);
	}

	if((fdEcr = open(NOM_TUBE, O_WRONLY)) == -1)
	{
		fprintf(stderr, ERREUR"Donneur d'ordres - Impossible d'ouvrir le tube %s\n", NOM_TUBE);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	printf("Bienvenue dans le programme Tubetella !!!!!\n");
	while(continuerOrder)
	{
		afficherMenu();
		printf(ACTION"Donneur d'ordres - Veuillez saisir la commande :\n");
		if(scanf("%d", &cmde) == EOF)
		{
			printf(WARNING"Donneur d'ordres - Erreur lors de la saisie clavier de la commande principale\n");
			perror(" ");
		}
		if(continuerOrder)
		{
			if((cmde <= 5 && cmde >= 0) && write(fdEcr, &cmde, sizeof(int)) == -1)
			{
				fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'envoi de la commande au coeur\n");
				perror(" ");
				exit(EXIT_FAILURE);
			}

			while(((c=getchar()) != '\n') && ( c != EOF));
			switch(cmde)
			{
				case 0:		
					break;
				case 1:
					break;
				case 2:
					printf(ACTION"Donneur d'ordres - Veuillez saisir le numéro du fils à supprimer :\n");
					while(scanf("%d", &cmde) == -1 )
					{
						printf(WARNING"Donneur d'ordres - Erreur lors de la saisie clavier de la commande\n");
					}

					if(write(fdEcr, &cmde, sizeof(int)) == -1)
					{
						fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'envoi de la commande au coeur\n");
						perror(" ");
						exit(EXIT_FAILURE);
					}

					while(((c=getchar()) != '\n') && ( c != EOF));
					break;
				case 3:
					printf(ACTION"Donneur d'ordres - Veuillez saisir le numéro du premier fils de la connexion :\n");
					while(scanf("%d", &cmde) == -1 )
					{
						printf(WARNING"Donneur d'ordres - Erreur lors de la saisie clavier de la commande\n");
					}

					if(write(fdEcr, &cmde, sizeof(int)) == -1)
					{
						fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'envoi de la commande au coeur\n");
						perror(" ");
						exit(EXIT_FAILURE);
					}
					while(((c=getchar()) != '\n') && ( c != EOF));
					printf(ACTION"Donneur d'ordres - Veuillez saisir le numéro du second fils de la connexion :\n");
					while(scanf("%d", &cmde) == -1 )
					{
						printf(WARNING"Donneur d'ordres - Erreur lors de la saisie clavier de la commande\n");
					}

					if(write(fdEcr, &cmde, sizeof(int)) == -1)
					{
						fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'envoi de la commande au coeur\n");
						perror(" ");
						exit(EXIT_FAILURE);
					}
					while(((c=getchar()) != '\n') && ( c != EOF));
					break;
				case 4:
					printf(ACTION"Donneur d'ordres - Veuillez saisir le numéro du premier fils de la connexion à supprimer :\n");
					while(scanf("%d", &cmde) == -1 )
					{
						printf(WARNING"Donneur d'ordres - Erreur lors de la saisie clavier de la commande\n");
					}

					if(write(fdEcr, &cmde, sizeof(int)) == -1)
					{
						fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'envoi de la commande au coeur\n");
						perror(" ");
						exit(EXIT_FAILURE);
					}
					while(((c=getchar()) != '\n') && ( c != EOF));
					printf(ACTION"Donneur d'ordres - Veuillez saisir le numéro du premier fils de la connexion à supprimer :\n");
					while(scanf("%d", &cmde) == -1 )
					{
						printf(WARNING"Donneur d'ordres - Erreur lors de la saisie clavier de la commande\n");
					}

					if(write(fdEcr, &cmde, sizeof(int)) == -1)
					{
						fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'envoi de la commande au coeur\n");
						perror(" ");
						exit(EXIT_FAILURE);
					}
					while(((c=getchar()) != '\n') && ( c != EOF));
					break;
				case 5:
					printf(ACTION"Donneur d'ordres - Veuillez saisir le numéro du fils qui doit envoyer le message :\n");
					while(scanf("%d", &cmde) == -1 )
					{
						printf(WARNING"Donneur d'ordres - Erreur lors de la saisie clavier de la commande\n");
					}

					if(write(fdEcr, &cmde, sizeof(int)) == -1)
					{
						fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'envoi de la commande au coeur\n");
						perror(" ");
						exit(EXIT_FAILURE);
					}

					while(((c=getchar()) != '\n') && ( c != EOF));

					printf(ACTION"Donneur d'ordres - Veuillez saisir le numéro du fils destinataire :\n");
					while(scanf("%d", &cmde) == -1 )
					{
						printf(WARNING"Donneur d'ordres - Erreur lors de la saisie clavier de la commande\n");
					}

					if(write(fdEcr, &cmde, sizeof(int)) == -1)
					{
						fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'envoi de la commande au coeur\n");
						perror(" ");
						exit(EXIT_FAILURE);
					}
					while(((c=getchar()) != '\n') && ( c != EOF));
					printf(ACTION"Donneur d'ordres - Entrez le message : \n");
					while(fgets(buffer, sizeof(char)*TAILLE_MAX_MESSAGE, stdin) == NULL)
					{
						printf(WARNING"Donneur d'ordres - Erreur lors de la saisie clavier de la commande\n");
						printf(ACTION"Donneur d'ordres - Entrez le message : \n");
					}

					if((rectification = strchr(buffer, '\n')) != NULL)
					{
						*rectification = '\0';
					}

					tailleMessage = strlen(buffer);
					if(write(fdEcr, &tailleMessage, sizeof(int)) == -1)
					{
						fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'envoi de la commande au coeur\n");
						perror(" ");
						exit(EXIT_FAILURE);
					}
					if(write(fdEcr, &buffer, sizeof(char) * tailleMessage) == -1)
					{
						fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'envoi de la commande au coeur\n");
						perror(" ");
						exit(EXIT_FAILURE);
					}
					break;
				default:
					printf(INFO"Donneur d'ordres - Commande invalide\n");
					break;
			}
			if((resSig = sigtimedwait(&ensemble, &info, &timer)) == -1)
			{
				if(errno != EAGAIN)
				{
					fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'attente d'un signal\n");
					perror(" ");
					exit(EXIT_FAILURE);
				}
			}
			if(resSig == SIGRTMIN)
			{
				accuseReception(resSig, &info, NULL);
			}
			if(resSig == SIGRTMAX)
			{
				printf("J'ai reçu un sigrtmax\n");
				stopOrder(resSig, &info, NULL);
			}
		}
	}

	if(waitpid(pid, &statut, 0) == -1)
	{
		fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de l'attente du coeur %s\n", NOM_TUBE);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(WIFEXITED(statut))
	{
		printf(INFO"Donneur d'ordres - Le coeur s'est terminé correctement\n");
	}
	else
	{
		fprintf(stderr, WARNING"Donneur d'ordres - Le coeur s'est terminé anormalement\n");
	}

	if(close(fdEcr) == -1)
	{
		fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de la fermeture du tube %s\n", NOM_TUBE);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	if(unlink(NOM_TUBE) == -1)
	{
		fprintf(stderr, ERREUR"Donneur d'ordres - Erreur lors de la destruction du tube %s\n", NOM_TUBE);
		perror(" ");
		exit(EXIT_FAILURE);
	}
	printf(INFO"Donneur d'ordres - Arrêt du programme.\n");
	free(rectification);
	return EXIT_SUCCESS;
}