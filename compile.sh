#!/bin/bash 

mpic++ Processus/Master.cpp 		-o Master
mpic++ Processus/Coordinator.cpp  	-o Coordinator
mpic++ Processus/Slave.cpp  		-o Slave

mpirun -np 1 ./Master 