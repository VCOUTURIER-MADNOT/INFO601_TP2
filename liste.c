#include "liste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

List * creerListe()
{
	List * list = NULL;
	list = (List *)malloc(sizeof(List));

	list->cell = NULL;

	return list;
}

void detruireListe(List ** _list)
{
	Cell *next = NULL;
	Cell *cell = NULL;
	cell = (*_list)->cell;
	
	while(cell != NULL)
	{
		next = cell->next;
		detruireCellule(&cell);
		cell = next;
	}
	free(*_list);
}

void inserer(List * _list, Cell * _cell)
{
	_cell->next = _list->cell;
	if (_list->cell != NULL)
	{
		_list->cell->previous = _cell;
	}
	_list->cell = _cell;

	_cell->previous = NULL;
}

Cell * rechercher(List * _list, int _num)
{
	Cell * cell = NULL;
	cell = _list->cell;
	while(cell != NULL && (cell->num != _num))
	{
		cell = cell->next;
	}
	
	return cell;
}

void supprimer(List * _list, Cell * _cell)
{
	if(_cell != NULL){
		if(_cell->previous != NULL)
		{
			Cell * previous = _cell->previous;
			previous->next = _cell->next;
		}
		else
		{
			_list->cell = _cell->next;
		}

		if(_cell->next != NULL)
		{

			Cell * next = _cell->next;
			next->previous = _cell->previous;
		}
		detruireCellule(&_cell);
	}
}
