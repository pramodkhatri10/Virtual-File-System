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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fsLow.h"
#include "mfs.h"


void partitionSystem(char * filename, uint64_t volumeSize, uint64_t blockSize); // starts the volume partition system
