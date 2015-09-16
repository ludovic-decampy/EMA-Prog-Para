#include "stdafx.h"
#include <iostream>
#include <omp.h>

using namespace std;

int** generateDirections(){
	int** r = new int*[9];
	int k = 0; 
	for (int i = -1; i <= 1; i++){
		for (int j = -1; j <= 1; j++){
			r[k] = new int[2]; 
			r[k][0] = i;
			r[k][1] = j; 
			k++;
		}
	}
	return r; 
}

Plaque::Plaque()
{

}


Plaque::~Plaque()
{
}

Plaque::Plaque(double** grid){
	this->grid = grid; 
	//this->metaPlaque = new MetaPlaque();
}

Plaque::Plaque(double** grid, Plaque* ambiant){
	this->grid = grid;
	this->metaPlaque = new MetaPlaque(ambiant);

}

int** Plaque::getAllCoords(){
	int** result = new int*[WIDTH*HEIGHT];
	int i = 0;
	for (int x = 0; x < WIDTH; x++){
		for (int y = 0; y < HEIGHT; y++){
			result[i] = new int[2];
			result[i][0] = x;
			result[i][1] = y;
			i++;
		}
	}
	return result;
}

double Plaque::getNewValueForCoord(int x, int y){
	
	int** directions = generateDirections(); 
	double moyenne = 0;
	#pragma omp parallel shared(moyenne) 
	{
		#pragma omp for 
		for (int i = 0; i < 9; i++){
				int newX = x + directions[i][0];
				int newY = y + directions[i][1];
				if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT){
					if (newX < 0){
						if (newY < 0){
							// NW
							moyenne += this->metaPlaque->plaques[0][0]->grid[WIDTH - 1][HEIGHT - 1];
						}
						else if (newY < HEIGHT){
							// W
							moyenne += this->metaPlaque->plaques[0][1]->grid[WIDTH - 1][newY];
						}
						else{
							// SW
							moyenne += this->metaPlaque->plaques[0][2]->grid[WIDTH - 1][0];
						}
					}
					else if (newX >= WIDTH){
						if (newY < 0){
							// NE
							moyenne += this->metaPlaque->plaques[2][0]->grid[0][HEIGHT - 1];
						}
						else if (newY < HEIGHT){
							// E
							moyenne += this->metaPlaque->plaques[2][1]->grid[0][newY];
						}
						else{
							// SE
							moyenne += this->metaPlaque->plaques[2][2]->grid[0][0];
						}
					}
					else{
						if (newY < 0){
							// N
							moyenne += this->metaPlaque->plaques[1][0]->grid[newX][HEIGHT - 1];
						}
						else{
							// S
							moyenne += this->metaPlaque->plaques[1][2]->grid[newX][0];
						}
					}
				}
				else{
					moyenne += this->grid[newX][newY];
				}
			}
		}
	
	moyenne = moyenne / 9;
	return moyenne; 
}

void Plaque::updateGrid(){
	
	double** resultat = new double*[WIDTH];
	for (int i = 0; i < WIDTH; ++i) {
		resultat[i] = new double[HEIGHT];
	}

	int** cases = this->getAllCoords();
	#pragma omp parallel shared(resultat)
	{
		#pragma omp for 
		for (int i = 0; i <WIDTH*HEIGHT; i++){
			int x = cases[i][0];
			int y = cases[i][1];
			//On parcours toute celles qui sont autour 
			resultat[x][y] = this->getNewValueForCoord(x,y); 
		}
	}
	this->grid = resultat; 
}