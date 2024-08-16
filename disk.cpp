
#include "disk.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>

using namespace std; 

Disk::Disk(string fileName, int indexAmount){
    this->fileName = fileName; 
    this->indexAmount = static_cast<int>(pow(2,++indexAmount));
    initiliseFile();
}
 

void Disk::initiliseFile(){
    ofstream file(fileName, ios::out); 
    for(int i = 0; i < indexAmount; ++i){
        file << "00000" << endl;
    }
    file.close();
}

void Disk::write(int index, int value){
    ifstream file(fileName);  
    ofstream tempFile("temp.txt", ios::out); // temp file
    string line;
    if(file.is_open()){
        for(int i = 0; i < index; ++i){
            getline(file,line);
            tempFile << line << endl;
        }
        getline(file,line);
        tempFile << value << endl;
        while(getline(file,line)){
            tempFile << line << endl;
        }
    } 

    tempFile.close();
    file.close();

    remove(fileName.c_str());         // remove old file
    rename("temp.txt", fileName.c_str()); // rename temp file
}

int Disk::read(int index){      // index include offset
    ifstream file(fileName);

    if(file.is_open()){
        string line;
        for(int i = 0; i <= index; ++i){
            getline(file,line);
        }
        file.close();
        return stoi(line);
    }
    return -1;
}
 