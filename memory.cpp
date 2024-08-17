#include <iostream>
#include "memory.hpp" 
#include <cmath> 

using namespace std;

Memory::Memory(int frameSize, int numPhysical, int numVirtual,string diskName){
    this->diskFileName = diskName;
    this->frameSize = static_cast<int>(pow(2,frameSize));
    this->numPhysical = static_cast<int>(pow(2,numPhysical));
    
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
    cout << "set function \n";
    // 0: phsical frame index, 1: oldVirtualTable, 2: oldVirtualEntryLine, 3: oldVirtualEntryLineModifierBit of pageReplacementInf
    int* pageReplacementInf = mmu.mapVirtualToPhysical(threadNum, index); // example of index value is 256*3 for 4. index
    // phsical frame index + oldVirtualLine -1 se degilse gelen degeri sil
    // convert phsical adress and set value
    cout << "index and value ve threadNum " << index << " " << value<< " "<< threadNum << endl;
    cout << " pageReplaceInf " << pageReplacementInf[0] << " " << pageReplacementInf[1] << " " << pageReplacementInf[2] << " " << pageReplacementInf[3] << endl;
    // pageReplacementInf have 4 different integer value
    if(pageReplacementInf[1] == -1){ // there is no page fault, just update phsical frame and virtual entry as a valid
        // set value
        physicalMemory[pageReplacementInf[0]][0] = value; // pageReplacementInf[0] is phsical frame index
        if(threadNum == 1){
            virtualMemory1[index][0] = pageReplacementInf[0] ; // index is virtual entry line
            cout << " memory virtual -> "<< *(virtualMemory1 + (index * frameSize))<< endl;
        }else{
            virtualMemory2[index][0] = pageReplacementInf[0]; // index is virtual entry line
        }

        //printVirtualTable(virtualMemory1);
        //printPhysicalTable();
    }else{ // there are some pagereplacement operations
        // take a new data from disk -> newData = gets data from disk with (threadNum * index) 
        int newData = disk.read(threadNum * index); 
        cout << "newData -> " << newData << endl;
        // update disk if old data modified -> if(oldDataModified) update disk (virtualTable * virtualEntryLine)
        if(pageReplacementInf[3] == 1){
            disk.write(pageReplacementInf[1] * pageReplacementInf[2], physicalMemory[pageReplacementInf[0]][0]); //pageReplacementInf[1] give table id
        }
        // update phsical memory -> physicalMemory[phsicalFrameIndex][0] = newData
        physicalMemory[pageReplacementInf[0]][0] = newData;
        // update old virtual memory -> if(oldVirtualTable == 1)table1[virtualEntryLine][0] = -1 else table2[virtualEntryLine][0] = -1
        if(pageReplacementInf[1] == 1) virtualMemory1[pageReplacementInf[2]][0] = -1;
        else if(pageReplacementInf[1] == 2) virtualMemory2[pageReplacementInf[2]][0] = -1;
        // update new virtual memory -> table1[virtualEntryLine][0] = phsical frame index
        if(threadNum == 1) virtualMemory1[index][0] = pageReplacementInf[0]; 
        else if(threadNum == 2) virtualMemory2[index][0] = pageReplacementInf[0];     
    }
}

int Memory::get(unsigned int threadNum, unsigned int index){
    int value = 0;
    int* pageReplacementInf = mmu.mapVirtualToPhysical(threadNum, index);
    if(pageReplacementInf[1] == -1){
        value = physicalMemory[pageReplacementInf[0]][0];
    }else{
        int newData = disk.read(threadNum * index);
        if(pageReplacementInf[3] == 1){
            disk.write(pageReplacementInf[1] * pageReplacementInf[2], physicalMemory[pageReplacementInf[0]][0]);
        }
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