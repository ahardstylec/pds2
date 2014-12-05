// file...: matrix_parallel.cpp
// desc...: parallel matrix multiplication solution -- TEMPLATE --
// <type_date_here> | <type_author_here>

#include <iostream>
#include <assert.h>
#include <vector>
#include <mpi.h>
#include <stdlib.h>
#include "CMatrix.h"
#include <unistd.h>

using namespace std;

struct MatrixRange {
    int start;
    int end;
    int size;
    int matrixsize;
    int proc_id;
};

void printError(const char *progname, const char *error) {
  if (error != NULL) {
	cerr << "ERROR: " << error << endl;
  }
  cerr << "usage: " << progname << " <matrix1> <matrix2>" << endl
	   << "\twhere <matrix1> and <matrix2> are file names containing matrices."
	   << endl;
}

void createMatrixRange(struct MatrixRange * range, CMatrix *a, CMatrix *b, int proc_id,int numprocs){
    range->matrixsize = a->width*b->height;
    int div_erg = range->matrixsize / numprocs;
    range->start = div_erg * proc_id;
    if (proc_id !=  numprocs -1){
        range->size = div_erg;
        range->end = range->size * proc_id + range->size;
    }else{
        range->size = range->matrixsize - range->start;
        range->end=range->matrixsize;
    }
    range->proc_id=proc_id;
}

// +++ main starts here +++
int main(int argc, char *argv[]) {

    // process arguments
    if (argc != 3) {
        printError(argv[0], "wrong number of arguments.");
        return -1;
    }

    int myid, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    CMatrix m1(argv[1]);                 // read 1st matrix
    CMatrix m2(argv[2]);                 // read 2nd matrix
    CMatrix result(m1.height, m2.width); // allocate memory

    assert(m1.width == m2.height); // check compatibility
    assert(m2.width == m1.height); // check compatibility

    MatrixRange my_range;
    createMatrixRange(&my_range, &m1,&m2, myid, numprocs);

    double tmp_erg = 0.0;
    int row,col;
    for (int pos = my_range.start; pos < my_range.end; pos++, tmp_erg=0.0) {
        row = pos / result.width;
        col = pos % result.height;
        for (int i = 0; i < m1.width; i++) {
            tmp_erg += m1[row][i] * m2[i][col];
        }
        result.container[pos]=tmp_erg;
    }

//    cout << "After Matrix Calc" << endl << flush;

    if (myid != 0) {
//        cout << "before send"<<endl<<flush;
        MPI_Send(&result.container[my_range.start], my_range.size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } else {
        // empfange ergebnisse und füge in matrix ein
        if(numprocs > 1){
            MPI_Status status;
            for (int i = 1; i < numprocs; i++) {
                MatrixRange proc_range;
                createMatrixRange(&proc_range, &m1, &m2, i, numprocs);
//                printf("from %d size: %d\t start: %d \n", i, proc_range.size, proc_range.start); fflush(stdout);
                MPI_Recv(&result.container[proc_range.start], proc_range.size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
            }
        }
        // print matrix
        result.print();
    }
    MPI::Finalize();

    return 0;
}

// EOF
