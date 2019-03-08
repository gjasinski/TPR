#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  MPI_Init(NULL, NULL);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
//  MPI_Barrier(MPI_COMM_WORLD);
  // We are assuming at least 2 processes for this task
  if (world_size < 2) {
    fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  int number;
  MPI_Request request, request2;
MPI_Status status;
  if (world_rank != 1) {
    // If we are rank 0, set the number to -1 and send it to process 1
    number = world_rank;
    MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    int number2 = number+1;
    MPI_Ssend(&number2, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    int number3 = number2+1;
    MPI_Buffer_attach(malloc(400), 400);
    MPI_Bsend(&number3, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    int number4 = number3 +1;
    MPI_Rsend(&number4, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    int number5=number4+1;
  //  MPI_Barrier(MPI_COMM_WORLD);
//  sleep(1);
    MPI_Isend(&number5, 1, MPI_INT, 1, 123, MPI_COMM_WORLD,&request);
    MPI_Wait(&request, &status);

  } else if (world_rank == 1) {
          int i = 0;
          for(i =0 ; i < 4; i++){
                MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("Process 1 received number %d from process 0\n", number);
          }
          number =0;
//        MPI_Barrier(MPI_COMM_WORLD);
sleep(1);
          MPI_Irecv(&number, 1, MPI_INT, 0, 123, MPI_COMM_WORLD, &request2);
        //   printf("Process 1 received number %d from process 0\n", number);

    MPI_Wait(&request2, &status);
    printf("Process 1 received number %d from process 0\n", number);
  }
  MPI_Finalize();
}

