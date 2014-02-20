#ifndef DEF_CELLULEVIDE
#define DEF_CELLULEVIDE

	typedef struct CellVide1
	{
		struct CellVide1 * previous;
		struct CellVide1 * next;
		int num;
	} CellVide;

	CellVide * creerCelluleVide(int _num);
	void detruireCelluleVide(CellVide ** _cell);

#endif