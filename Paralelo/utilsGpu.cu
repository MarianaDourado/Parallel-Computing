#include "utilsGpu.cuh"

void *GPU_allocateMemory(unsigned long long int nbOfBytes)
{
    //std::cout << "----> " << nbOfBytes << " ------ " << checkGpuMemFree() << std::endl;
    void *ptr;
    handleError(cudaMalloc((void **)&ptr, nbOfBytes));
    return ptr;
}

void GPU_setMemory(void *hostPointer, void *devicePointer, unsigned long long int nbOfBytes)
{
    handleError(cudaMemcpy(devicePointer, hostPointer, nbOfBytes, cudaMemcpyHostToDevice));
}

void GPU_getMemory(void *hostPointer, void *devicePointer, unsigned long long int nbOfBytes)
{
    handleError(cudaMemcpy(hostPointer, devicePointer, nbOfBytes, cudaMemcpyDeviceToHost));
}

// reset = memset
void GPU_resetMemory(void *devicePointer, unsigned long long int nbOfBytes)
{
    handleError(cudaMemset(devicePointer, 0, nbOfBytes));
}

void GPU_freeMemory(void *memoryPointer)
{
    if (memoryPointer != NULL)
        handleError(cudaFree(memoryPointer));
}