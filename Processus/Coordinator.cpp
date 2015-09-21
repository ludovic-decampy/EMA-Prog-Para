#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std; 


string colorFromTemp(float temp){
    int n = temp*100/60; 
    ostringstream result; 
    if(n < 0)   n = 0; 
    if(n > 100) n = 100;

    result << "hsl(" << (100-n)*160/100 << ",80\%,50\%)";
    return result.str(); 
}

void printArray(float* values, int nbVal, int width){
    int x=0; 
    int y=0; 
    int size=20;
    int svg_width = 100/width;
    int svg_height = 100/(nbVal/width);
    cout << "<br/><svg>" << endl; 
    for(int i = 0; i < nbVal; i++){
        cout << "<rect stroke='black' fill='"<< colorFromTemp(values[i]) <<"' x='"<<(x*svg_width)<<"\%' y='"<<(y*svg_height)<<"\%' width='"<<svg_width<<"\%' height='"<<svg_height<<"\%'></rect>";    
        cout << "<text fill='white' text-anchor='end' x='"<<((x+0.5)*svg_width)<<"\%' y='"<<((y+0.5)*svg_height)<<"\%' >"<<fixed << setprecision(1)<<values[i]<<"</text>" << endl; 
        x++;
        if(x == width){
            x=0; 
            y++; 
        }
    }
    cout << "</svg><br/>" << endl; 
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
        char end = 'c';
        int nbSlaves; 
        int width;
        float ambiantTemperature; 
        

        MPI_Recv(&ambiantTemperature, 1, MPI_FLOAT, 0, 0, parent, &etat);
        //printf ("Je suis un coordinateur ! La temperature ambiante est de %f°C \n", ambiantTemperature);
        MPI_Recv(&width, 1, MPI_INT, 0, 0, parent, &etat);
        //printf ("Je suis un coordinateur ! La width est de %d \n", width);        
        MPI_Recv(&nbSlaves, 1, MPI_INT, 0, 0, parent, &etat);
        //printf ("Je suis un coordinateur ! Le coordinateur a reçu la taille de la plaque %d \n", nbSlaves);

        float* temperatures = new float[nbSlaves]; 

        // CACA JULIEN         
        for(int i=1; i<=nbSlaves; i++){
            float recv; 
            MPI_Recv(&recv, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &etat);
            temperatures[i-1] = recv; 
        }
        printArray(temperatures, nbSlaves, width); 
        cout << endl; 
        // CACA JULIEN 

        for(int k = 0; k < 10; k++){

            for(int i=1; i<=nbSlaves; i++){
                MPI_Send (&ambiantTemperature,1,MPI_FLOAT, i, 0, MPI_COMM_WORLD);            
            }

            
            for(int i=1; i<=nbSlaves; i++){
                float recv; 
                MPI_Recv(&recv, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &etat);
                temperatures[i-1] = recv; 
            }
            printArray(temperatures, nbSlaves, width); 
            cout << endl; 
        }

        MPI_Send (&end,1,MPI_CHAR, 0, 0, parent);
    }
    MPI_Finalize();
    return 0;
}
