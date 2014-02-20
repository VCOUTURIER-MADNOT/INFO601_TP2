#include "listeVide.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ListeVide * creerListeVide()
{
	ListeVide * listeVide = NULL;
	listeVide = (ListeVide*)malloc(sizeof(ListeVide));

	listeVide->cell = NULL;

	return listeVide;
}

void detruireListeVide(ListeVide ** _listeVide)
{
	CellVide *next = NULL;
	CellVide *cell = NULL;
	cell = (*_listeVide)->cell;
	
	while(cell != NULL)
	{
		next = cell->next;
		detruireCelluleVide(&cell);
		cell = next;
	}
	free(*_listeVide);
}

void insererVide(ListeVide * _listeVide, CellVide * _cell)
{
	_cell->next = _listeVide->cell;
	if (_listeVide->cell != NULL)
	{
		_listeVide->cell->previous = _cell;
	}
	_listeVide->cell = _cell;
	_cell->previous = NULL;
}

CellVide * rechercherVide(ListeVide * _listeVide, int _num)
{
	CellVide * cell = NULL;
	cell = _listeVide->cell;
	while(cell != NULL && (cell->num != _num))
	{
		cell = cell->next;
	}
	
	return cell;
}

void supprimerVide(ListeVide * _listeVide, CellVide * _cell)
{
	CellVide *previous, *next;

	if(_cell != NULL){
		if(_cell->previous != NULL)
		{
			previous = _cell->previous;
			previous->next = _cell->next;
		}
		else
		{
			_listeVide->cell = _cell->next;
		}

		if(_cell->next != NULL)
		{

			next = _cell->next;
			next->previous = _cell->previous;
		}
		detruireCelluleVide(&_cell);
	}
}
