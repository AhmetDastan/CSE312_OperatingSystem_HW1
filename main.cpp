#include <iostream>
#include <pthread.h>
#include <cmath> 
#include "memory.hpp"
#include "disk.hpp"
#include "mergeSort.cpp"
#include "binarySearch.cpp"

using namespace std;

 
void* threadFunction1(void* arg); 
void* threadFunction2(void* arg);
 
pthread_mutex_t mutexMemory;

// global argument variables
int frameSize = 8;
int numPhysical = 5;
int numVirtual = 7;
string pageReplacement;
int pageTablePrintInt = 10000;
string diskFileName = "disk.dat";
 
pthread_t process1, process2; 

Memory memory(frameSize, numPhysical, numVirtual, pageTablePrintInt ,diskFileName);

int main(int argc, char* argv[]){ 
      

    pthread_mutex_init(&mutexMemory, NULL);

    // Create the first thread 
    if (pthread_create(&process1, NULL, threadFunction1, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    } 

    // Create the second thread 
    if (pthread_create(&process2, NULL, threadFunction2, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // Wait for the threads to finish
    pthread_join(process1, NULL);

    pthread_join(process2, NULL);
    
    pthread_mutex_destroy(&mutexMemory);
 
    memory.printEndOfStatistics(); 
}

// Function to be executed by the first thread
void* threadFunction1(void* arg) {
    printf("Thread 1 is running\n");
    int randomNumber;
    int virtualSize = static_cast<int>(pow(2,numVirtual));
    int virtualFrameSize = static_cast<int>(pow(2,frameSize));

    int** virtualTable1 = (int**)malloc(sizeof(int*) * virtualSize);
    for(int i = 0; i < virtualSize; i++) virtualTable1[i] = (int*)malloc(sizeof(int) * virtualFrameSize);

    
    srand(1000);
    for(int i = 0; i < virtualSize; i++){ //virtualSize
        randomNumber = rand() % 100;  
        pthread_mutex_lock(&mutexMemory); 
        memory.set(1, i, randomNumber); // inserting to virtual and phyiscal memory 
        pthread_mutex_unlock(&mutexMemory);
    }

    //for sort operation get whole array and sort it
    cout << " virtual size " << virtualSize<<endl;
    for(int i = 0; i < virtualSize; ++i){
        pthread_mutex_lock(&mutexMemory);
        virtualTable1[i][0] = memory.get(1, i);
        pthread_mutex_unlock(&mutexMemory);
    }
    mergeSort(virtualTable1, 0, virtualSize - 1);

    //do search operation 5 time, 2 of them in not that array, for getting array.
    int searchValue;
    for(int i = 0; i < 5; i++){
        if(i == 0 || i == 1) int searchValue = -99;
        else {
            pthread_mutex_lock(&mutexMemory);
            searchValue = memory.get(1, i);
            pthread_mutex_unlock(&mutexMemory);
        }

        int index = binarySearch(virtualTable1, 0, virtualSize - 1, searchValue); 
    }
    
    for(int i = 0; i < virtualSize; i++) free(virtualTable1[i]);
    free(virtualTable1);

    return NULL;
}

// Function to be executed by the second thread
void* threadFunction2(void* arg) {
    printf("Thread 2 is running\n");
    int randomNumber;
    int virtualSize = static_cast<int>(pow(2,numVirtual));
    int virtualFrameSize = static_cast<int>(pow(2,frameSize));

    int** virtualTable2 = (int**)malloc(sizeof(int*) * virtualSize);
    for(int i = 0; i < virtualSize; i++) virtualTable2[i] = (int*)malloc(sizeof(int) * virtualFrameSize);

    srand(2000);
    for(int i = 0; i < virtualSize; i++){
        randomNumber = rand() % 100;  
        pthread_mutex_lock(&mutexMemory);
        memory.set(2, i, randomNumber);  // inserting to virtual and phyiscal  memory
        pthread_mutex_unlock(&mutexMemory);
    }

    //for sort operation get whole array and sort it
    for(int i = 0; i < virtualSize; ++i){
        pthread_mutex_lock(&mutexMemory);
        virtualTable2[i][0] = memory.get(2, i);
        pthread_mutex_unlock(&mutexMemory);
    }
    mergeSort(virtualTable2, 0, virtualSize - 1);

    //do search operation 5 time, 2 of them in not that array, for getting array.
    int searchValue;
    for(int i = 0; i < 5; i++){
        if(i == 0 || i == 1) int searchValue = -99;
        else {
            pthread_mutex_lock(&mutexMemory);
            searchValue = memory.get(2, i);
            pthread_mutex_unlock(&mutexMemory);
        }
        int index = binarySearch(virtualTable2, 0, virtualSize - 1, searchValue); 
    }
    
    for(int i = 0; i < virtualSize; i++) free(virtualTable2[i]);
    free(virtualTable2);
    return NULL;
}
 