#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int number_of_batches_1k = 10000;

int* prepare_buffer(int buffer_size){
  int* buffer = (int*)calloc(send_size * sizeof(int));
  for(int i = 0; i < buffer_size; i++){
    buffer[i] = i;
  }
  return buffer;
}

void mpi_send_batch(int send_size){
  int* buffer = prepare_buffer(send_size);
  fprintf(stderr, "Start sending (send) messages");
  for(int i = 0; i < number_of_batches_1k; i++){
    MPI_Send(buffer, send_size, MPI_INT, 1, 0, MPI_COMM_WORLD);
  }
  fprintf(stderr, "Finished sending (send) messasges");
}

void mpi_isend_batch(int send_size){
  int* buffer = prepare_buffer(send_size);
  fprintf(stderr, "Start sending (rsend) messages");
  for(int i = 0; i < number_of_batches_1k; i++){
    MPI_Isend(buffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
  }
  fprintf(stderr, "Finished sending (rsend) messasges");
}

int main(int argc, char** argv) {
  if (argc != 2){
    fprintf(stderr, "You must pass size of seneded message in B");
    return -1;
  }
  MPI_Init(NULL, NULL);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  if (world_size < 2) {
    fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  MPI_Request request, request2;
  MPI_Status status;
  if (world_rank != 1) {
    mpi_send_batch(100);
  } else {
    int* buffer = (int*)calloc(100 * sizeof(int));
    for(int i = 0 ; i < number_of_batches_1k; i++){
      MPI_Recv(&number, 100, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("Process 1 received ");
      for(int j = 0; j < 100; j++){
        fprintf(stderr, "%d", buffer[j]);
      }
      fprintf(stderr, "\n");
    }
    /*
    MPI_Irecv(&number, 1, MPI_INT, 0, 123, MPI_COMM_WORLD, &request2);
    MPI_Wait(&request2, &status);
    printf("Process 1 received number %d from process 0\n", number);
    */
  }
  MPI_Finalize();
}

