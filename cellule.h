#ifndef DEF_CELLULE
#define DEF_CELLULE

	typedef struct Cell1
	{
		struct Cell1 * previous;
		struct Cell1 * next;
		int num;
		int fd[2];
	} Cell;

	Cell * creerCellule(int _num);
	void detruireCellule(Cell ** _cell);

#endif