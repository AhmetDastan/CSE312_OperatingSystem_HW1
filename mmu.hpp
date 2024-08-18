/*  
    I have 2 page table in MMU class for managenin my 2 thread
    This tables try to finding and mapping virtual address to physical address
*/

#ifndef MMU_H
#define MMU_H

#include <iostream>

struct mmuPageTable
{
    int pageTableEntryAmount;
    bool *presentBit;
    bool *modifiedBit;
    bool *referencedBit;
    int *pageFrame;
};
 

class MMU {
public:
    MMU(){};
    MMU(int virtualEntryAmount, int physicalEntryAmount);
    ~MMU(); 
    int* mapVirtualToPhysical(int threadNum, int virtualEntryLine);
    int getPhsicalAddress(); 
    void pageFaultSignal(); 
    
    int* clockPageAlrorithm(int threadNum, int virtualEntryLine); 

    void printPageTable1();
    void printPageTable2();

    int pageReplacementCounterForThread1 = 0;
    int pageReplacementCounterForThread2 = 0;
    int pageMissCounterForThread1 = 0;
    int pageMissCounterForThread2 = 0;
private:
    void initilisePageTables(int pageTableEntryAmount);
    void initilisePhysicalReferenceTable(int physicalEntryAmount);
    bool pageFault;
    bool pageReplacement;

    int physicalEntryAmount;
    int *physicalReferenceTable;

    int clockAlgorithmCurrentPoint;
     
protected:  
    mmuPageTable pageTable1, pageTable2; 

    
    void printPhysicalReferenceTable();
 
};

#endif