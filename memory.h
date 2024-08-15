#ifndef MEMORY_H
#define MEMORY_H

#include "mmu.h"

class Memory {
public:
    Memory(int frameSize, int numPhysical, int numVirtual);
    ~Memory();
    void set(unsigned int threadNum, unsigned int index, int value);
    int get(unsigned int threadNum, unsigned int index);

private:
    int frameSize;
    int numPhysical;
    
    int* physicalMemory;
    int* disk;
    int* diskTable1;
    int* diskTable2;

    int virtualMemorySize;
    int* virtualMemory1;
    int* virtualMemory2;

    MMU mmu;
};


#endif