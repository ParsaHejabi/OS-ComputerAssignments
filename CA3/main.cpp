//
//  main.cpp
//  CA3
//
//  Created by Parsa Hejabi on 12/31/18.
//  Copyright © 2018 Parsa Hejabi. All rights reserved.
//

#include <bits/stdc++.h>
using namespace std;

// PARTITION FILE FORMAT:
// [file_name|file_size|permissions(3)|author_name|last_modified(8)||data]

struct myFile {
    string name;
    string size;
    string permission;
    string authorName;
    string lastModified;
};

bool operator < (const myFile& f1, const myFile& f2){
    return f1.name.compare(f2.name) < 0;
}

// DEFINE CONSTANTS
#define BLOCK_SIZE 8
#define DELIMITER_CHAR '|'
#define EXISTING_FILE_START_CHAR '['
#define DELETED_FILE_START_CHAR '#'
#define END_ATTRIBUTE_CHAR char(186)
#define FILE_END_CHAR ']'
#define PARTITION_START_INDEX 32

// MAIN VARIABLES
set<myFile> files;

char* myPartition;

int myPartitionSize;
int numberOfBlocks;
int numberOfFreeCells;
int numberOfFreeBlocks;
int pointerSize;
int dataSizeInBlocks;

int N;

// IO VARIABLES
ofstream output1File("output1.txt");
ofstream output2File("output2.txt");
ofstream debugFile("debug.txt");

// FUNCTIONS
void debug(){
    for (int counter = 1; counter <= numberOfBlocks; counter ++) {
        debugFile << "Block number #" << counter << "    ";
        for (int j = (counter - 1) * BLOCK_SIZE; j < (counter - 1) * BLOCK_SIZE + 8; j++) {
            debugFile << myPartition[j] << " ";
        }
        debugFile << "\n";
    }
    
    debugFile << "----------------------\n";
    debugFile << "Number of free cells: " << numberOfFreeCells << "\n";
    debugFile << "Number of free blocks: " << numberOfFreeBlocks << "\n";
}

int* intToBase256(int number, int pointerSize){
    int* base256 = new int[pointerSize];
    
    int i = 0;
    while (number > 0) {
        base256[i] = number % 256;
        number /= 256;
        i++;
    }
    return base256;
}

string intToBase256Str(int number, int pointerSize){
    int* base256 = intToBase256(number, pointerSize);
    string base256Str;
    for (int i = pointerSize - 1; i >= 0; i--) {
        base256Str += char(base256[i]);
    }
    return base256Str;
}

int base256StrToInt(string base256, int pointerSize){
    int i = pointerSize - 1;
    int output = 0;
    for (auto it = base256.begin(); it != base256.end(); it++) {
        output += ((int) (*it)) * pow(256, i);
        i--;
    }
    return output;
}

int myLog(int base, int number){
    return (int)(log(number) / log(base));
}

void finish(){
    delete [] myPartition;
    output1File.close();
    output2File.close();
}

int calcNextBlockIndex(int startIndex){
    string nextPointer;
    for (int i = startIndex; i < startIndex + pointerSize; i++) {
        nextPointer += myPartition[i];
    }
    return base256StrToInt(nextPointer, pointerSize);
}

string findFileName(int blockStartIndex){
    string output;
    int index = blockStartIndex + 1;
    int blockEndIndex = blockStartIndex + 7;
    int blockLastDataIndex = blockEndIndex - pointerSize;
    while (myPartition[index] != DELIMITER_CHAR) {
        if (index == blockLastDataIndex + 1) {
            int nextBlockIndex = calcNextBlockIndex(index);
            blockStartIndex = nextBlockIndex * BLOCK_SIZE;
            index = nextBlockIndex * BLOCK_SIZE;
            blockEndIndex = blockStartIndex + 7;
            blockLastDataIndex = blockEndIndex - pointerSize;
        }
        else{
            output += myPartition[index];
            index++;
        }
    }
    return output;
}

bool isLastBlockOfFile(int blockStartIndex){
    int blockEndIndex = blockStartIndex + 7;
    for (int i = blockStartIndex; i <= blockEndIndex; i++) {
        if (myPartition[i] == FILE_END_CHAR) {
            return true;
        }
    }
    return false;
}

string fetchAllOfAttributes(int blockStartIndex){
    string output;
    int index = blockStartIndex + 1;
    int blockEndIndex = blockStartIndex + 7;
    int blockLastDataIndex;
    if (isLastBlockOfFile(blockStartIndex)) {
        blockLastDataIndex = blockEndIndex;
    }
    else{
        blockLastDataIndex = blockEndIndex - pointerSize;
    }
    while (myPartition[index] != END_ATTRIBUTE_CHAR) {
        if (index == blockLastDataIndex + 1) {
            int nextBlockIndex = calcNextBlockIndex(index);
            blockStartIndex = nextBlockIndex * BLOCK_SIZE;
            index = nextBlockIndex * BLOCK_SIZE;
            blockEndIndex = blockStartIndex + 7;
            if (isLastBlockOfFile(blockStartIndex)) {
                blockLastDataIndex = blockEndIndex;
            }
            else{
                blockLastDataIndex = blockEndIndex - pointerSize;
            }
        }
        else{
            output += myPartition[index];
            index++;
        }
    }
    return output;
}

myFile createMyFile(string attributes){
    myFile output;
    
    //    DEBUG
//    cout << attributes << endl;
    
    char delimiter = DELIMITER_CHAR;
    string tokens[5];
    int counter = 0;
    size_t pos = 0;
    while ((pos = attributes.find(delimiter)) != string::npos) {
        tokens[counter++] = attributes.substr(0, pos);
        attributes.erase(0, pos + 1);
    }
    tokens[4] = attributes;
    output.name = tokens[0];
    output.size = tokens[1];
    output.permission = tokens[2];
    output.authorName = tokens[3];
    output.lastModified = tokens[4];
    
    return output;
}

void output2(){
    for (int counter = PARTITION_START_INDEX; counter < myPartitionSize; counter += BLOCK_SIZE) {
        if (myPartition[counter] == EXISTING_FILE_START_CHAR) {
            string attributes = fetchAllOfAttributes(counter);
            files.insert(createMyFile(attributes));
        }
    }
    int i = 1;
    for (auto it = files.begin(); it != files.end(); it++) {
        output2File << "#line" << i << "\t" << (*it).name << "\t" << (*it).lastModified << "\t" << (*it).authorName << "\t" << (*it).permission << "\t" << (*it).size << "\n";
        i++;
    }
}

void output1(){
    int i = 1;
    for (int counter = PARTITION_START_INDEX; counter < myPartitionSize; counter += BLOCK_SIZE) {
        if (myPartition[counter] == EXISTING_FILE_START_CHAR) {
            string fileName = findFileName(counter);
            int blockNumber = counter / BLOCK_SIZE;
            output1File << "#line" << i << "\t" << "Block Number #" << blockNumber + 1 << "\t" << "File Name: " << fileName << "\n";
            i++;
        }
    }
}

int calcBlocksNeededAdd(int cellsNeeded){
    return (cellsNeeded / (BLOCK_SIZE - pointerSize)) + ((cellsNeeded % (BLOCK_SIZE - pointerSize) <= pointerSize) ? 0 : 1);
}

int calcCellsNeededAdd(string tokens[]){
    return tokens[1].size() + tokens[2].size() + 3 + tokens[4].size() + 8 + 4 + 1 + stoi(tokens[2]) + 2;
}

int findFreeBlock(){
    int output = 4;
    for (int counter = PARTITION_START_INDEX; counter < myPartitionSize; counter += BLOCK_SIZE) {
        if (myPartition[counter] == DELETED_FILE_START_CHAR || myPartition[counter] == '\0') {
            break;
        }
        else{
            output++;
        }
    }
    return output;
}

string makeFileAttributes(string tokens[]){
    string output = EXISTING_FILE_START_CHAR + tokens[1] + DELIMITER_CHAR + tokens[2] + DELIMITER_CHAR + tokens[3] + DELIMITER_CHAR + tokens[4] + DELIMITER_CHAR + tokens[5] + char(186);
    for (int i = 0; i < stoi(tokens[2]); i++) {
        output += "-";
    }
    output += "]";
    return output;
}

bool isWritingLastBlock(int blocksNeededToWrite,int blocksRemainedToWrite){
    if (blocksNeededToWrite == 1) {
        return true;
    }
    else{
        if (blocksRemainedToWrite == 1) {
            return true;
        }
        else{
            return false;
        }
    }
}

void addFile(string tokens[]){
    int cellsNeeded = calcCellsNeededAdd(tokens);
    int cellsRemainedToWrite = cellsNeeded;
    int blocksNeeded = calcBlocksNeededAdd(cellsNeeded);
    int blocksRemainedToWrite = blocksNeeded;
    cellsNeeded += (blocksNeeded - 1) * pointerSize;
    
    if (cellsNeeded <= numberOfFreeCells && blocksNeeded <= numberOfFreeBlocks) {
        //        we can add the file
        numberOfFreeCells -= cellsNeeded;
        numberOfFreeBlocks -= blocksNeeded;
        
        string attributes = makeFileAttributes(tokens);
        
//        DEBUG
//        cout << "THIS IS Attributes length: " << attributes.size() << endl;
        
        //        find first free block
        int firstFreeBlockIndex = findFreeBlock();
        
        int freeBlockStartIndex = firstFreeBlockIndex * BLOCK_SIZE;
        int freeBlockEndIndex = freeBlockStartIndex + 7;
        int freeBlockLastDataIndex;
        
        int attributesIndex = 0;
        
        while (cellsRemainedToWrite != 0) {
            if (isWritingLastBlock(blocksNeeded, blocksRemainedToWrite)) {
                freeBlockLastDataIndex = freeBlockEndIndex;
            }
            else{
                freeBlockLastDataIndex = freeBlockEndIndex - pointerSize;
            }
            for (int i = freeBlockStartIndex; i <= freeBlockLastDataIndex; i++) {
                myPartition[i] = attributes[attributesIndex];
                attributesIndex++;
                cellsRemainedToWrite--;
                if (cellsRemainedToWrite == 0) {
                    break;
                }
            }
            if (!isWritingLastBlock(blocksNeeded, blocksRemainedToWrite)) {
//                find next block
                firstFreeBlockIndex = findFreeBlock();
                string pointer = intToBase256Str(firstFreeBlockIndex, pointerSize);
//                DEBUG
//                cout << pointer << endl;
                int pointerIndex = 0;
                for (int i = freeBlockEndIndex - pointerSize + 1; i <= freeBlockEndIndex; i++) {
                    myPartition[i] = pointer.at(pointerIndex);
                    pointerIndex++;
                }
                freeBlockStartIndex = firstFreeBlockIndex * BLOCK_SIZE;
                freeBlockEndIndex = freeBlockStartIndex + 7;
            }
            blocksRemainedToWrite--;
        }
    }
    else{
        cout << "There is not enough space to add \"" << tokens[1] << "\" file with this attributes!" << endl;
        cout << "Bytes need: " << cellsNeeded << endl;
        cout << "Bytes currently have on disk: " << numberOfFreeCells << endl;
        return;
    }
}

int calcBlocksFreedDelete(int cellsFreed){
    return (cellsFreed / (BLOCK_SIZE - pointerSize)) + ((cellsFreed % (BLOCK_SIZE - pointerSize) <= pointerSize) ? 0 : 1);
}

int calcCellsFreedDelete(string attributes){
    int output = attributes.size();
    char delimiter = DELIMITER_CHAR;
    string tokens[5];
    int counter = 0;
    size_t pos = 0;
    while ((pos = attributes.find(delimiter)) != string::npos) {
        tokens[counter++] = attributes.substr(0, pos);
        attributes.erase(0, pos + 1);
    }
//    DEBUG
//    cout << stoi(tokens[1]) + 3 + attributes.() << endl;
    output += 3 + stoi(tokens[1]);
    return output;
}

void deleteAllOfBlocks(int blockStartIndex){
    myPartition[blockStartIndex] = DELETED_FILE_START_CHAR;
    int index = blockStartIndex + 1;
    int blockEndIndex = blockStartIndex + 7;
    int blockLastDataIndex;
    if (isLastBlockOfFile(blockStartIndex)) {
        blockLastDataIndex = blockEndIndex;
    }
    else{
        blockLastDataIndex = blockEndIndex - pointerSize;
    }
    while (myPartition[index] != FILE_END_CHAR) {
        if (index == blockLastDataIndex + 1) {
            int nextBlockIndex = calcNextBlockIndex(index);
            blockStartIndex = nextBlockIndex * BLOCK_SIZE;
            index = nextBlockIndex * BLOCK_SIZE;
            blockEndIndex = blockStartIndex + 7;
            if (isLastBlockOfFile(blockStartIndex)) {
                blockLastDataIndex = blockEndIndex;
            }
            else{
                blockLastDataIndex = blockEndIndex - pointerSize;
            }
        }
        else{
            myPartition[blockStartIndex] = DELETED_FILE_START_CHAR;
            index++;
        }
    }
}

void deleteFile(string tokens[]){
    bool isFileExist = false;
    for (int counter = PARTITION_START_INDEX; counter < myPartitionSize; counter += BLOCK_SIZE) {
        if (myPartition[counter] == EXISTING_FILE_START_CHAR) {
            string thisFileName = findFileName(counter);
//            DEBUG
//            cout << thisFileName << endl;
            if (tokens[1].compare(thisFileName) == 0) {
                isFileExist = true;
                string attributes = fetchAllOfAttributes(counter);
                deleteAllOfBlocks(counter);
//                DEBUG
//                cout << attributes << "\t" << attributes.size() << endl;
                int cellsFreed = calcCellsFreedDelete(attributes);
//                DEBUG
//                cout << cellsFreed << endl;
                int blocksFreed = calcBlocksFreedDelete(cellsFreed);
//                DEBUG
//                cout << "blocksFreed: " << blocksFreed << endl;
                numberOfFreeBlocks += blocksFreed;
                cellsFreed += (blocksFreed - 1) * pointerSize;
//                DEBUG
//                cout << "cellsFreed: " << cellsFreed << endl;
                numberOfFreeCells += cellsFreed;
                myPartition[counter] = DELETED_FILE_START_CHAR;
            }
        }
    }
    if (!isFileExist) {
        cout << "There is no file with this name!\nThis instruction has been skipped!" << endl;
    }
}

bool splitInstruction(string instruction){
    string delimiter = " ";
    string tokens[6];
    int counter = 0;
    size_t pos = 0;
    while ((pos = instruction.find(delimiter)) != string::npos) {
        tokens[counter++] = instruction.substr(0, pos);
        instruction.erase(0, pos + delimiter.size());
    }
    if (tokens[0].compare("add") == 0 || tokens[0].compare("Add") == 0) {
        tokens[5] = instruction;
        counter++;
        if (counter == 6) {
            addFile(tokens);
            return true;
        }
        else{
            return false;
        }
    }
    else if (tokens[0].compare("Delete") == 0 || tokens[0].compare("delete") == 0){
        tokens[1] = instruction;
        counter++;
        if (counter == 2) {
            deleteFile(tokens);
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}

void getInstructions(){
    string inputInstruction;
    for (int instCounter = 1; instCounter <= N; instCounter++) {
        getline(cin ,inputInstruction);
        bool validInst = splitInstruction(inputInstruction);
        if (!validInst) {
            cout << "Instruction number #" << instCounter << " is not valid and it has been skipped!" << endl;
        }
//        debug();
    }
}

void init(){
    cout << "Please enter the size of your partition in Bytes:" << endl;
    cin >> myPartitionSize;
    myPartition = new char[myPartitionSize];
    for (int i = PARTITION_START_INDEX; i < myPartitionSize; i++) {
        myPartition[i] = '\0';
    }
    numberOfBlocks = myPartitionSize / BLOCK_SIZE + (myPartitionSize % BLOCK_SIZE == 0 ? 0 : 1);
    numberOfFreeCells = myPartitionSize - PARTITION_START_INDEX;
    numberOfFreeBlocks = numberOfBlocks - 4;
    pointerSize = myLog(256, myPartitionSize) + 1;
    
    if (pointerSize >= 8) {
        cout << "ERROR: Yout partition size is so big that you need 8 or more Bytes for pointers!" << endl;
        terminate();
    }
    
    dataSizeInBlocks = BLOCK_SIZE - pointerSize;
    
    cout << "Please enter the number of instructions:" << endl;
    cin >> N;
    cin.ignore();
}

int main(int argc, const char * argv[]) {
    init();
    getInstructions();
    output1();
//    debug();
    output2();
    debug();
    finish();
    return 0;
}
