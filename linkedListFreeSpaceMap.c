/**************************************************************
 * Class:  CSC-415-01
 * Name: Pramod Khatri, Hareesh Pedireddi, Vito Gano, Emin Musayev
 * Student ID: 920831584, 920425984, 916229878, 916823510
 * Project: Virtual File System
 *
 * File: linkedListFreeSpaceMap.c
 *
 * Description: This sets up our Free Space Management using Linked Lists.
 * 
 * 
 *              
 **************************************************************/

#include "linkedListFreeSpaceMap.h"

//this is the head of the linked list
Node *head;
int initialized;

vcb *myVCBInfo;

fdDir *root;

#define MAXFILENAMESIZE 30
#define MAXNUMOFCHILDREN 10
#define STARTINGBLOCKLOCATION 0
#define NUMOFCHILDERNSTORING 100

//this initializes our free space management.
void initializeFreeSpaceBlock(uint64_t blockBytes)
{
    printf("Initializing the Free Space\n");
    int calFS = roundUpDiv(sizeof(Node), blockBytes);
    int calRoot = roundUpDiv(sizeof(fdDir), blockBytes);
    head = calloc(calFS, calFS * blockBytes); //starts allocating the head which depends on the blocksize times the size of the Node
    head->next = NULL;
    head->blocksAvailable = myVCBInfo->freeSpaceAvailable;
    head->startingFreeBlock = (myVCBInfo->rootLocation) + calRoot;
    fdDir *children[NUMOFCHILDERNSTORING];
    int numOfChildren = 0;
    for (int i = 0; i < NUMOFCHILDERNSTORING; i++)
    {
        children[i] = calloc(calRoot, blockBytes * calRoot);
    }
    for (int i = 0; i < MAXNUMOFCHILDREN; i++)
    {
        if (root->child[i][0] != NULL)
        {
            fdDir *child = calloc(calRoot, blockBytes * calRoot);
            LBAread(child, calRoot, root->childLocation[i]);
            LBAwrite(child, calRoot, head->startingFreeBlock);
            root->childLocation[i] = head->startingFreeBlock;
            //taking the root's children
            memcpy(children[numOfChildren], child, blockBytes * calRoot);
            numOfChildren++;
            head->startingFreeBlock += calRoot;
            head->blocksAvailable -= calRoot;
            free(child);
        }
    }

    //checking if the roots children also has children
    while (numOfChildren > 0)
    {
        //making a temporary variable
        fdDir *temp = calloc(calRoot, blockBytes * calRoot);
        memcpy(temp, children[numOfChildren - 1], blockBytes * calRoot); //copying the child
        numOfChildren--;

        //checking if temp has any children
        for (int i = 0; i < MAXNUMOFCHILDREN; i++)
        {
            //when there is a child
            if (temp->child[i][0] != NULL)
            {
                fdDir *child = calloc(calRoot, blockBytes * calRoot);
                LBAread(child, calRoot, temp->childLocation[i]);
                LBAwrite(child, calRoot, head->startingFreeBlock);
                temp->childLocation[i] = head->startingFreeBlock;
                
                //updating it's children
                for(int j = 0; j< MAXNUMOFCHILDREN; j++){
                    if (temp->child[i][0] != NULL){
                        fdDir *itschildren = calloc(calRoot, blockBytes * calRoot);
                        LBAread(itschildren, calRoot, itschildren->childLocation[i]);
                        //updating its parent's location
                        itschildren -> parentLocation = temp->childLocation[i];
                        LBAwrite(itschildren, calRoot, itschildren->childLocation[i]);
                        free(itschildren);
                    }
                }

                //taking the temp's children
                memcpy(children[numOfChildren], child, blockBytes * calRoot);
                numOfChildren++;

                //updating the head
                head->startingFreeBlock += calRoot;
                head->blocksAvailable -= calRoot;
                free(child);
            }
        }
        free(temp);
    }
    for (int i = 0; i < MAXNUMOFCHILDREN; i++)
    {
        free(children[i]);
    }
    printf("Finished Initializing the Free Space\n");
}

//initializes free sapce
//validates if fress space is already allocated
void initializeLinkedList(uint64_t blockNumber)
{
    if (initialized == 0)
    {
        printf("\nAllocating Free Space Map to the Volume......... \n");
        initializeRoot(blockNumber);
        printf("Allocation is successful! \n");
        initialized = 1;
    }
    else
    {
        printf("Free Space is Already Allocated!");
    }
}

//initializes free space block
//dispays free sapce block, VCB info, node ID, node header and size of the directory in test
void initializeRoot(uint64_t blockBytes)
{
    int calVCB = roundUpDiv(sizeof(vcb), blockBytes);    //calculates the number of blocks used for the Volume Control Block
    int calRoot = roundUpDiv(sizeof(fdDir), blockBytes); //calculates the number of blocks used for Root Directory
    root = calloc(calRoot, calRoot * blockBytes);
    vcb *testVCB = calloc(calVCB, blockBytes * calVCB);

    LBAread(testVCB, calVCB, STARTINGBLOCKLOCATION);

    int rootStartingLocation = (calVCB); //sets the root location as the number of blokcs that was used for the Volume Control Block
    //creating our root for the volume
    strcpy(root->name, "root");
    strcpy(root->path, "/root");
    strcpy(root->parent, "root"); //the parent for the root is itself
    root->parentLocation = rootStartingLocation;
    for (int i = 0; i < MAXNUMOFCHILDREN; i++)
    { //creating its children
        root->child[i][0] = NULL;
        root->childLocation[i] = 0;
    }
    root->d_reclen = sizeof(fdDir);
    root->directoryStartLocation = rootStartingLocation;

    testVCB->freeSpaceAvailable = (testVCB->blocksInVolumeBytes) - calVCB - calRoot; //allows the volume control block to know the updated free space available.

    LBAwrite(root, calRoot, rootStartingLocation);

    testVCB->fSBlockLocation = testVCB->blockSizeofVCB + calRoot;
    testVCB->rootLocation = rootStartingLocation;

    LBAwrite(testVCB, calVCB, STARTINGBLOCKLOCATION);

    printf("Size of Directory: %ld \n", sizeof(fdDir));

    printf("Next Free Space Block Location: %d \n", testVCB->fSBlockLocation);

    printf("Root Position: %ld\n", testVCB->rootLocation);
    // printMyNode(testNode);

    printf("Available Free Space Blocks: %d\n", testVCB->freeSpaceAvailable);


    free(root);
    free(testVCB);
}


//this opens our VCB and Root Directories in addition to creating our linked list nodes.
void openVolume(char *name)
{
    uint64_t existingVolumeSize;
    uint64_t sizeTest;
    int retVal = startPartitionSystem(name, &existingVolumeSize, &sizeTest);

    int vcbBlocks = roundUpDiv(sizeof(vcb), sizeTest);
    int rootBlocks = roundUpDiv(sizeof(fdDir), sizeTest);
    head = NULL;
    myVCBInfo = calloc(vcbBlocks, sizeTest * vcbBlocks);
    root = calloc(rootBlocks, rootBlocks * sizeTest);

    
    LBAread(myVCBInfo, vcbBlocks, STARTINGBLOCKLOCATION);
    LBAread(root, rootBlocks, vcbBlocks);

    printf("Opening the Volume. \n\n");
    if (!retVal)
    {
        initializeFreeSpaceBlock(sizeTest);
    }
}

void closeVolume()
{
    printf("Closing the Volume. \n\n");
    free(myVCBInfo);
    free(root);
    //deleting linklist
    while (head != NULL)
    {
        Node *temp = head;
        head = head->next;
        free(temp);
    }
}

uint64_t getNumVolBlocks(){ // return bytes per blocks 
    return myVCBInfo -> numVolBlocks;
}

uint64_t getRootLocation(){ //gets the block number of the root
    return myVCBInfo -> rootLocation;
}

int checkFreeSpace(int num){ //returns a block number available for free space
    Node * localLinkedListPtr = head;
    while(localLinkedListPtr != NULL){ // traversing to the linked list
        if(num <= localLinkedListPtr -> blocksAvailable){//checking the num of blocks can be allocated in the blocks available
            return localLinkedListPtr -> startingFreeBlock;
        }
        localLinkedListPtr = localLinkedListPtr -> next;
    }
    return -1; // returns -1 when there is no more free space block available
}

void changeFreeSpaceBlockAvailable(int numBlocks, int startLocation){ //changes the location of the free space blocka available
    Node * localLinkedListPtr = head;
    while(localLinkedListPtr != NULL){ // traversing the linked list 
        if(startLocation == localLinkedListPtr -> startingFreeBlock){//finding the node that needs to be changed
            localLinkedListPtr -> startingFreeBlock += numBlocks;
            localLinkedListPtr -> blocksAvailable -= numBlocks;
        }
        localLinkedListPtr = localLinkedListPtr -> next;
    }
}