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
 
int* MMU::mapVirtualToPhysical(int threadNum, int virtualEntryLine){ // sanal lineda ozel yer var mi yoksa page foult dondur swap yap 
    
    pageFault = false;
    int indexOfPhysicalAddress = -1;
    int* pageReplacementInf;  // 0: phsical frame index, 1: oldVirtualTable, 2: oldVirtualEntryLine, 3: oldVirtualEntryLineModifierBit
    if(threadNum == 1){
        
        if(pageTable1.presentBit[virtualEntryLine]){    // if have a poin to physical memory send adress atherwise do page replacement
            indexOfPhysicalAddress = pageTable1.pageFrame[virtualEntryLine]; 
        }else{  // find proper place in phsical table for virtual entry, otherwise do swap
            ++pageMissCounterForThread1;
            for(int i = 0; i < physicalEntryAmount/2; i++){ 
                if(physicalReferenceTable[i] == -1){ // founded empty space
                    physicalReferenceTable[i] = virtualEntryLine;
                    pageTable1.presentBit[virtualEntryLine] = true;
                    pageTable1.pageFrame[virtualEntryLine] = i;
                    pageTable1.referencedBit[virtualEntryLine] = true;
                    pageTable1.modifiedBit[virtualEntryLine] = true;
                    indexOfPhysicalAddress = i; 
                    break;
                }
            }
            cout << "indexOfPhysicalAddress " << indexOfPhysicalAddress<<endl;
            if(indexOfPhysicalAddress == -1){
                // increase page replacement counter
                ++pageReplacementCounterForThread1;
                pageFault = true;
            }
        }
        
    }else if(threadNum == 2){
        if(pageTable2.presentBit[virtualEntryLine] == true){    // if have a poin to physical memory send adress atherwise do page replacement
            indexOfPhysicalAddress = pageTable2.pageFrame[virtualEntryLine];
        }else{  // find proper place in phsical table for virtual entry, otherwise do swap
            ++pageMissCounterForThread2;
            for(int i = physicalEntryAmount/2; i < physicalEntryAmount; i++){
                if(physicalReferenceTable[i] == -1){
                    physicalReferenceTable[i] = virtualEntryLine;
                    pageTable2.presentBit[virtualEntryLine] = true;
                    pageTable2.referencedBit[virtualEntryLine] = true;
                    pageTable2.pageFrame[virtualEntryLine] = i;
                    pageTable2.modifiedBit[virtualEntryLine] = true;
                    indexOfPhysicalAddress = i; 
                    break;
                }
            } 
            if(indexOfPhysicalAddress == -1){
                // increase page replacement counter
                ++pageReplacementCounterForThread2;
                pageFault = true;
            }
                
        }
    }
    if(pageFault){ 
        pageFaultSignal();
        // 0: phsical frame index, 1: oldVirtualTable, 2: oldVirtualEntryLine, 3: oldVirtualEntryLineModifierBit of pageReplacementInf
        pageReplacementInf = clockPageAlrorithm(threadNum, virtualEntryLine);  
    }else{
        pageReplacementInf = new int[4];
        pageReplacementInf[0] = indexOfPhysicalAddress;
        pageReplacementInf[1] = -1;
        pageReplacementInf[2] = -1;
        pageReplacementInf[3] = -1;
    } 
    return pageReplacementInf;
}

void MMU::pageFaultSignal(){
    //cout << "Page Fault Signal" << endl;
}

int* MMU::clockPageAlrorithm(int threadNum, int virtualEntryLine){
    
    // find R == 0 bit and swap. Otherwise if you find R == 1, set to R = 0 and continue
    // check tables just true of present bit
    int pageFrame = -1;  // point to phsical frame index
    int oldVirtualTable = 0;
    int oldVirtualEntryLine = -1;
    int oldVirtualEntryLineModifierBit = 0;
    int* pageReplacementInf = new int[4];
 

    if(threadNum == 1){     // first look table1
        while(true){
            clockAlgorithmCurrentPoint = 0; 
            while(clockAlgorithmCurrentPoint <= pageTable1.pageTableEntryAmount && pageFrame == -1){
                if(pageTable1.presentBit[clockAlgorithmCurrentPoint] == true){
                    if(pageTable1.referencedBit[clockAlgorithmCurrentPoint] == false){
                        // old position will be remove
                        pageFrame = pageTable1.pageFrame[clockAlgorithmCurrentPoint]; // take physical address from virtual entry
                        pageTable1.presentBit[clockAlgorithmCurrentPoint] = false;
                        pageTable1.referencedBit[clockAlgorithmCurrentPoint] = false;
                        pageTable1.pageFrame[clockAlgorithmCurrentPoint] = -1;
                        physicalReferenceTable[pageFrame] = -1;
                        if(pageTable1.modifiedBit[clockAlgorithmCurrentPoint] == true){
                            // update to disk
                            oldVirtualEntryLineModifierBit = 1;
                            pageTable1.modifiedBit[clockAlgorithmCurrentPoint] == false;
                        }
                        
                        oldVirtualEntryLine = clockAlgorithmCurrentPoint;

                        // new position will be added
                        pageTable1.modifiedBit[virtualEntryLine] = false;
                        pageTable1.referencedBit[virtualEntryLine] = true;
                        pageTable1.presentBit[virtualEntryLine] = true;
                        pageTable1.pageFrame[virtualEntryLine] = pageFrame;

                        physicalReferenceTable[pageFrame] = virtualEntryLine;
                        oldVirtualTable = 1;
                        break;
                    }else{
                        pageTable1.referencedBit[clockAlgorithmCurrentPoint] = false;
                    }
                } 
                ++clockAlgorithmCurrentPoint;
            } 
            if(pageFrame != -1 && pageTable1.presentBit[virtualEntryLine] == true) break;

            clockAlgorithmCurrentPoint = 0;
            while(clockAlgorithmCurrentPoint <= pageTable2.pageTableEntryAmount && pageFrame == -1){
                if(pageTable2.presentBit[clockAlgorithmCurrentPoint] == true){
                    if(pageTable2.referencedBit[clockAlgorithmCurrentPoint] == false){
                        pageFrame = pageTable2.pageFrame[clockAlgorithmCurrentPoint];
                        pageTable2.presentBit[clockAlgorithmCurrentPoint] = false;
                        pageTable2.referencedBit[clockAlgorithmCurrentPoint] = false;
                        pageTable2.pageFrame[clockAlgorithmCurrentPoint] = -1;
                        physicalReferenceTable[pageFrame] = -1;
                        if(pageTable2.modifiedBit[clockAlgorithmCurrentPoint] == true){
                            oldVirtualEntryLineModifierBit = 1;
                            pageTable2.modifiedBit[clockAlgorithmCurrentPoint] == false;
                        }

                        oldVirtualEntryLine = clockAlgorithmCurrentPoint;

                        pageTable2.modifiedBit[virtualEntryLine] = false;
                        pageTable2.referencedBit[virtualEntryLine] = true;
                        pageTable2.presentBit[virtualEntryLine] = true;
                        pageTable2.pageFrame[virtualEntryLine] = pageFrame;

                        physicalReferenceTable[pageFrame] = virtualEntryLine;
                        oldVirtualTable = 2;
                        break;
                    }else{
                        pageTable2.referencedBit[clockAlgorithmCurrentPoint] = false;
                    }
                }
                ++clockAlgorithmCurrentPoint;
            } 
            if(pageFrame != -1 && pageTable2.presentBit[virtualEntryLine] == true) break;
        } 
    }else if(threadNum == 2){  // first look table2
        while(true){
            clockAlgorithmCurrentPoint = 0; 
            while(clockAlgorithmCurrentPoint <= pageTable2.pageTableEntryAmount && pageFrame == -1){
                if(pageTable2.presentBit[clockAlgorithmCurrentPoint] == true){
                    if(pageTable2.referencedBit[clockAlgorithmCurrentPoint] == false){
                        pageFrame = pageTable2.pageFrame[clockAlgorithmCurrentPoint];
                        pageTable2.presentBit[clockAlgorithmCurrentPoint] = false;
                        pageTable2.referencedBit[clockAlgorithmCurrentPoint] = false;
                        pageTable2.pageFrame[clockAlgorithmCurrentPoint] = -1;
                        physicalReferenceTable[pageFrame] = -1;
                        if(pageTable2.modifiedBit[clockAlgorithmCurrentPoint] == true){
                            oldVirtualEntryLineModifierBit = 1;
                            pageTable2.modifiedBit[clockAlgorithmCurrentPoint] == false;
                        }

                        oldVirtualEntryLine = clockAlgorithmCurrentPoint;

                        pageTable2.modifiedBit[virtualEntryLine] = false;
                        pageTable2.referencedBit[virtualEntryLine] = true;
                        pageTable2.presentBit[virtualEntryLine] = true;
                        pageTable2.pageFrame[virtualEntryLine] = pageFrame;

                        physicalReferenceTable[pageFrame] = virtualEntryLine;
                        oldVirtualTable = 2;
                        break;
                    }else{
                        pageTable2.referencedBit[clockAlgorithmCurrentPoint] = false;
                    }
                } 
                ++clockAlgorithmCurrentPoint;
            } 
            if(pageFrame != -1 && pageTable2.presentBit[virtualEntryLine] == true) break;

            clockAlgorithmCurrentPoint = 0;
            while(clockAlgorithmCurrentPoint <= pageTable1.pageTableEntryAmount && pageFrame == -1){
                if(pageTable1.presentBit[clockAlgorithmCurrentPoint] == true){
                    if(pageTable1.referencedBit[clockAlgorithmCurrentPoint] == false){
                        pageFrame = pageTable1.pageFrame[clockAlgorithmCurrentPoint];
                        pageTable1.presentBit[clockAlgorithmCurrentPoint] = false;
                        pageTable1.referencedBit[clockAlgorithmCurrentPoint] = false;
                        pageTable1.pageFrame[clockAlgorithmCurrentPoint] = -1;
                        physicalReferenceTable[pageFrame] = -1;
                        if(pageTable1.modifiedBit[clockAlgorithmCurrentPoint] == true){
                            oldVirtualEntryLineModifierBit = 1;
                            pageTable1.modifiedBit[clockAlgorithmCurrentPoint] == false;
                        }

                        oldVirtualEntryLine = clockAlgorithmCurrentPoint;

                        pageTable1.modifiedBit[virtualEntryLine] = false;
                        pageTable1.referencedBit[virtualEntryLine] = true;
                        pageTable1.presentBit[virtualEntryLine] = true;
                        pageTable1.pageFrame[virtualEntryLine] = pageFrame;

                        physicalReferenceTable[pageFrame] = virtualEntryLine;
                        oldVirtualTable = 1;
                        break;
                    }else{
                        pageTable1.referencedBit[clockAlgorithmCurrentPoint] = false;
                    }
                }
                ++clockAlgorithmCurrentPoint;
            }
        } 
    }  
    
    pageReplacementInf[0] = pageFrame;
    pageReplacementInf[1] = oldVirtualTable;
    pageReplacementInf[2] = oldVirtualEntryLine;
    pageReplacementInf[3] = oldVirtualEntryLineModifierBit; 
    return pageReplacementInf; 
}
 

void MMU::printPageTable1(){
    cout << "|--------------------------------------------------------------------------------|" << endl;
    cout << "|-------------------------  Page Table 1 For Thread 1" << " ---------------------------|" << endl;
    cout << "|--------------------------------------------------------------------------------|" << endl; 
    cout << "| Virtual Entry Line | Present Bit | Modified Bit | Referenced Bit | Page Frame  |" << endl;
    for(int i = 0; i < pageTable1.pageTableEntryAmount; ++i){
        cout<<"|";
        if(i < 10) cout << "  ";
        else if(i < 100) cout << " "; 
        cout << "         " << i << "        |      "; // virtual line
        cout << pageTable1.presentBit[i] << "      |       "; // present bit
        cout << pageTable1.modifiedBit[i] << "      |         "; // modified bit
        cout << pageTable1.referencedBit[i] << "      |       "; // referenced bit
        cout << pageTable1.pageFrame[i] << "   "; // page frame
        if(i < 10) cout << "  ";
        else if(i >= 10) cout << " ";
        cout<<endl;
    }
    cout << "|--------------------------------------------------------------------------------|" << endl;
 
}

void MMU::printPageTable2(){
    cout << "|--------------------------------------------------------------------------------|" << endl;
    cout << "|-------------------------  Page Table 2 For Thread 2" << " ---------------------------|" << endl;
    cout << "|--------------------------------------------------------------------------------|" << endl; 
    cout << "| Virtual Entry Line | Present Bit | Modified Bit | Referenced Bit | Page Frame  |" << endl;
    for(int i = 0; i < pageTable2.pageTableEntryAmount; ++i){
        cout<<"|";
        if(i < 10) cout << "  ";
        else if(i < 100) cout << " "; 
        cout << "         " << i << "        |      "; // virtual line
        cout << pageTable2.presentBit[i] << "      |       "; // present bit
        cout << pageTable2.modifiedBit[i] << "      |         "; // modified bit
        cout << pageTable2.referencedBit[i] << "      |       "; // referenced bit
        cout << pageTable2.pageFrame[i] << "   |"; // page frame
        if(i < 10) cout << "  ";
        else if(i >= 10) cout << " ";
        cout<<endl;
    }
    cout << "|--------------------------------------------------------------------------------|" << endl;
}

void MMU::printPhysicalReferenceTable(){
    cout << "Physical Reference Table: ";
    for(int i = 0; i < physicalEntryAmount; i++){
        cout << physicalReferenceTable[i] << " ";
    }
    cout << endl;
}