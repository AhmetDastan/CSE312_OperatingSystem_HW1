#ifndef MEMORY_H
#define MEMORY_H

#include "mmu.hpp"
#include "disk.hpp"
#include "string"

using namespace std;

class Memory {
public:
    Memory(int frameSize, int numPhysical, int numVirtual,string diskName);
    ~Memory();
    void set(unsigned int threadNum, unsigned int index, int value);
    int get(unsigned int threadNum, unsigned int index);

private:
    void initiliseTables();
    int frameSize;
    int numPhysical;
    string diskFileName;

    int** physicalMemory;
     
    int virtualMemorySize;
    int** virtualMemory1;
    int** virtualMemory2;

    Disk disk;
    MMU mmu;

    void printVirtualTable(int** virtualTable);
    void printPhysicalTable();
};


#endif