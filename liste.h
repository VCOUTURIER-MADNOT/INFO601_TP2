#ifndef DEF_LISTE
#define DEF_LISTE

	#include "cellule.h"

	typedef struct {
		Cell * cell;
	} List;

	List * creerListe();
	void detruireListe(List ** _list);
	void inserer(List * _list, Cell * _cell);
	Cell * rechercher(List * _list, int _num);
	void supprimer(List * _list, Cell * _cell);

#endif