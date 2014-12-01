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
  MPI::Init();
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_rank(MPI_COMM_WORLD, &numprocs);
  CMatrix m1(argv[1]);                 // read 1st matrix
  CMatrix m2(argv[2]);                 // read 2nd matrix
  CMatrix result(m1.height, m2.width); // allocate memory

  assert(m1.width == m2.height); // check compatibility
  assert(m2.width == m1.height); // check compatibility

  int ergSize = m1.height * m2.width;
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
	// Datentypen zum senden vorbereiten
	int arr_size = 2 + (endpos - startpos);
	double send_arr[arr_size];
	send_arr[0] = (double)startpos;
	send_arr[1] = (double)endpos;
	for (int i = 0; i < tmp_res.size() - 1; i++) {
	  send_arr[i + 2] = tmp_res[i];
	}
	// senden
	MPI_Send(tmp_res.data(), tmp_res.size(), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
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
	double recv_arr[1000];

	// empfange ergebnis und füge in matrix ein
	for (int i = 1; i < numprocs - 1; i++) {
	  if (i == numprocs - 1) {
		  //????
	  } else {
		MPI_Recv(recv_arr, div_erg, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, NULL);
		for (int j = 0; j < div_erg; j++) {
		  result.container[j + div_erg * i] = recv_arr[j];
		}
	  }
	}
  }

  // TODO multiply matrices
  MPI_Bcast(result.container, result.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
  // --- multiply matrices ---
  for (unsigned int col = 0; col < m2.width; col++) {
	for (unsigned int row = 0; row < m1.height; row++) {
	  double sum = 0.0;
	  for (unsigned int dot = 0; dot < m2.height; dot++) {
		// cout <<  "m1[" << row << "][" << dot << "] = " << m1[row][dot]
		//  << ", m2[" << dot << "][" << col << "] = " << m2[dot][col] << endl;
		sum += m1[row][dot] * m2[dot][col];
	  }
	  // cout << "result[" << row <<" ][" << col << "] = " << sum;
	  result[row][col] = sum;
	}
  }

  MPI_Reduce(result.container, NULL, 0, NULL, MPI_SUM, 0, MPI_COMM_WORLD);

  cout << "someday i will multiply here; i'm rank "
	   << MPI::COMM_WORLD.Get_rank() << " of " << MPI::COMM_WORLD.Get_size()
	   << endl;

  MPI::Finalize();

  // print matrix
  result.print();

  return 0;
}

// EOF
