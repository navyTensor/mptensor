/*
  Apr. 24, 2015
  Copyright (C) 2015 Satoshi Morita
 */

#include <mpi.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>

#include <mptensor.hpp>
#include "mpi_tool.hpp"
#include "functions.hpp"
#include "typedef.hpp"

namespace tests {


//! Test for full trace of tensor
/*! trace(A, Axes(0,1), Axes(3,2))
  \f$ = \sum_{i,j} A_{ijji} \f$

  \param comm MPI communicator
  \param L size of tensor, A.shape = (L, L+1, L+1, L)
  \param ostrm output stream for results
*/
void test_trace(const MPI_Comm &comm, int L, std::ostream &ostrm) {
  using namespace mptensor;
  const double EPS = 1.0e-10;
  double time0, time1, time2, time3, time4;
  int mpirank;
  int mpisize;
  bool mpiroot;
  mpi_info(comm, mpirank, mpisize, mpiroot);

  int N0 = L;
  int N1 = L+1;

  TensorD A(Shape(N0, N1, N1, N0));
  Shape shape_A = A.shape();

  for(size_t i=0;i<A.local_size();++i) {
    Index index = A.global_index(i);
    double val = func4_1(index, shape_A);
    A.set_value(index, val);
  }

  time0 = MPI_Wtime();

  double result = trace(A, Axes(0,1), Axes(3,2));

  time1 = MPI_Wtime();

  TensorD M = transpose(A, Axes(0,1,3,2));
  Shape s = M.shape();
  M = reshape(M, Shape(s[0]*s[1], s[2]*s[3]));

  time2 = MPI_Wtime();

  double result_matrix = trace(M);

  time3 = MPI_Wtime();

  double exact = 0.0;
  for(size_t i=0;i<N0;++i) {
    for(size_t j=0;j<N1;++j) {
      exact += func4_1(Index(i,j,j,i), shape_A);
    }
  }
  double error = std::abs(result-exact)/std::abs(exact);
  double error_matrix = std::abs(result_matrix-exact)/std::abs(exact);

  time4 = MPI_Wtime();

  double max_error, max_error_matrix;
  MPI_Reduce(&error, &max_error, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
  MPI_Reduce(&error_matrix, &max_error_matrix, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

  if(mpiroot) {
    ostrm << "========================================\n"
          << "Full Trace <double> ( A[N0, N1, N1, N0], Axes(0,1), Axes(3,2) )\n"
          << "[N0, N1, N1, N0] = " <<  A.shape() << "\n"
          << "Error=          " << max_error << "\n"
          << "Error(matrix)=  " << max_error_matrix << "\n"
          << "Time=           " << time1-time0 << " [sec]\n"
          << "Time(reshape)=  " << time2-time1 << " [sec]\n"
          << "Time(matrix)=   " << time3-time2 << " [sec]\n"
          << "Time(check)=    " << time4-time3 << " [sec]\n"
          << "----------------------------------------\n";
    ostrm << "A: ";
    A.print_info(ostrm);
    ostrm << "========================================" << std::endl;
  }
  assert(error < EPS);
  MPI_Barrier(comm);
}


//! Test for full trace of tensor (complex version)
/*! trace(A, Axes(0,1), Axes(3,2))
  \f$ = \sum_{i,j} A_{ijji} \f$

  \param comm MPI communicator
  \param L size of tensor, A.shape = (L, L+1, L+1, L)
  \param ostrm output stream for results
*/
void test_trace_complex(const MPI_Comm &comm, int L, std::ostream &ostrm) {
  using namespace mptensor;
  const double EPS = 1.0e-10;
  double time0, time1, time2, time3, time4;
  int mpirank;
  int mpisize;
  bool mpiroot;
  mpi_info(comm, mpirank, mpisize, mpiroot);

  int N0 = L;
  int N1 = L+1;

  TensorC A(Shape(N0, N1, N1, N0));
  Shape shape_A = A.shape();

  for(size_t i=0;i<A.local_size();++i) {
    Index index = A.global_index(i);
    complex val = cfunc4_1(index, shape_A);
    A.set_value(index, val);
  }

  time0 = MPI_Wtime();

  complex result = trace(A, Axes(0,1), Axes(3,2));

  time1 = MPI_Wtime();

  TensorC M = transpose(A, Axes(0,1,3,2));
  Shape s = M.shape();
  M = reshape(M, Shape(s[0]*s[1], s[2]*s[3]));

  time2 = MPI_Wtime();

  complex result_matrix = trace(M);

  time3 = MPI_Wtime();

  complex exact = 0.0;
  for(size_t i=0;i<N0;++i) {
    for(size_t j=0;j<N1;++j) {
      exact += cfunc4_1(Index(i,j,j,i), shape_A);
    }
  }
  double error = std::abs(result-exact)/std::abs(exact);
  double error_matrix = std::abs(result_matrix-exact)/std::abs(exact);

  time4 = MPI_Wtime();

  double max_error, max_error_matrix;
  MPI_Reduce(&error, &max_error, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
  MPI_Reduce(&error_matrix, &max_error_matrix, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

  if(mpiroot) {
    ostrm << "========================================\n"
          << "Full Trace <complex> ( A[N0, N1, N1, N0], Axes(0,1), Axes(3,2) )\n"
          << "[N0, N1, N1, N0] = " <<  A.shape() << "\n"
          << "Error=          " << max_error << "\n"
          << "Error(matrix)=  " << max_error_matrix << "\n"
          << "Time=           " << time1-time0 << " [sec]\n"
          << "Time(reshape)=  " << time2-time1 << " [sec]\n"
          << "Time(matrix)=   " << time3-time2 << " [sec]\n"
          << "Time(check)=    " << time4-time3 << " [sec]\n"
          << "----------------------------------------\n";
    ostrm << "A: ";
    A.print_info(ostrm);
    ostrm << "========================================" << std::endl;
  }
  assert(error < EPS);
  MPI_Barrier(comm);
}


//! Test for full contraction of two tensors
/*! trace(A, B, Axes(0,3,2,1), Axes(3,2,0,1))
  \f$ = \sum_{a,b,c,d} A_{adcb} B_{dcab} \f$

  \param comm MPI communicator
  \param L size of tensor, A.shape = (L, L+1, L+2, L+3)
  \param ostrm output stream for results
*/
void test_trace2(const MPI_Comm &comm, int L, std::ostream &ostrm) {
  using namespace mptensor;
  const double EPS = 1.0e-10;
  double time0, time1, time2, time3, time4;
  int mpirank;
  int mpisize;
  bool mpiroot;
  mpi_info(comm, mpirank, mpisize, mpiroot);

  int N0 = L;
  int N1 = L+1;
  int N2 = L+2;
  int N3 = L+3;

  TensorD A(Shape(N0, N1, N2, N3));
  TensorD B(Shape(N2, N1, N3, N0));

  Shape shape_A = A.shape();
  for(size_t i=0;i<A.local_size();++i) {
    Index index = A.global_index(i);
    double val = func4_1(index, shape_A);
    A.set_value(index, val);
  }

  Shape shape_B = B.shape();
  for(size_t i=0;i<B.local_size();++i) {
    Index index = B.global_index(i);
    double val = func4_2(index, shape_B);
    B.set_value(index, val);
  }

  time0 = MPI_Wtime();

  double result = trace(A, B, Axes(0,3,2,1), Axes(3,2,0,1));

  time1 = MPI_Wtime();

  double exact = 0.0;
  for(size_t i=0;i<N0;++i) {
    for(size_t j=0;j<N1;++j) {
      for(size_t k=0;k<N2;++k) {
        for(size_t l=0;l<N3;++l) {
          exact += func4_1(Index(i,j,k,l), shape_A) * func4_2(Index(k,j,l,i), shape_B);
        }
      }
    }
  }
  double error = std::abs(result-exact)/std::abs(exact);

  time2 = MPI_Wtime();

  double max_error;
  MPI_Reduce(&error, &max_error, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

  if(mpiroot) {
    ostrm << "========================================\n"
          << "Trace_2 <double> ( A[N0, N1, N2, N3], B[N2, N1, N3, N0], Axes(0,3,2,1), Axes(3,2,0,1) )\n"
          << "[N0, N1, N2, N3] = " <<  A.shape() << "\n"
          << "Result=         " << result << "\n"
          << "Exact=          " << exact << "\n"
          << "Error=          " << max_error << "\n"
          << "Time=           " << time1-time0 << " [sec]\n"
          << "Time(check)=    " << time2-time1 << " [sec]\n"
          << "----------------------------------------\n";
    ostrm << "A: ";
    A.print_info(ostrm);
    ostrm << "B: ";
    B.print_info(ostrm);
    ostrm << "========================================" << std::endl;
  }
  assert(error < EPS);
  MPI_Barrier(comm);
}


//! Test for full contraction of two tensors (complex version)
/*! trace(A, B, Axes(0,3,2,1), Axes(3,2,0,1))
  \f$ = \sum_{a,b,c,d} A_{adcb} B_{dcab} \f$

  \param comm MPI communicator
  \param L size of tensor, A.shape = (L, L+1, L+2, L+3)
  \param ostrm output stream for results
*/
void test_trace2_complex(const MPI_Comm &comm, int L, std::ostream &ostrm) {
  using namespace mptensor;
  const double EPS = 1.0e-10;
  double time0, time1, time2, time3, time4;
  int mpirank;
  int mpisize;
  bool mpiroot;
  mpi_info(comm, mpirank, mpisize, mpiroot);

  int N0 = L;
  int N1 = L+1;
  int N2 = L+2;
  int N3 = L+3;

  TensorC A(Shape(N0, N1, N2, N3));
  TensorC B(Shape(N2, N1, N3, N0));

  Shape shape_A = A.shape();
  for(size_t i=0;i<A.local_size();++i) {
    Index index = A.global_index(i);
    complex val = cfunc4_1(index, shape_A);
    A.set_value(index, val);
  }

  Shape shape_B = B.shape();
  for(size_t i=0;i<B.local_size();++i) {
    Index index = B.global_index(i);
    complex val = cfunc4_2(index, shape_B);
    B.set_value(index, val);
  }

  time0 = MPI_Wtime();

  complex result = trace(A, B, Axes(0,3,2,1), Axes(3,2,0,1));

  time1 = MPI_Wtime();

  complex exact = 0.0;
  for(size_t i=0;i<N0;++i) {
    for(size_t j=0;j<N1;++j) {
      for(size_t k=0;k<N2;++k) {
        for(size_t l=0;l<N3;++l) {
          exact += cfunc4_1(Index(i,j,k,l), shape_A) * cfunc4_2(Index(k,j,l,i), shape_B);
        }
      }
    }
  }
  double error = std::abs(result-exact)/std::abs(exact);

  time2 = MPI_Wtime();

  double max_error;
  MPI_Reduce(&error, &max_error, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

  if(mpiroot) {
    ostrm << "========================================\n"
          << "Trace_2 <complex> ( A[N0, N1, N2, N3], B[N2, N1, N3, N0], Axes(0,3,2,1), Axes(3,2,0,1) )\n"
          << "[N0, N1, N2, N3] = " <<  A.shape() << "\n"
          << "Result=         " << result << "\n"
          << "Exact=          " << exact << "\n"
          << "Error=          " << max_error << "\n"
          << "Time=           " << time1-time0 << " [sec]\n"
          << "Time(check)=    " << time2-time1 << " [sec]\n"
          << "----------------------------------------\n";
    ostrm << "A: ";
    A.print_info(ostrm);
    ostrm << "B: ";
    B.print_info(ostrm);
    ostrm << "========================================" << std::endl;
  }
  assert(error < EPS);
  MPI_Barrier(comm);
}


} // namespace tests
