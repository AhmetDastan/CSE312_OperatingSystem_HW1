#ifndef MMU_H
#define MMU_H

#include <iostream>

struct mmuPageTable
{
    int pageTableEntryAmount;
    bool *presentBit;
    bool *modifiedBit;
    bool *referencedBit;
    int* pageTable;
};

class MMU {
public:
    MMU();
    MMU(int pageTableEntryAmount);
    ~MMU(); 
    int mapVirtualToPhysical(int threadNum, int virtualEntryLine, int value);
    int getPhsicalAddress(); 
    void pageFaultSignal();
protected:
    void initilisePageTable(int pageTableEntryAmount);
private:
    
    bool pageFault;
    bool pageReplacement;
    
    mmuPageTable pageTable;
    
};

#endif