#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() 
{
    MPI_Init(NULL, NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int all = 0;
    int hit = 0;
    time_t tt;
    int zarodek = time(&tt);
    srand(zarodek);
    for(int i = 0; i < 10000000; i++){
        double x = drand48();
        double y = drand48();
         
        if(x*x + y*y <= 1.0){
            hit++;
        }
        all++;
    }
    double pi = ((double)hit/(double)all)*4.0;
    fprintf(stderr, "PI: %d\n", hit);
    fprintf(stderr, "PI: %d\n", all);
    fprintf(stderr, "PI: %f\n", pi);
}