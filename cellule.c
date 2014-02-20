#include "cellule.h"
#include <stdlib.h>
#include <string.h>

Cell * creerCellule(int _num)
{
	Cell * cell;

	cell = (Cell *) malloc(sizeof(Cell));
	cell->num = _num;
	cell->fd[0] = 0;
	cell->fd[1] = 0;
	cell->next = NULL;
	cell->previous = NULL;

	return cell;
}

void detruireCellule(Cell ** _cell)
{
	(*_cell)->previous=NULL;
	(*_cell)->next=NULL;
	free(*_cell);
	_cell=NULL;
}