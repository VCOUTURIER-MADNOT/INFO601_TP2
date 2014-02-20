#ifndef DEF_LISTEVIDE
#define DEF_LISTEVIDE

	#include "celluleVide.h"

	typedef struct {
		CellVide * cell;
	} ListeVide;

	ListeVide * creerListeVide();
	void detruireListeVide(ListeVide ** _ListeVide);
	void insererVide(ListeVide * _ListeVide, CellVide * _cell);
	CellVide * rechercherVide(ListeVide * _ListeVide, int _num);
	void supprimerVide(ListeVide * _ListeVide, CellVide * _cell);

#endif