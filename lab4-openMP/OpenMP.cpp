#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    srand(time(NULL));
    int sizeA = 1000;
    int sizeB = 1000;

    //allocate matrix
    int** matrixA  = (int**)malloc(sizeA * sizeof(int*));
    for(int i = 0; i < sizeA; i++){
        matrixA[i] = (int*)malloc(sizeA * sizeof(int));
    }
    int** matrixB  = (int**)malloc(sizeB * sizeof(int*));
    for(int i = 0; i < sizeB; i++){
        matrixB[i] = (int*)malloc(sizeB * sizeof(int));
    }
    int** matrixC  = (int**)malloc(sizeA * sizeof(int*));
    for(int i = 0; i < sizeA; i++){
        matrixC[i] = (int*)malloc(sizeB * sizeof(int));
    }
    for(int i = 0; i < sizeA; i++){
        for(int j = 0; j < sizeA; j++){
            matrixA[i][j] = rand();
        }
    }
    for(int i = 0; i < sizeB; i++){
        for(int j = 0; j < sizeB; j++){
            matrixB[i][j] = rand();
        }
    }

    double start = omp_get_wtime( ) ;
    #pragma  omp  parallel
    {
    fprintf(stderr, "Liczba w¹tków: %d\n",omp_get_num_threads( ) );
    fprintf(stderr, "Jestem w¹tek nr %d\n",omp_get_thread_num( ) ); 
    //#pragma omp for 
    //#pragma  omp for schedule
    //#pragma  omp for schedule(static)
    //#pragma  omp for schedule(dynamic)
    #pragma  omp for schedule(guided)
    for(int i = 0; i < sizeA; i++){
        for(int j = 0; j < sizeB; j++){
            int sum = 0;
            for(int k = 0; k < sizeA; k++){
                sum += matrixA[i][k] * matrixB[k][j];
            }
            matrixC[i][j] = sum;
            //fprintf(stderr, "%d ", sum);
        }
        //fprintf(stderr, "\n");
    }
    }
    double end=omp_get_wtime( ) ;
    float seconds = (float)(end - start);
    fprintf(stderr, "time diff = %f", seconds);
    fprintf(stderr, "Speedup = %f", 12.67/seconds);
    

    for(int i = 0; i < sizeA; i++){
        free(matrixA[i]);
    }
    for(int i = 0; i < sizeB; i++){
        free(matrixB[i]);
    }
    for(int i = 0; i < sizeA; i++){
        free(matrixC[i]);
    }
    free(matrixA);
free(matrixB);
free(matrixC);

}