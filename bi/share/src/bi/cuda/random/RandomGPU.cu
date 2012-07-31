/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#include "RandomGPU.cuh"

#include "RandomKernel.cuh"
#include "../../cuda/device.hpp"

using namespace bi;

void RandomGPU::seeds(Random& rng, const unsigned seed) {
  dim3 Db, Dg;
  Db.x = deviceIdealThreadsPerBlock();
  Dg.x = deviceIdealThreads()/Db.x;

  kernelSeeds<<<Dg,Db>>>(Random(rng), seed);
  // ^ copy of *this here seems necessary, otherwise the copy constructor is
  //   not called, and rng is destroyed on kernel exit!
  CUDA_CHECK;
}
