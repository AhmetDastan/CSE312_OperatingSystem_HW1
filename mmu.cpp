#include "mmu.h"

using namespace std;

MMU::MMU(){
    
}

MMU::MMU(int pageTableEntryAmount){
    initilisePageTable(pageTableEntryAmount);
}

MMU::~MMU(){ 

}

void MMU::initilisePageTable(int pageTableEntryAmount){ 
    this->pageTable.pageTableEntryAmount = pageTableEntryAmount;
    pageTable.presentBit = (bool*)malloc(sizeof(bool)*pageTable.pageTableEntryAmount);
    pageTable.modifiedBit = (bool*)malloc(sizeof(bool)*pageTable.pageTableEntryAmount);
    pageTable.referencedBit = (bool*)malloc(sizeof(bool)*pageTable.pageTableEntryAmount);
    pageTable.pageTable = (int*)malloc(sizeof(int)*pageTable.pageTableEntryAmount);

    for(int i = 0; i < pageTable.pageTableEntryAmount; i++){
        pageTable.presentBit[i] = false;
        pageTable.modifiedBit[i] = false;
        pageTable.referencedBit[i] = false;
        pageTable.pageTable[i] = -1; 
    } 
}
 
int MMU::mapVirtualToPhysical(int threadNum, int virtualEntryLine, int value){ // sanal lineda ozel yer var mi yoksa page foult dondur swap yap
    int indexOfPhysicalAddress = -1;
    return indexOfPhysicalAddress;
}

void MMU::pageFaultSignal(){

}