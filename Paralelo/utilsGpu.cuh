#ifndef UTILSGPU
#define UTILSGPU

#include <cuda_runtime_api.h>
#include <cuda.h>

#include <iostream>
#include <assert.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void *GPU_allocateMemory(unsigned long long int nbOfBytes);

void GPU_setMemory(void *hostPointer, void *devicePointer, unsigned long long int nbOfBytes);
void GPU_getMemory(void *hostPointer, void *devicePointer, unsigned long long int nbOfBytes);

void GPU_resetMemory(void *devicePointer, unsigned long long int nbOfBytes);

void GPU_freeMemory(void *memoryPointer);

inline cudaError_t handleError(cudaError_t result)
{
    if (result != cudaSuccess)
    {
        fprintf(stderr, "CUDA Runtime Error: %s\n", cudaGetErrorString(result));
        assert(result == cudaSuccess);
    }
    return result;
}

inline cudaError_t handleError(cudaError_t result, int line)
{
    if (result != cudaSuccess)
    {
        fprintf(stderr, "Line: %d CUDA Runtime Error: %s\n", line, cudaGetErrorString(result));
        assert(result == cudaSuccess);
    }
    return result;
}

#endif // UTILSGPU