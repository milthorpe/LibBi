/**
 * @file
 *
 * Documentation elements.
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_INIT_HPP
#define BI_INIT_HPP

namespace bi {
/**
 * Initialise Bi.
 *
 * @param threads Number of threads.
 */
void bi_init(const int threads = 0);
}

#include "cuda/cuda.hpp"
#include "misc/omp.hpp"
#include "ode/IntegratorConstants.hpp"
#include "cuda/device.hpp"

#ifdef ENABLE_MPI
#include "boost/mpi.hpp"
#endif

// need to keep in same compilation unit as caller for bi_ode_init()
void bi::bi_init(const int threads) {
  bi_omp_init(threads);
  bi_ode_init();
  #ifdef ENABLE_CUDA
  cudaThreadSetCacheConfig(cudaFuncCachePreferL1);
  #ifdef ENABLE_MPI
  boost::mpi::communicator world;
  int rank = world.rank();
  #else
  int rank = 0;
  #endif
  int dev = chooseDevice(rank);
  //std::cerr << "Rank " << rank << " using device " << dev << std::endl;
  #endif
}

#endif
