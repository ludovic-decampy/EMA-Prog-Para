#include <mpi.h>
#include <stdio.h>
#include <iostream>
using namespace std; 
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
        char end = 'c';
        int nbSlaves; 
        int ambiantTemperature; 
        MPI_Recv(&nbSlaves, 1, MPI_INT, 0, 0, parent, &etat);
        MPI_Recv(&ambiantTemperature, 1, MPI_INT, 0, 0, parent, &etat);
        printf ("Je suis un coordinateur ! La temperature ambiante est de %d°C \n", ambiantTemperature);

        for(int k = 0; k < 10; k++){
            for(int i=1; i<=nbSlaves; i++){
                MPI_Send (&ambiantTemperature,1,MPI_INT, i, 0, MPI_COMM_WORLD);            
            }

            int* temperatures = new int[nbSlaves]; 
            for(int i=1; i<=nbSlaves; i++){
                int recv; 
                MPI_Recv(&recv, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &etat);
                temperatures[i-1] = recv; 
            }

            for(int i=0; i<nbSlaves; i++){
                cout << i << "=" << temperatures[i] << ";" ; 
            }
            cout << endl; 
        }

        MPI_Send (&end,1,MPI_CHAR, 0, 0, parent);
    }
    MPI_Finalize();
    return 0;
}
