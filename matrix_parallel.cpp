// file...: matrix_parallel.cpp
// desc...: parallel matrix multiplication solution -- TEMPLATE --
// <type_date_here> | <type_author_here>

#include <iostream>
#include <assert.h>
#include <vector>
#include <mpi.h>
#include "CMatrix.h"

using namespace std;

void printError(const char *progname, const char *error) {
  if (error != NULL) {
	cerr << "ERROR: " << error << endl;
  }
  cerr << "usage: " << progname << " <matrix1> <matrix2>" << endl
	   << "\twhere <matrix1> and <matrix2> are file names containing matrices."
	   << endl;
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
  if(numprocs < 2){
	  cout << "Need more than 1 Procs" << endl << flush;
	  return 0;
  }
  CMatrix m1(argv[1]);                 // read 1st matrix
  CMatrix m2(argv[2]);                 // read 2nd matrix
  CMatrix result(m1.height, m2.width); // allocate memory

  assert(m1.width == m2.height); // check compatibility
  assert(m2.width == m1.height); // check compatibility

  int ergSize = m1.height * m2.width;
  //cout << "DEBUG:"<< numprocs << "===" << ergSize << flush;
  int div_erg = ergSize / numprocs;
  int startpos, endpos;
  if (myid != 0) {
	startpos = div_erg * myid;
	endpos = div_erg * myid+ div_erg;
	if (myid == numprocs -1) {
	  endpos = ergSize;
	}
	//cout << "STARTPOS (" << myid << "): " << startpos << "\tENDPOS: " << endpos << "\tSIZE: " << endpos-startpos << endl<<flush;
	// Berechne zwischen startpos und endpos
	// int startpos = 0;//chnage later
	// int endpos = div_erg;
	int col;
	int row;
	vector<double> tmp_res;
	double tmp_erg = 0.0;
	//cout << "Befor for" << endl << flush;
	for (int pos = startpos; pos < endpos; pos++) {
	  row = pos / result.width;
	  col = pos % result.width;
	  //cout << myid << " ROW:" << row << "\tCOL:" << col << "\tPOS:" << pos << endl << flush;
	  for (int i = 0; i < m1.width; i++) {
		  tmp_erg += m1[ row][i] * m2[i][col];
	  }
	  tmp_res.push_back(tmp_erg);
	  tmp_erg = 0.0;
	}
	//cout << "After for" << endl << flush;

	// senden
	MPI_Send(tmp_res.data(), tmp_res.size(), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  } else {
	// berechnen den ersten abschnitt
	int startpos = 0;
	int endpos = div_erg;
	//cout << "STARTPOS (" << myid << "): " << startpos << "\tENDPOS: " << endpos << "\tSIZE: " << endpos-startpos << endl<<flush;
	int col;
	int row;
	vector<double> tmp_res;
	double tmp_erg = 0.0;
	for (int pos = startpos; pos < endpos; pos++) {
	  row = pos / result.width;
	  col = pos % result.width;
	  //cout << myid << " ROW:" << row << "\tCOL:" << col << "\tPOS:" << pos << endl << flush;
	  for (int i = 0; i < m1.width; i++) {
		  tmp_erg += m1[ row][i] * m2[i][col];
	  }
	  result.container[pos]=tmp_erg;
	  tmp_erg = 0.0;
	}

	// empfange ergebnis und füge in matrix ein
	for (int i = 1; i < numprocs; i++) {
	  if (i == numprocs - 1) {
		startpos = div_erg *( numprocs - 1);
		endpos = ergSize;
		double recv_arr[endpos - startpos];
		//cout << "ergsize: "<< ergSize << "\t diverg: "<< div_erg << "\tstartpos:" << startpos <<endpos-startpos << endl << flush;
		MPI_Recv(recv_arr, (endpos - startpos), MPI_DOUBLE, i, 0,
				 MPI_COMM_WORLD, NULL);
		for (int j = 0; j < (endpos - startpos); j++) {
		  //cout << "place result cell: " << j + div_erg * i<<endl << flush;
		  result.container[j + div_erg * i] = recv_arr[j];
		}
	  } else {
		double recv_arr[div_erg];
		MPI_Recv(recv_arr, div_erg, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, NULL);
		for (int j = 0; j < div_erg; j++) {
			//cout << "place result cell: " << j + div_erg * i <<endl << flush;
		  result.container[j + div_erg * i] = recv_arr[j];
		}
	  }
	}

	result.print();
  }


  MPI::Finalize();

  // print matrix

  return 0;
}

// EOF
