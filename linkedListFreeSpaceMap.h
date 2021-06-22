/**************************************************************
 * Class:  CSC-415-01
 * Name: Pramod Khatri, Hareesh Pedireddi, Vito Gano, Emin Musayev
 * Student ID: 920831584, 920425984, 916229878, 916823510
 * Project: Virtual File System
 *
 * File: linkedListFreeSpaceMap.h
 *
 * Description: This sets up our Free Space Management using Linked Lists.
 * 
 *              
 **************************************************************/

#include "volumeControlBlock.h"
#include "volumeFormatter.h"
#include <string.h>

typedef struct{ 
    int startingFreeBlock;
    struct Node * next; //next pointer of the node
    int blocksAvailable;
}Node;


void initializeLinkedList();

void initializeRoot(uint64_t blockNumber);

void openVolume(char * name);

uint64_t getNumVolBlocks();

uint64_t getRootLocation();

int checkFreeSpace(int num);

void changeFreeSpaceBlockAvailable(int numBlocks, int startLocation);

