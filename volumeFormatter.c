/**************************************************************
 * Class:  CSC-415-01
 * Name: Pramod Khatri, Hareesh Pedireddi, Vito Gano, Emin Musayev
 * Student ID: 920831584, 920425984, 916229878, 916823510
 * Project: Virtual File System
 *
 * File: volumeFormatter.c
 *
 * Description: This allows us to format our volume and run the
 * the main method. This will start the partition system and 
 * call the function that initializes the volume control block.
 *              
 **************************************************************/

#include "volumeFormatter.h"

int main(int argc, char *argv[])
{
	char *filename;
	uint64_t volumeSize;
	uint64_t blockSize;
	;

	//validates arguments count for voluem control block
	if (argc > 3)
	{
		filename = argv[1];
		volumeSize = atoll(argv[2]);
		blockSize = atoll(argv[3]);
	}
	else
	{
		printf("Usage: fsLowDriver volumeFileName volumeSize blockSize\n");
		return -1;
	}

	partitionSystem(filename, volumeSize, blockSize);

	return 0;
}

/**
 * creates, opens and closes our volume control block
 * specifies file name to our fs disk
 * closes volume file and abrupts any read and write further
*/
void partitionSystem(char *filename, uint64_t volumeSize, uint64_t blockSize)
{

	int retVal;

	retVal = startPartitionSystem(filename, &volumeSize, &blockSize);
	printf("Opened %s, Volume Size: %llu;  BlockSize: %llu; Return %d;\n\n", filename, (ull_t)volumeSize, (ull_t)blockSize, retVal);

	//initializes our volume control block
	init(filename, volumeSize, blockSize);

	//initializes our linked list for free space managament
	initializeLinkedList(blockSize);

	//closes partition for VCB
	closePartitionSystem();
}
