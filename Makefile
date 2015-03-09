CXX=mpicc
CPPFLAGS=-O3

all: int_ring jacobi-mpi

int_ring: int_ring.c
	$(CXX) $(CPPFLAGS) int_ring.c -o int_ring

jacobi-mpi: jacobi-mpi.c
	$(CXX) $(CPPFLAGS) jacobi-mpi.c -o jacobi-mpi

clean:
	rm -rf *o int_ring jacobi-mpi