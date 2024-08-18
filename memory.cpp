#include <iostream>
#include "memory.hpp" 
#include <cmath> 

using namespace std;

Memory::Memory(int frameSize, int numPhysical, int numVirtual, int pageTablePrintInt ,string diskName){
    this->diskFileName = diskName;
    this->frameSize = static_cast<int>(pow(2,frameSize));
    this->numPhysical = static_cast<int>(pow(2,numPhysical));
    this->pageTablePrintInt = pageTablePrintInt;

    virtualMemorySize = static_cast<int>(pow(2,numVirtual));
    physicalMemory = (int**)malloc(sizeof(int*) * this->numPhysical);
    for(int i = 0; i < this->numPhysical; i++) physicalMemory[i] = (int*)malloc(sizeof(int) * this->frameSize);
    virtualMemory1 = (int**)malloc(sizeof(int*) * virtualMemorySize);
    for(int i = 0; i < virtualMemorySize; i++) virtualMemory1[i] = (int*)malloc(sizeof(int) * this->frameSize);
    virtualMemory2 = (int**)malloc(sizeof(int*) * virtualMemorySize);
    for(int i = 0; i < virtualMemorySize; i++) virtualMemory2[i] = (int*)malloc(sizeof(int) * this->frameSize);
    
    initiliseTables();
    disk = Disk(diskName, 2 * virtualMemorySize); // its stored amount 2 virtual memory size
    mmu = MMU(this->virtualMemorySize, this->numPhysical); // page table entry size
    
    // cout varaibles     
    cout << "frameSize 2: " << this->frameSize << endl;
    cout << "numPhysical 2: " << this->numPhysical << endl;
    cout << "virtualMemorySize 2: " << virtualMemorySize << endl;
    cout << "physicalMemory: " << sizeof(int)*this->numPhysical*this->frameSize << endl;
    cout << "virtualMemory1: " << sizeof(int)*static_cast<int>(pow(2,numVirtual))*this->frameSize << endl; 
}

Memory::~Memory(){
    for(int i = 0; i < numPhysical; i++) free(physicalMemory[i]);
    free(physicalMemory);
    for(int i = 0; i < virtualMemorySize; i++) free(virtualMemory1[i]);
    free(virtualMemory1);
    for(int i = 0; i < virtualMemorySize; i++) free(virtualMemory2[i]);
    free(virtualMemory2); 
}

void Memory::initiliseTables(){
    for (int i = 0; i < virtualMemorySize; i++) virtualMemory1[i][0] = virtualMemory2[i][0] = -1;
        
    for(int i = 0; i < numPhysical; i++) physicalMemory[i][0] = -1;        
}

void Memory::set(unsigned int threadNum, unsigned int index, int value){ 
    
    ++accessOfMemory;
    if(accessOfMemory > this->pageTablePrintInt){
        mmu.printPageTable1();
        mmu.printPageTable2();
        accessOfMemory = 0;
    }

    if(threadNum == 1) ++numberOfWritesForThread1;
    else if(threadNum == 2) ++numberOfWritesForThread2;

    // 0: phsical frame index, 1: oldVirtualTable, 2: oldVirtualEntryLine, 3: oldVirtualEntryLineModifierBit of pageReplacementInf
    int* pageReplacementInf = mmu.mapVirtualToPhysical(threadNum, index); // example of index value is 256*3 for 4. index
    // phsical frame index + oldVirtualLine -1 se degilse gelen degeri sil
    // convert phsical adress and set value
    
    // pageReplacementInf have 4 different integer value
    if(pageReplacementInf[1] == -1){ // there is no page fault, just update phsical frame and virtual entry as a valid
        // set value
        physicalMemory[pageReplacementInf[0]][0] = value; // pageReplacementInf[0] is phsical frame index
        if(threadNum == 1){
            virtualMemory1[index][0] = pageReplacementInf[0] ; // index is virtual entry line 
        }else{
            virtualMemory2[index][0] = pageReplacementInf[0]; // index is virtual entry line
        }
    }else{ // there are some pagereplacement operations 

        /*if(threadNum == 1) ++numberOfDiskPageReadForThread1;
        else if(threadNum == 2) ++numberOfDiskPageReadForThread2;*/
        

        // update disk if old data modified -> if(oldDataModified) update disk (virtualTable * virtualEntryLine)
        if(threadNum == 1) ++numberOfDiskPageWriteForThread1;
        else if(threadNum == 2) ++numberOfDiskPageWriteForThread2;
        disk.write((((pageReplacementInf[1]-1) * virtualMemorySize) + pageReplacementInf[2]), physicalMemory[pageReplacementInf[0]][0]); //pageReplacementInf[1] give table id
         
        // update phsical memory -> physicalMemory[phsicalFrameIndex][0] = newData
        physicalMemory[pageReplacementInf[0]][0] = value;
        // update old virtual memory -> if(oldVirtualTable == 1)table1[virtualEntryLine][0] = -1 else table2[virtualEntryLine][0] = -1
        if(pageReplacementInf[1] == 1) virtualMemory1[pageReplacementInf[2]][0] = -1;
        else if(pageReplacementInf[1] == 2) virtualMemory2[pageReplacementInf[2]][0] = -1;
        // update new virtual memory -> table1[virtualEntryLine][0] = phsical frame index
        if(threadNum == 1) virtualMemory1[index][0] = pageReplacementInf[0]; 
        else if(threadNum == 2) virtualMemory2[index][0] = pageReplacementInf[0];     
    } 
}

int Memory::get(unsigned int threadNum, unsigned int index){
    // pageReplacementInf -> 0: phsical frame index, 1: oldVirtualTable, 2: oldVirtualEntryLine, 3: oldVirtualEntryLineModifierBit
    ++accessOfMemory;
    if(accessOfMemory > this->pageTablePrintInt){
        mmu.printPageTable1();
        mmu.printPageTable2();
        accessOfMemory = 0;
    }
    
    if(threadNum == 1) ++numberOfReadsForThread1; 
    else if(threadNum == 2) ++numberOfReadsForThread2;

    int value = 0;
    int* pageReplacementInf = mmu.mapVirtualToPhysical(threadNum, index);
    if(pageReplacementInf[1] == -1){
        value = physicalMemory[pageReplacementInf[0]][0];
    }else{
        if(threadNum == 1) ++numberOfDiskPageReadForThread1;
        else if(threadNum == 2) ++numberOfDiskPageReadForThread2;

        int newData = disk.read((((pageReplacementInf[1]-1) * virtualMemorySize) + index));  // change this area
        
        if(threadNum == 1) ++numberOfDiskPageWriteForThread1;
        else if(threadNum == 2) ++numberOfDiskPageWriteForThread2;
        disk.write((((pageReplacementInf[1]-1) * virtualMemorySize) + pageReplacementInf[2]), physicalMemory[pageReplacementInf[0]][0]);
        
        physicalMemory[pageReplacementInf[0]][0] = newData;
        if(pageReplacementInf[1] == 1) virtualMemory1[pageReplacementInf[2]][0] = -1;
        else if(pageReplacementInf[1] == 2) virtualMemory2[pageReplacementInf[2]][0] = -1;
        if(threadNum == 1) virtualMemory1[index][0] = pageReplacementInf[0];
        else if(threadNum == 2) virtualMemory2[index][0] = pageReplacementInf[0];
        value = newData;
    }

    return value;
}

void Memory::printVirtualTable(int** virtualTable){
    for (int i = 0; i < virtualMemorySize; i++) {
        cout << "Virtual Entry " << i << ": " << virtualTable[i][0] << endl;
    } 
}
void Memory::printPhysicalTable(){
    for (int i = 0; i < numPhysical; i++) {
        cout << "Physical Frame " << i << ": " << physicalMemory[i][0] << endl;
    }
}

void Memory::printEndOfStatistics(){
    cout << "---------- Thread 1 Stats ----------"<< endl;
    cout << "Number of Reads: " << numberOfReadsForThread1 << endl;
    cout << "Number of Writes: " << numberOfWritesForThread1 << endl;
    cout << "Number of Page Misses " << mmu.pageMissCounterForThread1 << endl;
    cout << "Number of Page Replacements " << mmu.pageReplacementCounterForThread1 << endl;
    cout << "Number of Disk Page Reads: " << numberOfDiskPageReadForThread1 << endl;
    cout << "Number of Disk Page Writes: " << numberOfDiskPageWriteForThread1 << endl;
    cout <<"\n----------------------------------------\n";
    cout << "---------- Thread 2 Stats ----------"<< endl;
    cout << "Number of Reads: " << numberOfReadsForThread2 << endl;
    cout << "Number of Writes: " << numberOfWritesForThread2 << endl;
    cout << "Number of Page Misses " << mmu.pageMissCounterForThread2 << endl;
    cout << "Number of Page Replacements " << mmu.pageReplacementCounterForThread2 << endl;
    cout << "Number of Disk Page Reads: " << numberOfDiskPageReadForThread2 << endl;
    cout << "Number of Disk Page Writes: " << numberOfDiskPageWriteForThread2 << endl;
}   