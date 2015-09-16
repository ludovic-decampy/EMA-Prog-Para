#include "stdafx.h"

MetaPlaque::MetaPlaque(Plaque* ambiant)
{
	this->plaques = new Plaque**[3];
	for (int i = 0; i < 3; ++i) {
		this->plaques[i] = new Plaque*[3];
		for (int j = 0; j < 3; j++){
			this->plaques[i][j] = ambiant; 
		}
	}
}

MetaPlaque::~MetaPlaque()
{
}
