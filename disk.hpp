#ifndef DISK_H
#define DISK_H

#include <iostream>
#include <string>

using namespace std;

class Disk{
public: 
    Disk(){};
    Disk(string fileName,int indexAmount); 
    
    void write(int index, int value);
    int read(int index); 
    
private:
    void initiliseFile();
    int indexAmount;
    string fileName;
};



#endif