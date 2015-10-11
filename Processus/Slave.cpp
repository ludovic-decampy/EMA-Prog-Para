#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <omp.h>

using namespace std;


int getSlaveIdFromXY(int x, int y, int width){
    if(x < 0 || y < 0)  return -1;
    if(x >= width)      return -1;
    return y*width + x + 1; 
}

int* getSlaveXYFromId(int id, int width){
    int* result = new int[2];
    result[0] = (int) (id-1)%width; //x    
    result[1] = (int) (id-1)/width; //y
    return result; 
}

// NO / N / NE / O / E / SO / S / SE 
float getCellVal(float* cells, float** neighbourCells, int slaveWidth, int slaveHeight, int x, int y){
    if( y == -1 ){
        if( x == -1 ) return neighbourCells[0][0]; 
        if( x == slaveWidth ) return neighbourCells[2][0]; 
        return neighbourCells[1][x];
    }else if( y == slaveHeight ){
        if( x == -1 ) return neighbourCells[5][0]; 
        if( x == slaveWidth ) return neighbourCells[7][0]; 
        return neighbourCells[6][x];        
    }else if( x == -1 ){
        return neighbourCells[3][y];
    }else if( x == slaveWidth ){
        return neighbourCells[4][y];
    }else{
        return cells[y*slaveWidth+x]; 
    }
}

int main( int argc, char *argv[] )
{
    int compteur, myrank;
    MPI_Comm parent;
    MPI_Status etat;
    MPI_Init (&argc, &argv);
    MPI_Comm_get_parent (&parent);
    MPI_Comm_rank (MPI_COMM_WORLD,&myrank);

    if (parent == MPI_COMM_NULL)
    {
        printf ("Fils %d : Slave : Pas de pere !\n", myrank);
    }
    else {
        float ambiantTemperature; 
        int masterWidth; 
        int masterHeight; 
        int slaveWidth;
        int slaveHeight; 

        // ----------------- Initialisation -----------------
        // Reception des différentes tailles 
        MPI_Recv (&masterWidth,        1,MPI_INT,0,0,parent,&etat);
        MPI_Recv (&masterHeight,       1,MPI_INT,0,0,parent,&etat);          
        MPI_Recv (&slaveWidth,         1,MPI_INT,0,0,parent,&etat);
        MPI_Recv (&slaveHeight,        1,MPI_INT,0,0,parent,&etat);
        MPI_Recv (&ambiantTemperature,        1,MPI_FLOAT,0,0,parent,&etat);
        // Reception des données de la plaque 
        float* cells = new float[slaveWidth*slaveHeight];  
        MPI_Recv (cells,         (slaveWidth*slaveHeight),MPI_FLOAT,0,0,parent,&etat);

        // Initialisation des coordonnées de la sous plaque 
        int* coords = getSlaveXYFromId(myrank, masterWidth); 
        int x = coords[0];
        int y = coords[1];

        // Permet de faire le premier tout a blanc et d'afficher la plaque a son état d'origine
        for(int loop=0; true; loop++){
            if(loop){
                MPI_Request request; 

                int*   neighbourIds     = new int[8]; 
                neighbourIds[0] = getSlaveIdFromXY(x-1,y-1,masterWidth); //NO
                neighbourIds[1] = getSlaveIdFromXY(x-0,y-1,masterWidth); //N
                neighbourIds[2] = getSlaveIdFromXY(x+1,y-1,masterWidth); //NE
                neighbourIds[3] = getSlaveIdFromXY(x-1,y-0,masterWidth); //O
                neighbourIds[4] = getSlaveIdFromXY(x+1,y-0,masterWidth); //E
                neighbourIds[5] = getSlaveIdFromXY(x-1,y+1,masterWidth); //SO
                neighbourIds[6] = getSlaveIdFromXY(x-0,y+1,masterWidth); //S
                neighbourIds[7] = getSlaveIdFromXY(x+1,y+1,masterWidth); //SE
                for(int i=0; i<8; i++){
                    if(neighbourIds[i]<1||neighbourIds[i]>masterHeight*masterWidth){
                        neighbourIds[i] = -1; 
                    }
                }

                int neighbourSizes[8]     = {1,slaveWidth,1,slaveHeight,slaveHeight,1,slaveWidth,1};
                float** neighbourCells  = (float**) malloc(8 * sizeof(float*)); 
                neighbourCells[0]       = new float[neighbourSizes[0]]; //NO
                neighbourCells[1]       = new float[neighbourSizes[1]]; //N
                neighbourCells[2]       = new float[neighbourSizes[2]]; //NE
                neighbourCells[3]       = new float[neighbourSizes[3]]; //O
                neighbourCells[4]       = new float[neighbourSizes[4]]; //E
                neighbourCells[5]       = new float[neighbourSizes[5]]; //SO
                neighbourCells[6]       = new float[neighbourSizes[6]]; //S
                neighbourCells[7]       = new float[neighbourSizes[7]]; //SE


                neighbourCells[0][0] = cells[0]; // NO
                for(int i=0; i < slaveWidth; i++){
                    neighbourCells[1][i] = cells[i]; // N
                    neighbourCells[6][i] = cells[(slaveHeight-1)*slaveWidth+i]; // S
                }
                neighbourCells[2][0] = cells[slaveWidth-1]; // NE
                for(int i=0; i < slaveHeight; i++){
                    neighbourCells[3][i] = cells[slaveWidth*i]; // O
                    neighbourCells[4][i] = cells[slaveWidth*i + slaveWidth - 1];  // E
                }
                neighbourCells[5][0] = cells[(slaveHeight-1)*slaveWidth];  // SO
                neighbourCells[7][0] = cells[slaveWidth*slaveHeight-1];  // SE 

                // Envois des données aux voisins dans le besoin 
                for(int i = 0; i < 8; i++){
                    if(neighbourIds[i] != -1){
                        MPI_Isend(neighbourCells[i], neighbourSizes[i], MPI_FLOAT, neighbourIds[i], 0, MPI_COMM_WORLD, &request);            
                    }
                }

                // Réinitialisation des données
                // On remet la température ambiante pour absorber l'absence de voisin 
                neighbourCells[0][0] = ambiantTemperature;
                for(int i=0; i < slaveWidth; i++){
                    neighbourCells[1][i] = ambiantTemperature;
                    neighbourCells[6][i] = ambiantTemperature;
                }
                neighbourCells[2][0] = ambiantTemperature;
                for(int i=0; i < slaveHeight; i++){
                    neighbourCells[3][i] = ambiantTemperature;
                    neighbourCells[4][i] = ambiantTemperature;
                }
                neighbourCells[5][0] = ambiantTemperature;
                neighbourCells[7][0] = ambiantTemperature;

                // Récupération des données de voisins si existent 
                for(int i = 0; i < 8; i++){
                    if(neighbourIds[i] != -1){
                        MPI_Recv(neighbourCells[i], neighbourSizes[i], MPI_FLOAT, neighbourIds[i], 0, MPI_COMM_WORLD, &etat);                 
                    }
                }

                // Calcul de la moyenne de la nouvelle cellule 
                float* newCells = new float[slaveWidth*slaveHeight];   
                #pragma omp parallel shared(newCells) 
                {
                    #pragma omp for
                    for (int i = 0; i <slaveWidth*slaveHeight; i++){            
                        double moyenne = 0;        
                        int* coords = getSlaveXYFromId(i+1, slaveWidth); 
                        int x = coords[0];
                        int y = coords[1]; 
                        for (int dx=-1; dx<=1; dx++){
                            for (int dy=-1; dy<=1; dy++){
                                moyenne+=getCellVal(cells, neighbourCells, slaveWidth, slaveHeight, x+dx, y+dy); 
                            }
                        }                        
                        newCells[i]=moyenne/9;
                    }
                }
                cells = newCells; 
            }
            MPI_Send(cells, slaveWidth*slaveHeight, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);   
        

            char again;
            MPI_Recv(&again,1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &etat);                 
            if(again == 'n') break; 
        }       
    }


    MPI_Finalize();

    return 0;
}
