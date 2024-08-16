#include "mmu.hpp"

using namespace std;

MMU::MMU(int virtualEntryAmount, int physicalEntryAmount){
    initilisePageTables(virtualEntryAmount);
    initilisePhysicalReferenceTable(physicalEntryAmount);
    clockAlgorithmCurrentPoint = 0;
}

MMU::~MMU(){ 

}

void MMU::initilisePageTables(int pageTableEntryAmount){ 
    this->pageTable1.pageTableEntryAmount = pageTableEntryAmount;
    pageTable1.presentBit = (bool*)malloc(sizeof(bool)*pageTable1.pageTableEntryAmount);
    pageTable1.modifiedBit = (bool*)malloc(sizeof(bool)*pageTable1.pageTableEntryAmount);
    pageTable1.referencedBit = (bool*)malloc(sizeof(bool)*pageTable1.pageTableEntryAmount);
    pageTable1.pageFrame = (int*)malloc(sizeof(int)*pageTable1.pageTableEntryAmount);

    for(int i = 0; i < pageTable1.pageTableEntryAmount; i++){
        pageTable1.presentBit[i] = false;
        pageTable1.modifiedBit[i] = false;
        pageTable1.referencedBit[i] = false;
        pageTable1.pageFrame[i] = -1; 
    } 

    this->pageTable2.pageTableEntryAmount = pageTableEntryAmount;
    pageTable2.presentBit = (bool*)malloc(sizeof(bool)*pageTable2.pageTableEntryAmount);
    pageTable2.modifiedBit = (bool*)malloc(sizeof(bool)*pageTable2.pageTableEntryAmount);
    pageTable2.referencedBit = (bool*)malloc(sizeof(bool)*pageTable2.pageTableEntryAmount);
    pageTable2.pageFrame = (int*)malloc(sizeof(int)*pageTable2.pageTableEntryAmount);

    for(int i = 0; i < pageTable2.pageTableEntryAmount; i++){
        pageTable2.presentBit[i] = false;
        pageTable2.modifiedBit[i] = false;
        pageTable2.referencedBit[i] = false;
        pageTable2.pageFrame[i] = -1; 
    } 
}

void MMU::initilisePhysicalReferenceTable(int physicalEntryAmount){
    this->physicalEntryAmount = physicalEntryAmount;
    physicalReferenceTable = (int*)malloc(sizeof(int)*physicalEntryAmount);
    for(int i = 0; i < physicalEntryAmount; i++){
        physicalReferenceTable[i] = -1;
    }
}
 
int MMU::mapVirtualToPhysical(int threadNum, int virtualEntryLine){ // sanal lineda ozel yer var mi yoksa page foult dondur swap yap
    int indexOfPhysicalAddress = -1;
    if(threadNum == 1){
        if(pageTable1.presentBit[virtualEntryLine] == true){    // if have a poin to physical memory send adress atherwise do page replacement
            indexOfPhysicalAddress = pageTable1.pageFrame[virtualEntryLine];
        }else{  // find proper place in phsical table for virtual entry, otherwise do swap
            for(int i = 0; i < physicalEntryAmount; i++){
                if(physicalReferenceTable[i] == -1){
                    physicalReferenceTable[i] = virtualEntryLine;
                    pageTable1.presentBit[virtualEntryLine] = true;
                    pageTable1.pageFrame[virtualEntryLine] = i;
                    pageTable2.referencedBit[virtualEntryLine] = true;
                    indexOfPhysicalAddress = i; 
                    break;
                }
            }
            if(indexOfPhysicalAddress == -1)
                pageFault = true;
        }
    }else if(threadNum == 2){
        if(pageTable2.presentBit[virtualEntryLine] == true){    // if have a poin to physical memory send adress atherwise do page replacement
            indexOfPhysicalAddress = pageTable2.pageFrame[virtualEntryLine];
        }else{  // find proper place in phsical table for virtual entry, otherwise do swap
            for(int i = physicalEntryAmount/2; i < physicalEntryAmount; i++){
                if(physicalReferenceTable[i] == -1){
                    physicalReferenceTable[i] = virtualEntryLine;
                    pageTable2.presentBit[virtualEntryLine] = true;
                    pageTable2.referencedBit[virtualEntryLine] = true;
                    pageTable2.pageFrame[virtualEntryLine] = i;
                    indexOfPhysicalAddress = i; 
                    break;
                }
            } 
            if(indexOfPhysicalAddress == -1)
                pageFault = true;
        }
    }
    if(pageFault){
        pageFaultSignal();
    }
    return indexOfPhysicalAddress;
}

void MMU::pageFaultSignal(){

}

int MMU::clockPageAlrorithm(int threadNum, int virtualEntryLine){
    // find R == 0 bit and swap. Otherwise if you find R == 1, set to R = 0 and continue
    // check tables just true of present bit
    int changeAddressPoint = -1;
    
    if(threadNum == 1){     // first look table1
        while(true){
            clockAlgorithmCurrentPoint = 0; 
            while(pageTable1.presentBit[clockAlgorithmCurrentPoint] == true && clockAlgorithmCurrentPoint != pageTable1.pageTableEntryAmount){
                if(pageTable1.referencedBit[clockAlgorithmCurrentPoint] == false){
                    changeAddressPoint = pageTable1.pageFrame[clockAlgorithmCurrentPoint];
                    pageTable1.presentBit[clockAlgorithmCurrentPoint] = false;
                    break;
                }else{
                    pageTable1.referencedBit[clockAlgorithmCurrentPoint] = false;
                }
                ++clockAlgorithmCurrentPoint;
            } 
            clockAlgorithmCurrentPoint = 0;
            while(clockAlgorithmCurrentPoint != pageTable2.pageTableEntryAmount){
                if(pageTable2.referencedBit[clockAlgorithmCurrentPoint] == false){
                    changeAddressPoint = pageTable2.pageFrame[clockAlgorithmCurrentPoint];
                    break;
                }else{
                    pageTable2.referencedBit[clockAlgorithmCurrentPoint] = false;
                }
                ++clockAlgorithmCurrentPoint;
            } 
        } 
    }else if(threadNum == 2){  // first look table2
        while(true){
            clockAlgorithmCurrentPoint = 0; 
            while(clockAlgorithmCurrentPoint != pageTable2.pageTableEntryAmount){
                if(pageTable2.referencedBit[clockAlgorithmCurrentPoint] == false){
                    changeAddressPoint = pageTable2.pageFrame[clockAlgorithmCurrentPoint];
                    break;
                }else{
                    pageTable2.referencedBit[clockAlgorithmCurrentPoint] = false;
                }
                ++clockAlgorithmCurrentPoint;
            }
            clockAlgorithmCurrentPoint = 0; 
            while(clockAlgorithmCurrentPoint != pageTable1.pageTableEntryAmount){
                if(pageTable1.referencedBit[clockAlgorithmCurrentPoint] == false){
                    changeAddressPoint = pageTable1.pageFrame[clockAlgorithmCurrentPoint];
                    break;
                }else{
                    pageTable1.referencedBit[clockAlgorithmCurrentPoint] = false;
                }
                ++clockAlgorithmCurrentPoint;
            }
        }
    } 
    return changeAddressPoint; 
}