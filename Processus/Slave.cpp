#include <mpi.h>
#include <stdio.h>
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
        int cellTemperature; 
        int ambiantTemperature; 
        MPI_Recv(&cellTemperature, 1, MPI_INT, 0, 0, parent, &etat);
        printf ("Je suis un esclave ! La temperature de ma cellule est de %d°C \n", cellTemperature);
        
        // reception du coordinateur 

        for(int i=0; i<10; i++){
            MPI_Recv(&ambiantTemperature, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &etat);
            printf ("/!\\ Reception coordinateur :  %d°C \n", ambiantTemperature); 
            MPI_Send(&ambiantTemperature, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);            
        }

    }

    

    MPI_Finalize();
    return 0;
}
