#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdlib.h>


using namespace std; 


string colorFromTemp(float temp){
    int n = temp*100/60; 
    ostringstream result; 
    if(n < 0)   n = 0; 
    if(n > 100) n = 100;

    result << "hsl(" << (100-n)*160/100 << ",80\%,50\%)";
    return result.str(); 
}


int* getSlaveXYFromId(int id, int width){
    int* result = new int[2];
    result[0] = (int) (id-1)%width; //x    
    result[1] = (int) (id-1)/width; //y
    return result; 
}


void printArray(int masterWidth, int masterHeight, int slaveWidth, int slaveHeight, int masterX, int masterY, float* values, int length){
    float svg_w_100 = 100/masterWidth; 
    float svg_h_100 = 100/masterHeight;

    float svg_start_x = masterX * svg_w_100; 
    float svg_start_y = masterY * svg_h_100; 

    int x = 0; int y = 0; 

    for(int i = 0; i < length; i++){

        float svg_x = svg_start_x + (x*svg_w_100/slaveWidth);
        float svg_w = (svg_w_100/slaveWidth); 
        float svg_y = svg_start_y + (y*svg_h_100/slaveHeight);
        float svg_h = (svg_h_100/slaveHeight); 

        float svg_t_x = svg_x + svg_w*1;
        float svg_t_y = svg_y + svg_h*0.5;

        cout << "<rect stroke='black' fill='"<< colorFromTemp(values[i]) <<"' x='"<<(svg_x)<<"\%' y='"<<(svg_y)<<"\%' width='"<<svg_w<<"\%' height='"<<svg_h<<"\%'></rect>";    
        cout << "<text fill='white' text-anchor='end' x='"<<svg_t_x<<"\%' y='"<<svg_t_y<<"\%' >"<<fixed << setprecision(1)<<values[i]<<"</text>" << endl; 
        x++; 
        if(x==slaveWidth){
            x = 0; 
            y++;
        }
    }
}

void printBoundaries(int masterWidth, int masterHeight, int slaveWidth, int slaveHeight, int masterX, int masterY){
    float svg_w_100 = 100/masterWidth; 
    float svg_h_100 = 100/masterHeight;

    float svg_start_x = masterX * svg_w_100; 
    float svg_start_y = masterY * svg_h_100; 
    cout << "<rect fill='transparent' stroke-width='5' stroke='blue' x='"<<(svg_start_x)<<"\%' y='"<<(svg_start_y)<<"\%' width='"<<svg_w_100<<"\%' height='"<<svg_h_100<<"\%'></rect>";    
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

        int masterHeight; 
        int masterWidth; 
        int slaveHeight; 
        int slaveWidth;         
        float ambiantTemperature; 

        char end = 'c';

        // ----------------- Initialisation -----------------
        // 1) Reception du père
        MPI_Recv (&masterWidth,         1,MPI_INT,0,0,parent, &etat);
        MPI_Recv (&masterHeight,        1,MPI_INT,0,0,parent, &etat);    
        MPI_Recv (&slaveWidth,          1,MPI_INT,0,0,parent, &etat);
        MPI_Recv (&slaveHeight,         1,MPI_INT,0,0,parent, &etat);            
        MPI_Recv (&ambiantTemperature,  1,MPI_FLOAT,0,0,parent, &etat);

        float** plaques = (float **) malloc(masterHeight*masterWidth*sizeof(float*)); 
        for(int i = 0; i < masterHeight*masterWidth; i++){
            plaques[i] = (float *) malloc(slaveWidth*slaveHeight*sizeof(float));
        }

        for(int i = 0; i < masterHeight*masterWidth; i++){
            MPI_Recv (plaques[i],         (slaveWidth*slaveHeight),MPI_FLOAT,i+1,0,MPI_COMM_WORLD,&etat);
        }


        cout << "<svg height='500' width='1000'>";
        for(int i = 0; i < masterHeight*masterWidth; i++){
            int* coords = getSlaveXYFromId(i+1, masterWidth); 
            int x = coords[0];
            int y = coords[1];            
            printArray(masterWidth, masterHeight, slaveWidth, slaveHeight, x, y, plaques[i], slaveHeight*slaveWidth); 
        }
        for(int i = 0; i < masterHeight*masterWidth; i++){
            int* coords = getSlaveXYFromId(i+1, masterWidth); 
            int x = coords[0];
            int y = coords[1];            
            printBoundaries(masterWidth, masterHeight, slaveWidth, slaveHeight, x, y); 
        }

        cout << "</svg>";
        //float* temperatures = new float[masterHeight*masterWidth]; 



        MPI_Send (&end,1,MPI_CHAR, 0, 0, parent);
    }

    MPI_Finalize();

    return 0;
}
