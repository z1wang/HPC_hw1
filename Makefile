CXX=mpicc

all: int_ring jacobi-mpi

int_ring: int_ring.c
	$(CXX) int_ring.c -o int_ring

jacobi-mpi: jacobi-mpi.c
	$(CXX) jacobi-mpi.c -o jacobi-mpi

clean:
	rm -rf *o int_ring jacobi-mpi