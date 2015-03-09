#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"

//This program is for iteration less than 999999 because of final tag
//The output is the final residual
double residue(int n, double* u){
  double res = 0;
  int s = n - 1;

  //unit is 1/h^2
  double unit = (n + 1) * (n + 1);  

  //temp is A * u - f
  double *temp = malloc(sizeof(double) * n);
  int i;
  for (i = 0; i < n; i++){
    temp[i] = 2 * u[i];
    if(i == 0){
      temp[i] = temp[i] - u[i + 1];
    }else if(i == s){
      temp[i] = temp[i] - u[i - 1];
    }else{
      temp[i] = temp[i] - u[i - 1] - u[i + 1];
    }
    temp[i] = temp[i] * unit;
    temp[i] = temp[i] - 1;
    res += temp[i] * temp[i];
  }
  free(temp);
  res = sqrt(res);
//  printf("residue is %f\n", res);
  return res;
}

double * jacobi(int n, int size, double * init){
  double *ret = malloc(sizeof(double) * (size + 2));
  int i;

  //unit is 1/h^2
  double unit = (n + 1) * (n + 1);
  for (i = 0; i < size; i++){
  //compute f_i - a * u
    if(i == 0){
      ret[i] = 1 + unit * init[i + 1] + unit * init[size];
    }else if(i == (size - 1)){
      ret[i] = 1 + unit * init[i - 1] + unit * init[size + 1];
    }else{
      ret[i] = 1 + unit * init[i - 1] + unit * init[i + 1];
    }
    ret[i] = ret[i] / 2;
    ret[i] = ret[i] / unit;
  } 
  free(init);
  return ret;
}

int main( int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(stderr, "I need exactly 2 parameters, what's wrong with you!\n");
    abort();
  }

// N is the size of stencile, F is the size of iteration
  int N = atoi(argv[1]);
  int F = atoi(argv[2]);
  int it = F;
  int rank, tag, origin, destination, i;
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

  if(N % nprocs != 0){
    fprintf(stderr, "N is not a multiple of cores\n");
    abort();
  }

  int size = N / nprocs;

  double message_out1;
  double message_out2;
  double message_in1;
  double message_in2;
  tag = it;

  destination = rank + 1;
  destination = destination % nprocs;
  origin = rank - 1;
  origin = origin % nprocs;

//initial vector
  double * init = malloc(sizeof(double) * (size + 2));

  while(it > 0){
    if(it == F){
      double unit = (N + 1) * (N + 1);
      for(i = 0; i < size; i++){
        init[i] = 0.5 / unit;
      }
      if(rank == 0){
        get_timestamp(&time1);  
        message_out1 = init[0];
        MPI_Send(&message_out1, 1, MPI_DOUBLE, destination, tag, MPI_COMM_WORLD);
        it --;
        tag = it;        
      }else if(rank == (nprocs - 1)){
        message_out2 = init[size - 1];
        MPI_Send(&message_out2, 1, MPI_DOUBLE, origin, tag, MPI_COMM_WORLD);
        it --;
        tag = it;

      }else{
        message_out1 = init[0];
        message_out2 = init[size - 1];
        MPI_Send(&message_out1, 1, MPI_DOUBLE, destination, tag, MPI_COMM_WORLD);
        MPI_Send(&message_out2, 1, MPI_DOUBLE, origin, tag, MPI_COMM_WORLD);
        it --;
        tag = it;
      }
    }else{
      if(rank == 0){
        MPI_Recv(&message_in2,  1, MPI_DOUBLE, destination, tag + 1, MPI_COMM_WORLD, &status);
        init[size] = 0;
        init[size + 1] = message_in2;
        init = jacobi(N, size, init);
        message_out1 = init[size - 1];
        MPI_Send(&message_out1, 1, MPI_DOUBLE, destination, tag, MPI_COMM_WORLD);
        it --;
        tag = it;
      }else if(rank == (nprocs - 1)){
        MPI_Recv(&message_in1,  1, MPI_DOUBLE, origin, tag + 1, MPI_COMM_WORLD, &status);
        init[size] = message_in1;
        init[size + 1] = 0;
        init = jacobi(N, size, init);
        message_out2 = init[0];
        MPI_Send(&message_out2, 1, MPI_DOUBLE, origin, tag, MPI_COMM_WORLD);
        it --;
        tag = it;
      }else{
        MPI_Recv(&message_in1,  1, MPI_DOUBLE, origin, tag + 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&message_in2,  1, MPI_DOUBLE, destination, tag + 1, MPI_COMM_WORLD, &status);
        init[size] = message_in1;
        init[size + 1] = message_in2;
        init = jacobi(N, size, init);
        message_out1 = init[0];
        message_out2 = init[size - 1];
        MPI_Send(&message_out1, 1, MPI_DOUBLE, origin, tag, MPI_COMM_WORLD);
        MPI_Send(&message_out2, 1, MPI_DOUBLE, destination, tag, MPI_COMM_WORLD);
        it --;
        tag = it;
      }
    }
  }

/* This part is used to verify the final result is correct
   It prints out the result vector from each processor
   It has been commented */

/* if (it == 0){
  printf(Rank is %d:\n, rank);
  int j;
  for(j = 0; j < size; j++){
    printf("%f ", init[j]);
  }
  printf("\n");
}
*/
  if(it == 0){
    if(rank == 0){
      double * result = malloc(sizeof(double) * N);
      int j;
      for (j = 0; j < size; j++){
       result[j] = init[j];
      }
      int index = size;
      for(j = 1; j < nprocs; j++){
        MPI_Recv(&result[index],  size, MPI_DOUBLE, j, 999999, MPI_COMM_WORLD, &status);
        index = index + size;
      }
      double ie = residue(N, result);
      free(result);
      printf("residue is %f.\n", ie);
    }else{
      MPI_Send(init, size, MPI_DOUBLE, 0, 999999, MPI_COMM_WORLD);
    }
  }
  free(init);
  
  MPI_Finalize();

  return 0;
}
