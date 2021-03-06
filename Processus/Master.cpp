#include <mpi.h>
#include <stdio.h>
#include <iostream>

using namespace std;


int main( int argc, char *argv[] )
{
    // Partie plaque 
    float ambiantTemperature = 20;

    int masterWidth     = 4; 
    int masterHeight    = 3;

    int slaveWidth      = 3; 
    int slaveHeight     = 3; 

    int nbSlaves        = masterHeight*masterWidth; 
    

    float plaques[12][9] = {
{50,50,50,50,90,50,50,50,50},
{50,50,50,50,90,50,50,50,50},
{50,50,50,50,90,50,50,50,50},
{50,50,50,50,90,50,50,50,50},
{50,50,50,50,90,50,50,50,50},
{50,50,50,50,90,50,50,50,50},
{50,50,50,50,90,50,50,50,50},
{50,50,50,50,90,50,50,50,50},
{50,50,50,50,90,50,50,50,50},
{50,50,50,50,90,50,50,50,50},
{50,50,50,50,90,50,50,50,50},
{50,50,50,50,90,50,50,50,50},
    };
    

    // Partie MPI     
    int i, compteur;
    MPI_Status etat;

    

    char *cmds[2] = { (char*) "Coordinator", (char*) "Slave"};


    char ***mpi_args = new char**[2]; 
    if(argc == 2){
        mpi_args[0] = new char*[1];
        mpi_args[0][0] = argv[1]; 
    }

    int np[2] = { 1, nbSlaves};

    MPI_Info infos[2] = { MPI_INFO_NULL, MPI_INFO_NULL };
    int errcodes[5]; 
    MPI_Comm intercomm; 
    MPI_Init( &argc, &argv );
    
    MPI_Comm_spawn_multiple (
        2,
        cmds,
        mpi_args,
        np,
        infos,
        0,
        MPI_COMM_WORLD,
        &intercomm,
        errcodes
    );


    // ----------------- Initialisation -----------------
    // 1) Envois au coordinateur 
    MPI_Send (&masterWidth,         1,MPI_INT,0,0,intercomm);
    MPI_Send (&masterHeight,        1,MPI_INT,0,0,intercomm);    
    MPI_Send (&slaveWidth,         1,MPI_INT,i,0,intercomm);
    MPI_Send (&slaveHeight,        1,MPI_INT,i,0,intercomm);    
    MPI_Send (&ambiantTemperature,  1,MPI_FLOAT,0,0,intercomm);


    // 2) Envois aux esclaves 
    for(int i=1; i<= nbSlaves; i++){
        // Envois des différentes tailles 
        MPI_Send (&masterWidth,        1,MPI_INT,i,0,intercomm);
        MPI_Send (&masterHeight,       1,MPI_INT,i,0,intercomm);          
        MPI_Send (&slaveWidth,         1,MPI_INT,i,0,intercomm);
        MPI_Send (&slaveHeight,        1,MPI_INT,i,0,intercomm);
        MPI_Send (&ambiantTemperature,  1,MPI_FLOAT,i,0,intercomm);
        // Envois des données de la plaque 
        MPI_Send (&plaques[i-1],       slaveWidth*slaveHeight,MPI_FLOAT,i,0,intercomm);
        
    }




    // Reception du signal de fin du coordinateur 
    char result;
    MPI_Recv (&result, 1, MPI_CHAR ,0, 0, intercomm, &etat);
    
    
    MPI_Finalize();
    return 0;
}
