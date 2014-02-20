#include "celluleVide.h"
#include <stdlib.h>
#include <string.h>

CellVide * creerCelluleVide(int _num)
{
	CellVide * cellVide;

	cellVide = (CellVide *) malloc(sizeof(CellVide));
	cellVide->num = _num;
	cellVide->next = NULL;
	cellVide->previous = NULL;

	return cellVide;
}

void detruireCelluleVide(CellVide ** _cellVide)
{
	(*_cellVide)->previous=NULL;
	(*_cellVide)->next=NULL;
	free(*_cellVide);
	_cellVide=NULL;
}