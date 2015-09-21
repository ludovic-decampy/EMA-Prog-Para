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

        int ambiantTemperature = 0; 
        MPI_Recv(&ambiantTemperature, 1, MPI_INT, 0, 0, parent, &etat);
        printf ("Je suis un coordinateur ! La temperature ambiante est de %d°C \n", ambiantTemperature);

    }
    MPI_Finalize();
    return 0;
}
