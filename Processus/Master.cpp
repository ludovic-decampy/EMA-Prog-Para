#include <mpi.h>
#include <stdio.h>



int main( int argc, char *argv[] )
{
    int i, compteur;
    MPI_Status etat;

    int nbSlaves = 2 ; 

    char *cmds[2] = {
        (char*) "Coordinator",
        (char*) "Slave",
    };
    int np[2] = {
        1,
        nbSlaves,
    };

    MPI_Info infos[2] = { MPI_INFO_NULL, MPI_INFO_NULL };
    int errcodes[5]; 

    MPI_Comm intercomm; 
    MPI_Init( &argc, &argv );
    
    MPI_Comm_spawn_multiple (
        2,
        cmds,
        MPI_ARGVS_NULL,
        np,
        infos,
        0,
        MPI_COMM_WORLD,
        &intercomm,
        errcodes
    );

    printf ("Pere : J'ai lance toutes les instances.\n");
    // Le père communique de façon synchrone avec chacun de
    // ses fils en utilisant l'espace de communication intercomm

    // envoie de la tempreature ambiant au coordinateur 
    int ambiantTemperature = 20;
    MPI_Send (&ambiantTemperature,1,MPI_INT,0,0,intercomm);

    // envoie de la temperature aux esclaves 
    for (i=1; i<nbSlaves+1; i++)
    {
        int cellTemperature = 30; 
        MPI_Send (&cellTemperature,1,MPI_INT,i,0,intercomm);
    }

    // envoie de la taille de la plaque 
    for (i=0; i<=nbSlaves; i++)
    {
        MPI_Send (&nbSlaves,1,MPI_INT,i,0,intercomm);
    }



    // Reception du signal de fin du coordinateur 
    char result;
    MPI_Recv (&result, 1, MPI_CHAR ,0, 0, intercomm, &etat);
    
    printf ("Pere : J'ai fini \n");
    MPI_Finalize();
    return 0;
}
