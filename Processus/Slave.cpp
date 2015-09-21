#include <mpi.h>
#include <stdio.h>
#include <iostream>

using namespace std;

#define MAXSIZE
typedef struct 
{
    char        data [MAXSIZE]; 
    int         datasize; 
    MPI_Request* req;
} Buffer ;

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
        int cellTemperature; 
        int ambiantTemperature; 
        MPI_Recv(&cellTemperature, 1, MPI_INT, 0, 0, parent, &etat);
        printf ("Je suis un esclave ! La temperature de ma cellule est de %d°C \n", cellTemperature);        
        MPI_Recv(&nbSlaves, 1, MPI_INT, 0, 0, parent, &etat);
        printf ("Je suis un esclave ! La taille de la plaque %d \n", nbSlaves);
        
        // reception du coordinateur 

        for(int i=0; i<10; i++){

            MPI_Recv(&ambiantTemperature, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &etat);


            int prevTemperature; 
            int nextTemperature; 

            MPI_Request sentToPrevious; 
            MPI_Request sentToNext; 


            int prevIndex = myrank - 1;
            int nextIndex = myrank + 1; 

            
            if(prevIndex > 0)
                MPI_Isend(&cellTemperature, 1, MPI_INT, prevIndex, 0, MPI_COMM_WORLD, &sentToPrevious);            
            if(nextIndex <= nbSlaves)
                MPI_Isend(&cellTemperature, 1, MPI_INT, nextIndex, 0, MPI_COMM_WORLD, &sentToNext);            


            if(prevIndex > 0)
                MPI_Recv(&prevTemperature, 1, MPI_INT, prevIndex, 0, MPI_COMM_WORLD, &etat);
            else 
               prevTemperature = ambiantTemperature; 


            if(nextIndex <= nbSlaves)
                MPI_Recv(&nextTemperature, 1, MPI_INT, nextIndex, 0, MPI_COMM_WORLD, &etat);
            else                        
                nextTemperature = ambiantTemperature;             


            int meanTemp = (prevTemperature + nextTemperature + cellTemperature)/3; 
            cellTemperature = meanTemp; 
            printf("L'escave %d a mis a jour sa temperature %d\n", myrank, cellTemperature);


            MPI_Send(&ambiantTemperature, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);            
        }

    }

    

    MPI_Finalize();
    return 0;
}
