#ifndef HEL
#define HEL

#define HEIGHT	3
#define WIDTH	3


class Plaque;
class MetaPlaque
{
private:

public:
	MetaPlaque(Plaque* ambiant);
	~MetaPlaque();

	Plaque*** plaques;
};


class Plaque
{

public:
	Plaque();
	Plaque(double** grid);
	Plaque(double** grid, Plaque* ambiant);
	~Plaque();

	MetaPlaque* metaPlaque;
	double** grid;

	int**  getAllCoords();
	double getNewValueForCoord(int x, int y);
	void updateGrid();
};


#endif