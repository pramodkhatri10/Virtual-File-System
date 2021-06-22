/**************************************************************
 * Class:  CSC-415-01
 * Name: Pramod Khatri, Hareesh Pedireddi, Vito Gano, Emin Musayev
 * Student ID: 920831584, 920425984, 916229878, 916823510
 * Project: Virtual File System
 *
 * File: volumeControlBlock.h
 *
 * Description: This program is the header file .
 *
 **************************************************************/
#include "volumeFormatter.h"
#include <string.h>

#define MAXNUMOFCHARVOLNAME 25

//structure for our main Volume Control Block
typedef struct{
    char header[20];
    char volumeName[20]; //the volume name
    uint64_t volumeSize; // size of the Volume
    uint64_t numVolBlocks;  //number of bytes per block
    uint32_t blocksNeeded; //displays the blocks needed for the volume
    uint64_t fSBlockLocation; //the free space block location
    uint64_t rootLocation; //location of the root directory
    uint64_t blocksInVolume; //displays number of blocks inside the volume
    uint32_t blocksInVolumeBytes; //converts number of blocks inside the volume to bytes
    int blockSizeofVCB; //tells what the block size of the VCB uses
    int freeSpaceAvailable; //tells what the available amount of blocks left.
}vcb;

void initializeVCB();

void init(char * filename, ull_t volSize, ull_t blkSize);

uint64_t roundUpDiv(uint64_t v1, uint64_t v2);

uint32_t calculateBlocks();

void writeVCBToBlock(int numOfBlocks);

void printVCBInfo();



