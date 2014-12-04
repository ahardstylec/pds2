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
int main(int argc, char **argv) {

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
  cout << "DEBUG:"<< numprocs << "===" << ergSize << flush;
  int div_erg = ergSize / numprocs;
  int startpos, endpos;
  if (myid != 0) {
	startpos = div_erg * myid;
	endpos = div_erg * myid + 1;
	if ((div_erg * myid + 2) > ergSize) {
	  endpos = ergSize;
	}
	// Berechne zwischen startpos und endpos
	// int startpos = 0;//chnage later
	// int endpos = div_erg;
	int col;
	int row;
	vector<double> tmp_res;
	double tmp_erg = 0.0;
	cout << "Befor for" << endl << flush;
	for (int pos = startpos; pos < endpos; pos++) {
	  row = pos / result.width;
	  col = pos / result.height;
	  for (int i = 0; i < m1.width; i++) {
		for (int k = 0; k < m2.height; k++) {
		  tmp_erg += m1[i + row][k + col] * m2[k + col][i + row];
		}
	  }
	  tmp_res.push_back(tmp_erg);
	  tmp_erg = 0.0;
	}
	cout << "After for" << endl << flush;
	// Datentypen zum senden vorbereiten
	int arr_size = (endpos - startpos);
	double send_arr[arr_size];
	//	send_arr[0] = (double)startpos;
	//	send_arr[1] = (double)endpos;
	for (int i = 0; i < tmp_res.size() - 1; i++) {
	  send_arr[i] = tmp_res[i];
	}
	// senden
	MPI_Send(send_arr, tmp_res.size(), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  } else {
	// berechnen den ersten abschnitt
	int startpos = 0;
	int endpos = div_erg;
	int col;
	int row;
	vector<double> tmp_res;
	double tmp_erg = 0.0;
	for (int pos = startpos; pos < endpos; pos++) {
	  row = pos / result.width;
	  col = pos / result.height;
	  for (int i = 0; i < m1.width; i++) {
		for (int k = 0; k < m2.height; k++) {
		  tmp_erg += m1[i + row][k + col] * m2[k + col][i + row];
		}
	  }
	  tmp_res.push_back(tmp_erg);
	  tmp_erg = 0.0;
	}
	for (int i = 0; i < div_erg; i++) {
	  result.container[i] = tmp_res.at(i);
	}

	// TODO korrigieren zu dynamischen array
	double recv_arr[ergSize];

	// empfange ergebnis und füge in matrix ein
	for (int i = 1; i < numprocs - 1; i++) {
	  if (i == numprocs - 1) {
		startpos = div_erg * numprocs - 1;
		endpos = ergSize;
		MPI_Recv(recv_arr, (endpos - startpos), MPI_DOUBLE, i, 0,
				 MPI_COMM_WORLD, NULL);
	  } else {
		MPI_Recv(recv_arr, div_erg, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, NULL);
		for (int j = 0; j < div_erg; j++) {
		  result.container[j + div_erg * i] = recv_arr[j];
		}
	  }
	}
  }


  MPI::Finalize();

  // print matrix
  result.print();

  return 0;
}

// EOF
