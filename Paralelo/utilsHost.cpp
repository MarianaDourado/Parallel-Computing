#include "utilsHost.hpp"
// facilita mexer memória na CPU

// verifica erro
void handleMemoryHostAllocationError(void *ptr)
{
    if (ptr == NULL)
    {
        fprintf(stderr, "Allocation NULL pointer\n");
        assert(0);
    }
}

// aloca memória 
void *allocHostMemory(unsigned long long int nbOfBytes)
{
    void *ptr = malloc(nbOfBytes);
    handleMemoryHostAllocationError(ptr);
    return ptr;
}

// libera memória
void freeHostMemory(void *ptr)
{
    if (ptr != NULL)
        free(ptr);
}

double time_in_ms(clock_t t_start, clock_t t_end)
{
    return ((1000.0 * t_end - 1000.0 * t_start) / CLOCKS_PER_SEC);
}