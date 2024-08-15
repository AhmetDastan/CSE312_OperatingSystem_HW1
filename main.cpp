#include <iostream>
#include <pthread.h>
#include "memory.h"

using namespace std;



void freeMemory(); 
void* threadFunction1(void* arg); 
void* threadFunction2(void* arg);
 

// global argument variables
int frameSiz = 8;
int numPhysical = 5;
int numVirtual = 7;
string pageReplacement;
int pageTablePrintInt;
string diskFileName;
 
pthread_t process1, process2;



int main(int argc, char* argv[]){
    cout << "ananas1" <<endl;     
    /*if(argc != 7){
        cout << "Invalid number of arguments" << endl;
        return 1;
    }*/
    // get sortArrays frameSize numPhysical numVirtual pageReplacement pageTablePrintInt diskFileName.dat

     

    Memory memory(frameSiz, numPhysical, numVirtual);  // argumans -> int frameSize, int numPhysical, int numVirtual
    
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



    freeMemory();
  
    cout << "ananas2"<<endl; 
}

// Function to be executed by the first thread
void* threadFunction1(void* arg) {
    printf("Thread 1 is running\n");
    /*srand(1000);
    for(int i = 0; i < virtualTable1.size; i++){
        virtualTable1.table[i] = rand() % 100;
        cout << virtualTable1.table[i] << " ";
    }*/
    return NULL;
}

// Function to be executed by the second thread
void* threadFunction2(void* arg) {
    printf("Thread 2 is running\n");
    /*srand(2000);
    for(int i = 0; i < virtualTable2.size; i++){
        virtualTable2.table[i] = rand() % 100;
        cout << virtualTable2.table[i] << " ";
    }*/
    return NULL;
}

void freeMemory(){
    /*free(virtualTable1.table);
    free(virtualTable2.table);*/
}