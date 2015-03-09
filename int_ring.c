#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include "util.h"
#define SIZE 250000


/*The commented code is used for sending arrays*/

int main( int argc, char *argv[])
{

  if (argc != 2) {
    fprintf(stderr, "I need exactly 1 parameter, what's wrong with you!\n");
    abort();
  }

  int N = atoi(argv[1]);
  int it = N;
  int rank, tag, origin, destination;
  MPI_Status status;
  int nprocs;
  timestamp_type time1, time2;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  if (nprocs < 2) {
    fprintf(stderr, "Go away, I am parallel computing not a serial one!\n");
    abort();
  }

  tag = it;
  int message_out;
  int message_in;
/*  double* in = malloc(sizeof(double) * SIZE);
  double* out = malloc(sizeof(double) * SIZE);
*/
  destination = rank + 1;
  origin = rank - 1;

  if (destination == nprocs){
    destination = 0;
  }

  if(origin < 0){
    origin = nprocs - 1;
  }

  while(it > 0){
   if(rank == 0 && it == N){
      get_timestamp(&time1);
      message_out = 0;
    }else{
      MPI_Recv(&message_in,  1, MPI_INT, origin, tag, MPI_COMM_WORLD, &status);
      message_out = message_in + rank;
    }
    if(rank == (nprocs - 1)){
      tag = tag - 1;
    }
    MPI_Send(&message_out, 1, MPI_INT, destination, tag, MPI_COMM_WORLD);
    it = it - 1;
    tag = it;



 /*   if(rank == 0 && it == N){
      get_timestamp(&time1);
    }else{
      MPI_Recv(out,  SIZE, MPI_DOUBLE, origin, tag, MPI_COMM_WORLD, &status);
    }
    if(rank == (nprocs - 1)){
      tag = tag - 1;
    }
    MPI_Send(in, SIZE, MPI_DOUBLE, destination, tag, MPI_COMM_WORLD);
    it --;
    tag = it;
    */
  }


  if(rank == 0 && it == 0){
    MPI_Recv(&message_in,  1, MPI_INT, origin, tag, MPI_COMM_WORLD, &status);
    get_timestamp(&time2);
    double elapsed = timestamp_diff_in_seconds(time1,time2);
    printf("%d\n", message_in); 
    printf("Time elapsed is %f seconds.\n", elapsed);
  }
/*   if(rank == 0 && it == 0){
      MPI_Recv(out,  SIZE, MPI_DOUBLE, origin, tag, MPI_COMM_WORLD, &status);
      get_timestamp(&time2);
      double elapsed = timestamp_diff_in_seconds(time1,time2);
      printf("Time elapsed is %f seconds.\n", elapsed);
    }*/

  MPI_Finalize();

  return 0;
}
