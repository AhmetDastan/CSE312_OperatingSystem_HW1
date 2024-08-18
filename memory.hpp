#ifndef MEMORY_H
#define MEMORY_H

#include "mmu.hpp"
#include "disk.hpp"
#include "string"

using namespace std;

class Memory {
public:
    Memory(int frameSize, int numPhysical, int numVirtual,int pageTablePrintInt, string diskName);
    ~Memory();
    void set(unsigned int threadNum, unsigned int index, int value);
    int get(unsigned int threadNum, unsigned int index);
    void printEndOfStatistics();
private:
    void initiliseTables();
    int frameSize;
    int numPhysical;
    int pageTablePrintInt;
    string diskFileName;

    int** physicalMemory;
     
    int virtualMemorySize;
    int** virtualMemory1;
    int** virtualMemory2;

    Disk disk;
    MMU mmu;

    void printVirtualTable(int** virtualTable);
    void printPhysicalTable();
 

    int accessOfMemory = 0;
    int numberOfReadsForThread1 = 0;
    int numberOfReadsForThread2 = 0;
    int numberOfWritesForThread1 = 0;
    int numberOfWritesForThread2 = 0;
    int numberOfDiskPageWriteForThread1 = 0;
    int numberOfDiskPageWriteForThread2 = 0;
    int numberOfDiskPageReadForThread1 = 0;
    int numberOfDiskPageReadForThread2 = 0;

};


#endif