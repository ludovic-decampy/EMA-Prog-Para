#include <mpi.h>
#include <stdio.h>
#include <iostream>

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

        char end = 's';
        int nbSlaves; 
        int width; 


        int cellTemperature = -9999; 
        int ambiantTemperature; 


        MPI_Recv(&cellTemperature, 1, MPI_INT, 0, 0, parent, &etat);
        printf ("Je suis un esclave ! La temperature de ma cellule est de %d°C \n", cellTemperature);        
        MPI_Recv(&width, 1, MPI_INT, 0, 0, parent, &etat);
        printf ("Je suis un esclave ! La largeur de la plaque %d \n", width);        
        MPI_Recv(&nbSlaves, 1, MPI_INT, 0, 0, parent, &etat);
        printf ("Je suis un esclave ! La taille de la plaque %d \n", nbSlaves);

        int* coords = getSlaveXYFromId(myrank, width); 

        int x = coords[0];
        int y = coords[1];

        //cout << "Je suis jesus n°"<<myrank<< " ("<<x<<";"<<y<<")" << "calc " << getSlaveIdFromXY(x,y,width) << endl; 

        
        // reception du coordinateur 

        for(int i=0; i<10; i++){

            MPI_Recv(&ambiantTemperature, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &etat);


            int prevTemperature; 
            int nextTemperature; 
            int prevIndex; 
            int nextIndex;

            MPI_Request request; 
            int neighbourTemp;
            int meanTemp = 0;  


            for(int i=-1; i<=1; i++){
                for(int j=-1; j<=1; j++){
                    int neighbourSlaveId = getSlaveIdFromXY(x+i, y+j, width); 
                    if(neighbourSlaveId > 0 && neighbourSlaveId <= nbSlaves){
                        MPI_Isend(&cellTemperature, 1, MPI_INT, neighbourSlaveId, 0, MPI_COMM_WORLD, &request);            
                    }
                }
            }

            for(int i=-1; i<=1; i++){
                for(int j=-1; j<=1; j++){
                    int neighbourSlaveId = getSlaveIdFromXY(x+i, y+j, width); 
                    if(neighbourSlaveId > 0 && neighbourSlaveId <= nbSlaves){
                        MPI_Recv(&neighbourTemp, 1, MPI_INT, neighbourSlaveId, 0, MPI_COMM_WORLD, &etat);
                    }else{
                        neighbourTemp = ambiantTemperature; 
                    }
                    meanTemp += neighbourTemp;
                }
            }

            meanTemp = meanTemp/9;
            
       
            cellTemperature = meanTemp; 
            //printf("L'escave %d a mis a jour sa temperature %d\n", myrank, cellTemperature);
            MPI_Send(&cellTemperature, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);            
        }

    }

    

    MPI_Finalize();
    return 0;
}
