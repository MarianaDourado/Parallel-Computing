#include "utilsGpu.cuh"

void *GPU_allocateMemory(unsigned long long int nbOfBytes)
{
    // std::cout << "----> " << nbOfBytes << " ------ " << checkGpuMemFree() << std::endl;
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

void GPU_resetMemory(void *devicePointer, unsigned long long int nbOfBytes)
{
    handleError(cudaMemset(devicePointer, 0, nbOfBytes));
}

void GPU_freeMemory(void *memoryPointer)
{
    if (memoryPointer != NULL)
        handleError(cudaFree(memoryPointer));
}

uint checkGpuMemFree() // in MB
{
    float free_m;
    size_t free_t, total_t;
    cudaMemGetInfo(&free_t, &total_t);
    // std::cout << "( " << free_t/1048576.0 << "  ---  " << total_t/1048576.0 << " )" << std::endl;
    // std::cout << "( " << free_t << "  ---  " << total_t << " )" << std::endl;
    free_m = free_t / 1048576.0;
    return (uint)free_m;
}