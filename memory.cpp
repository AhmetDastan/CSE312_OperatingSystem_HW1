#include <iostream>
#include "memory.h"
//#include <math.h>
 #include <cmath>

using namespace std;


Memory::Memory(int frameSize, int numPhysical, int numVirtual){
    this->frameSize = static_cast<int>(pow(2,frameSize));
    this->numPhysical = static_cast<int>(pow(2,numPhysical));
    virtualMemorySize = static_cast<int>(pow(2,numVirtual));
    physicalMemory = (int*)malloc(sizeof(int) * this->numPhysical * this->frameSize);
    virtualMemory1 = (int*)malloc(sizeof(int) * virtualMemorySize * this->frameSize);
    virtualMemory2 = (int*)malloc(sizeof(int) * virtualMemorySize * this->frameSize);
 
    mmu = MMU(virtualMemorySize);
    // cout varaibles 
    
    cout << "frameSize 2: " << this->frameSize << endl;
    cout << "numPhysical 2: " << this->numPhysical << endl;
    cout << "virtualMemorySize 2: " << virtualMemorySize << endl;
    cout << "physicalMemory: " << sizeof(int)*this->numPhysical*this->frameSize << endl;
    cout << "virtualMemory1: " << sizeof(int)*static_cast<int>(pow(2,numVirtual))*this->frameSize << endl; 
}

Memory::~Memory(){
    free(physicalMemory);
    free(virtualMemory1);
    free(virtualMemory2); 
}


void Memory::set(unsigned int threadNum, unsigned int index, int value){
    // convert phsical adress and set value
    if(mmu.mapVirtualToPhysical(threadNum, index, value) == -1){ // -1 means return page fault

    }else {
        // set value
        
    }
}

int Memory::get(unsigned int threadNum, unsigned int index){
    int value = 0;
    return value;
}