/**************************************************************
 * Class:  CSC-415-01
 * Name: Pramod Khatri, Hareesh Pedireddi, Vito Gano, Emin Musayev
 * Student ID: 920831584, 920425984, 916229878, 916823510
 * Project: Virtual File System
 *
 * File: fsLowDriver.c
 *
 * Description: This program is used to create a volume for the
 *              Test File System. It holds the free-space bit vector
 *		and the inodes on disk.
 *
 **************************************************************/

#include "volumeControlBlock.h"

#define BITSPERBYTE 8
vcb * myVCB;

char * fileNameInfo;
uint64_t volumeSize;
uint64_t blockSize;

// initializes our Volume Control Block
void initializeVCB(){
    //calculating the number of blocks the VCB needs
    int calcNumBlock = roundUpDiv(sizeof(vcb),blockSize);
    myVCB = calloc(calcNumBlock, calcNumBlock*blockSize);
    strcpy(myVCB -> header, "This is the VCB");
    myVCB -> volumeSize = volumeSize;
    myVCB -> numVolBlocks = blockSize;
    myVCB -> blocksNeeded = (uint32_t)calculateBlocks();
    strcpy(myVCB -> volumeName,fileNameInfo);
    myVCB -> fSBlockLocation = 0;
    myVCB -> rootLocation = 0;
    myVCB -> blocksInVolume = roundUpDiv(myVCB -> volumeSize, myVCB -> numVolBlocks);
    myVCB -> blocksInVolumeBytes = (uint32_t)roundUpDiv(myVCB -> blocksInVolume,BITSPERBYTE);
    myVCB -> blockSizeofVCB = calcNumBlock;
    myVCB -> freeSpaceAvailable = myVCB -> blocksInVolumeBytes;
    printVCBInfo();
    writeVCBToBlock(calcNumBlock);
}

// initializes the filename, volume size and block size in our VCB
void init(char * filename, ull_t volSize, ull_t blkSize){
    fileNameInfo = filename;
    volumeSize = volSize;
    blockSize = blkSize;
    initializeVCB();
}

// displays our Volume Control Block Information
void printVCBInfo(){
    printf("Here is the information regarding the Volume Control Block: \n");
    printf("VCB Volume Name: %s \n", myVCB -> volumeName);
    printf("VCB Volume Size: %lu \n", myVCB -> volumeSize);
    printf("VCB Volume Blocks: %lu \n", myVCB -> numVolBlocks);
    printf("VCB Volume Blocks In Volume: %lu \n", myVCB -> blocksInVolume);
    printf("VCB Volume Blocks In Volume (Bytes): %d \n", myVCB -> blocksInVolumeBytes);
    printf("VCB Volume Blocks Needed: %d \n", myVCB -> blocksNeeded);
    printf("VCB Free Space Block Number Index: %lu \n", myVCB -> fSBlockLocation);
    printf("VCB Root Location Index: %lu \n", myVCB -> rootLocation);
    printf("Size of myVCB structure: %ld \n\n", sizeof(vcb));
}

//rounds up our volume control block to 64 bits int value
uint64_t roundUpDiv(uint64_t v1, uint64_t v2){
    uint64_t v3 = v1/v2;
    if (v1%v2!=0){
        v3 += 1;
    }
    return v3;
}

//gets volume block size and counts to number of blocks
uint32_t calculateBlocks(){
    //bit to bytes
    int blockBits =  myVCB -> volumeSize/myVCB -> numVolBlocks; //gets block bits
    int blockBytes = blockBits/BITSPERBYTE; //converts block bits to bytes
    uint32_t blocksNeeded = blockBytes/myVCB -> numVolBlocks; //gives number of blocks needed for our free space
    
    if(blockBytes % myVCB -> numVolBlocks != 0){ //checks if there is a remainder after calculating the number of blocks.
        blocksNeeded += 1;
    }

    return blocksNeeded;
}

//gets VCB and writes the buffer
void writeVCBToBlock(int numOfBlocks){
    LBAwrite(myVCB,numOfBlocks,0);
    printf("VCB successfully written to the Block!\n\n");
    free(myVCB); //VCB is safely set to free
}
