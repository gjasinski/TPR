#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int number_of_batches_1k = 1000;
MPI_Request request, request2;
MPI_Status status;
MPI_Comm MPI_COMM_WORLD_NEW;

int* prepare_buffer(int buffer_size){
  int* buffer = (int*)calloc(buffer_size, sizeof(int));
  int i = 0;
  for(; i < buffer_size; i++){
    buffer[i] = i;
  }
  return buffer;
}

void mpi_send_batch(int send_size){
  int* buffer = prepare_buffer(send_size);
  fprintf(stderr, "Start sending (send) messages\n");
  double start = MPI_Wtime();
  int i = 0;
  for(; i < number_of_batches_1k; i++){
    MPI_Send(buffer, send_size, MPI_INT, 1, 0, MPI_COMM_WORLD_NEW);
  }
  int* buffer_recv = (int*)calloc(send_size, sizeof(int));
  i = 0;
  for(; i < number_of_batches_1k; i++){
    MPI_Recv(buffer_recv, send_size, MPI_INT, 1, 1, MPI_COMM_WORLD_NEW, MPI_STATUS_IGNORE);
  }
  double stop = MPI_Wtime();
  fprintf(stderr, "[send] Finished sending %d messages, every message contains %d b. Total size: %d b. Time: %f s\n", number_of_batches_1k, send_size, send_size * number_of_batches_1k, stop - start);
}

void mpi_isend_batch(int send_size){
  int* buffer = prepare_buffer(send_size);
  fprintf(stderr, "Start sending (rsend) messages\n");
  int i = 0;
  double start = MPI_Wtime();
  for(; i < number_of_batches_1k; i++){
    MPI_Isend(buffer, send_size, MPI_INT, 1, 123, MPI_COMM_WORLD_NEW, &request);
    MPI_Wait(&request, &status);
   }
  for(; i < number_of_batches_1k; i++){
    MPI_Irecv(buffer, send_size, MPI_INT, 1, 124, MPI_COMM_WORLD_NEW, &request);
    MPI_Wait(&request, &status);
  }
  double stop = MPI_Wtime();
  fprintf(stderr, "[isend] Finished sending %d messages, every message contains %d b. Total size: %d b. Time: %f s\n", number_of_batches_1k, send_size, send_size * number_of_batches_1k, stop - start);
}

void mpi_send_ping_pong(int send_size){
  int* buffer = prepare_buffer(send_size);
  int* buffer2 = (int*)calloc(send_size, sizeof(int));
  fprintf(stderr, "[ping pong] Start sending (send) messages\n");
  double start = MPI_Wtime();
  MPI_Send(buffer, send_size, MPI_INT, 1, 0, MPI_COMM_WORLD_NEW);
  MPI_Recv(buffer2, send_size, MPI_INT, 1, 0, MPI_COMM_WORLD_NEW, MPI_STATUS_IGNORE);
  double stop = MPI_Wtime();
  fprintf(stderr, "[ping pong] [send] Finished sending message, message contains %d b. Time: %f s\n", send_size, stop - start);
}

void mpi_isend_ping_pong(int send_size){
  int* buffer = prepare_buffer(send_size);
  int* buffer2 = (int*)calloc(send_size, sizeof(int));
  fprintf(stderr, "[ping pong] Start sending (send) messages\n");
  double start = MPI_Wtime();
  MPI_Isend(buffer, send_size, MPI_INT, 1, 124, MPI_COMM_WORLD_NEW, &request);
  MPI_Wait(&request, &status);
  MPI_Irecv(buffer2, send_size, MPI_INT, 1, 125, MPI_COMM_WORLD_NEW, &request2);
  MPI_Wait(&request2, &status);
  double stop = MPI_Wtime();
  fprintf(stderr, "[ping pong] [isend] Finished sending message, message contains %d b. Time: %f s\n", send_size, stop - start);
}

void receive_batched_messages(int send_size){
    int* buffer = (int*)calloc(send_size, sizeof(int));
    int i = 0;
    for(; i < number_of_batches_1k; i++){
      MPI_Recv(buffer, send_size, MPI_INT, 0, 0, MPI_COMM_WORLD_NEW, MPI_STATUS_IGNORE);
    }
    i = 0;
    for(; i < number_of_batches_1k; i++){
      MPI_Send(buffer, send_size, MPI_INT, 0, 0, MPI_COMM_WORLD_NEW);
    }
    i = 0; 
    for(; i < number_of_batches_1k; i++){
      MPI_Irecv(buffer, send_size, MPI_INT, 0, 123, MPI_COMM_WORLD_NEW, &request);
      MPI_Wait(&request, &status);
      MPI_Isend(buffer, send_size, MPI_INT, 0, 123, MPI_COMM_WORLD_NEW, &request);
    }
  }

int main(int argc, char** argv) {
  if (argc < 2){
    fprintf(stderr, "You must pass size of seneded message in B");
    return -1;
  }
  if (argc == 3){
    number_of_batches_1k = atoi(argv[2]);
  }
  int send_size = atoi(argv[1]);
  if (send_size >= 1024 * 1024){
    number_of_batches_1k = 10;
  }
  MPI_Init(NULL, NULL);
  int world_rank;

  /*
  MPI_Comm* sharedcomm = (MPI_Comm*) calloc(1, sizeof(MPI_Comm));
  MPI_Info info =0;  
  //MPI_Comm_split_type(MPI_COMM_WORLD, MPI_UNDEFINED, 0, info, sharedcomm);
  //MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, info, sharedcomm);
  MPI_COMM_WORLD_NEW = *sharedcomm;
  double window_data; 
  MPI_Win node_window;
  MPI_Win_allocate_shared(2* send_size, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD_NEW, &window_data, &node_window);
  MPI_Win_lock_all(0, node_window);
  MPI_Win_sync(node_window);  
  */
 MPI_COMM_WORLD_NEW = MPI_COMM_WORLD;

  MPI_Comm_rank(MPI_COMM_WORLD_NEW, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD_NEW, &world_size);
  if (world_size < 2) {
    fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD_NEW, 1);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if (world_rank != 1) {
    mpi_isend_batch(send_size);
  } else {
    int* buffer = (int*)calloc(send_size, sizeof(int));
    int i = 0;
    for(; i < number_of_batches_1k; i++){
      MPI_Irecv(buffer, send_size, MPI_INT, 0, 123, MPI_COMM_WORLD_NEW, &request);
      MPI_Wait(&request, &status);
    }
    for(; i < number_of_batches_1k; i++){
      MPI_Isend(buffer, send_size, MPI_INT, 0, 124, MPI_COMM_WORLD_NEW, &request);
      MPI_Wait(&request, &status);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if (world_rank != 1) {
    mpi_send_batch(send_size);
  } else {
    int* buffer = (int*)calloc(send_size, sizeof(int));
    int i = 0;
    for(; i < number_of_batches_1k; i++){
      MPI_Recv(buffer, send_size, MPI_INT, 0, 0, MPI_COMM_WORLD_NEW, MPI_STATUS_IGNORE);
    }
    i = 0;
    for(; i < number_of_batches_1k; i++){
      MPI_Send(buffer, send_size, MPI_INT, 0, 1, MPI_COMM_WORLD_NEW);
    }
  }


  MPI_Barrier(MPI_COMM_WORLD);
  if (world_rank != 1) {
    mpi_isend_ping_pong(send_size);
  } else {
    int* buffer = (int*)calloc(send_size, sizeof(int));
    MPI_Irecv(buffer, send_size, MPI_INT, 0, 124, MPI_COMM_WORLD_NEW, &request2);
    MPI_Wait(&request2, &status);
    
    MPI_Isend(buffer, send_size, MPI_INT, 0, 125, MPI_COMM_WORLD_NEW, &request2);
  }
  
  

  MPI_Barrier(MPI_COMM_WORLD);
  if (world_rank != 1) {
    mpi_send_ping_pong(send_size);
  } else {
    int* buffer = (int*)calloc(send_size, sizeof(int));
    MPI_Recv(buffer, send_size, MPI_INT, 0, 0, MPI_COMM_WORLD_NEW, MPI_STATUS_IGNORE);
    MPI_Send(buffer, send_size, MPI_INT, 0, 0, MPI_COMM_WORLD_NEW);
  }
  MPI_Finalize();
  return 0;
}

