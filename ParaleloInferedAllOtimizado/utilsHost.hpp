#ifndef UTILSHOST
#define UTILSHOST

#include <iostream>
#include <assert.h>
#include <cstring>

//--------------------------------------------------------------------

void handleMemoryHostAllocationError(void *ptr);
void *allocHostMemory(unsigned long long int nbOfBytes);
void freeHostMemory(void *ptr);
void memsetHostMemory(void *ptr, unsigned int size, int value = 0);

double time_in_ms(clock_t t_start, clock_t t_end);

//--------------------------------------------------------------------

#endif // UTILSHOST