#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long calcMonteCarlo(int points){
    	int hit = 0;
        int i = 0;
   	for(i; i < points; i++){
        	double x = drand48();
        	double y = drand48(); 
        	if(x*x + y*y <= 1.0){
	            hit++;
                }
        }
        return hit;
}

int main(int argc, char* argv[]) 
{
    if(argc < 0){
        fprintf(stderr, "Missing argumetns");
	return -1;
    } 
    
    MPI_Init(NULL, NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int iterations;
    int scal = atol(argv[2]);
    if (scal == 0){
        iterations = atol(argv[1]) / world_size;
    } 
    else {
        iterations = atol(argv[1]);
    }
    int root = 0;
    time_t curr_time;
    int seed = time(&curr_time) - world_rank;
    srand(seed);
    double t1, t2;
    MPI_Barrier(MPI_COMM_WORLD);  
    if(world_rank == 0){
       t1 = MPI_Wtime();
       long recv = 0;
       long calculatedPoints = calcMonteCarlo(iterations);
       MPI_Reduce(&calculatedPoints, &recv, 1,  MPI_LONG, MPI_SUM, root, MPI_COMM_WORLD);
//       fprintf(stderr, "ala %ld", recv);
       double pi = (double)(recv * 4) / (double)(iterations * world_size);
       t2 = MPI_Wtime();
       fprintf(stderr, "World size: %d, iterations: %d, scaled: %d, Result PI: %f. Time: %f\n", world_size, iterations * world_size, scal, pi, t2 - t1);
    }
    else {  
       long calculatedPoints = calcMonteCarlo(iterations);
       MPI_Reduce(&calculatedPoints, NULL, 1,  MPI_LONG, MPI_SUM, root, MPI_COMM_WORLD);

    }  
    MPI_Finalize(); 
    return 0;
}   


