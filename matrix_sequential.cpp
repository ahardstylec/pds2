// file...: matrix_sequential.cpp
// desc...: minimalistic matrix multiplication solution
// oct-2010 | a.knirsch@fbi.h-da.de

#include <iostream>
#include <assert.h>
#include "CMatrix.h"
#include <mpi.h>

using namespace std;

void printError(const char* progname, const char* error) {
    if(error != NULL) {
        cerr << "ERROR: " << error << endl;
    }
    cerr << "usage: " << progname << " <matrix1> <matrix2>" << endl
    << "\twhere <matrix1> and <matrix2> are file names containing matrices." << endl;
}


// +++ main starts here +++
int main(int argc, char** argv) {

    // process arguments
    if(argc != 3) {
        printError(argv[0], "wrong number of arguments.");
        return -1;
    }
    double startwtime = 0.0, endwtime;

    MPI_Init(&argc, &argv);
    startwtime = MPI_Wtime();

    CMatrix m1(argv[1]);                    // read 1st matrix
    CMatrix m2(argv[2]);                    // read 2nd matrix

    assert(m1.width == m2.height);          // check compatibility

    CMatrix result( m2.width, m1.height );      // allocate memory

    // --- multiply matrices ---
    for(unsigned int col = 0; col < m2.width; col++) {
        for(unsigned int row = 0; row < m1.height; row++) {
            double sum = 0.0;
            for(unsigned int dot = 0; dot < m2.height; dot++) {
                // cout <<  "m1[" << row << "][" << dot << "] = " << m1[row][dot]
                //  << ", m2[" << dot << "][" << col << "] = " << m2[dot][col] << endl;
                sum += m1[row][dot] * m2[dot][col];
            }
            // cout << "result[" << row <<" ][" << col << "] = " << sum;
            result[row][col] = sum;
        }
    }

    endwtime = MPI_Wtime();
    cout << result.width << " " << result.height << " " << endwtime-startwtime << endl;

    MPI::Finalize();
    // print matrix
//    result.print();

    return 0;
}

// EOF

