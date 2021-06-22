/**************************************************************
 * Class:  CSC-415-01
 * Name: Pramod Khatri, Hareesh Pedireddi, Vito Gano, Emin Musayev
 * Student ID: 920831584, 920425984, 916229878, 916823510
 * Project: Virtual File System
 *
 * File: mfs.c
 *
 * Description: Defines the functions to initialize, keep track of, and change our file system's inodes.
 *
 **************************************************************/

#include "mfs.h"

fdDir *myFS; // our global variable for storing file descriptor Directory

char currentWorkingDirectory[MAXPATHNAMESIZE];

int fs_mkdir(const char *pathname, mode_t mode) // returns -1 when it fails, otherwise returns 0
{
    printf("Calling fs_mkdir\n");
    int calFD = roundUpDiv(sizeof(fdDir), getNumVolBlocks());
    LBAread(myFS, calFD, getRootLocation());

    char workDirect[MAXPATHNAMESIZE];
    strcpy(workDirect, currentWorkingDirectory);
    const char forwardSlash[2] = "/";
    char *token = strtok(workDirect, forwardSlash); // sets up the tokenizer which separates the words based on the slash character

    while (token != NULL)
    {
        if (strcmp(token, "root") != 0)
        {
            fdDir *myTempFS = calloc(calFD, calFD * getNumVolBlocks());
            for (int i = 0; i < MAXNUMOFCHILDREN; i++)
            { //traverses to the current working directory
                if (strcmp(myFS->child[i], token) == 0)
                {
                    LBAread(myTempFS, calFD, myFS->childLocation[i]);
                    memcpy(myFS, myTempFS, calFD * getNumVolBlocks());
                    break;
                }
            }
            free(myTempFS);
        }
        token = strtok(NULL, forwardSlash);
    }
    //if pathname exists in the current path, then return an error statement.
    for (int i = 0; i < MAXNUMOFCHILDREN; i++)
    {
        if (strcmp(myFS->child[i], pathname) == 0)
        {
            printf("This directory already exists\n");
            return -1;
        }
    }
    printf("Pathname: %s\n", pathname);
    //check linked list nodes for any freespace
    int freeSpaceStartingBlock = checkFreeSpace(calFD);

    //when there is not freespace
    if (freeSpaceStartingBlock < 0)
    {
        printf("There is no more free space in the directory\n");
        return -1;
    }

    fdDir *makingDirectory = calloc(calFD, calFD * getNumVolBlocks());

    //creating our root for the volume
    strcpy(makingDirectory->name, pathname);
    //strcpy(root->path, "/root");
    strcpy(makingDirectory->parent, myFS->name); //the parent for the root is itself
    makingDirectory->parentLocation = myFS->directoryStartLocation;
    for (int i = 0; i < MAXNUMOFCHILDREN; i++)
    { //creating its children
        makingDirectory->child[i][0] = NULL;
        makingDirectory->childLocation[i] = 0;
    }
    makingDirectory->d_reclen = sizeof(fdDir);
    makingDirectory->directoryStartLocation = freeSpaceStartingBlock;
    strcpy(workDirect, currentWorkingDirectory); //string concatenates the current working directory with the filename
    strcat(workDirect, forwardSlash);
    strcat(workDirect, pathname);

    strcpy(makingDirectory->path, workDirect);
    changeFreeSpaceBlockAvailable(calFD, freeSpaceStartingBlock); //changes the free space block available

    int maxLoops = 0;

    printf("MY FS info: %s\n", myFS->name);

    for (maxLoops; maxLoops < MAXNUMOFCHILDREN; maxLoops++)
    { //checks the parents' child array for any uninitialized child and sets that child's information and its block in there
        if (myFS->childLocation[maxLoops] == 0)
        {
            strcpy(myFS->child[maxLoops], pathname);
            printf("Free Space Starting Block: %d\n", freeSpaceStartingBlock);
            myFS->childLocation[maxLoops] = freeSpaceStartingBlock;
            printf("Max Loops: %d\n", maxLoops);
            break;
        }
    }
    if (maxLoops == MAXNUMOFCHILDREN)
    { //if the directory hits the max number of children, returns an error
        printf("Directory is full. Cannot take any more children!\n");
        free(makingDirectory);
        return -1;
    }

    //writes the directory of the child to the free space block
    LBAwrite(makingDirectory, calFD, freeSpaceStartingBlock);
    LBAwrite(myFS, calFD, myFS->directoryStartLocation);
    free(makingDirectory);
    return 0;
}

int fs_rmdir(const char *pathname)
{ // this removes a directory
    printf("Calling fs_rmdir\n");
    return 0;
}

fdDir *fs_opendir(const char *name) // this opens the directory given the arguments that are passed down.
{
    printf("Name: %s\n", name);
    //ls -> ls root
    //
    int calFD = roundUpDiv(sizeof(fdDir), getNumVolBlocks());
    LBAread(myFS, calFD, getRootLocation());
    char workDirect[MAXPATHNAMESIZE];
    strcpy(workDirect, currentWorkingDirectory);
    const char forwardSlash[2] = "/";
    char *token = strtok(workDirect, forwardSlash);

    //going to the current working directory
    while (token != NULL)
    {
        //checking if it is not root
        if (strcmp(token, "root") != 0)
        {
            fdDir *myTempFS = calloc(calFD, calFD * getNumVolBlocks());
            //checking every child
            for (int i = 0; i < MAXNUMOFCHILDREN; i++)
            {
                //checking if it is the child
                if (strcmp(myFS->child[i], token) == 0)
                {
                    LBAread(myTempFS, calFD, myFS->childLocation[i]);
                    memcpy(myFS, myTempFS, calFD * getNumVolBlocks());
                    break;
                }
            }
            free(myTempFS);
        }
        token = strtok(NULL, forwardSlash);
    }

    return myFS;
}

struct fs_diriteminfo directEntry;

int itsChildren;
struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{
    //printf("Calling this function: fs_readdir\n");
    // printf("Dirp Name: %s\n",dirp ->name);
    if (dirp != NULL)
    {
        int calFD = roundUpDiv(sizeof(fdDir), getNumVolBlocks());
        fdDir *child = calloc(calFD, calFD * getNumVolBlocks());
        for (int i = itsChildren; i < MAXNUMOFCHILDREN; i++)
        { //finding the child
            //has child has been found
            if (dirp->childLocation[i] != 0)
            {
                //getting its child
                LBAread(child, calFD, dirp->childLocation[i]);
                itsChildren++;
                break;
            }
            itsChildren++;
        }

        if (itsChildren >= MAXNUMOFCHILDREN)
        {
            //this ends the LS command
            free(child);
            itsChildren = 0;
            return NULL;
        }
        strcpy(directEntry.d_name, child->name);
        directEntry.d_fileType = 'D';
        directEntry.d_reclen = roundUpDiv(sizeof(fdDir), getNumVolBlocks());
        free(child);
        return &directEntry;
    }
    return NULL;
}

int fs_closedir(fdDir *dirp)
{ //this closes the directory
    //printf("Closing the Directory\n\n");
    return 0;
}

char *fs_getcwd(char *buf, size_t size) // retrieves the current working directory
{
    printf("Getting the Current Directory\n\n");
    if (strlen(currentWorkingDirectory) > size)
    {
        printf("Error obtaining the current directory! \n\n");
        return NULL;
    }
    strcpy(buf, currentWorkingDirectory);
    return buf;
}
int fs_setcwd(char *buf) //sets the current working directory
{
    printf("Calling this function: fs_setcwd\n");
    
    if ((strlen(currentWorkingDirectory) + strlen(buf)) < MAXPATHNAMESIZE)
    {
        //setting thr root
        if (strcmp(buf, "/root") == 0)
        {
            strcpy(currentWorkingDirectory, buf);
            return 0;
        }
        
        int calFD = roundUpDiv(sizeof(fdDir), getNumVolBlocks());
        LBAread(myFS, calFD, getRootLocation()); //obtains the root location of the children
        char workDirect[MAXPATHNAMESIZE];
        strcpy(workDirect, currentWorkingDirectory);
        const char forwardSlash[2] = "/";
        char *token = strtok(workDirect, forwardSlash);
        
        
        //going to the current working directory
        while (token != NULL)
        {
            //checking if it is not root
            if (strcmp(token, "root") != 0)
            {
                fdDir *myTempFS = calloc(calFD, calFD * getNumVolBlocks());
                //checking every child
                for (int i = 0; i < MAXNUMOFCHILDREN; i++)
                {
                    //checking if it is the child
                    if (strcmp(myFS->child[i], token) == 0)
                    {
                        LBAread(myTempFS, calFD, myFS->childLocation[i]);
                        memcpy(myFS, myTempFS, calFD * getNumVolBlocks());
                        break;
                    }
                }
                free(myTempFS);
            }
            
            token = strtok(NULL, forwardSlash);
        }
        char Dotdot[3];
        strcpy(Dotdot, "..");
        char dotChar[2];
        strcpy(dotChar, ".");
        
        
        if (strcmp(Dotdot, buf) == 0)
        { // checks if the argument that is passed is ..
            char *myParent;
            strcpy(myParent, myFS->parent);
            strcpy(workDirect, currentWorkingDirectory);
            token = strtok(workDirect, forwardSlash);
            char *changedWD;
            strcat(changedWD, forwardSlash);
            while (strcmp(token, myParent) != 0) //traverses till it reaches the parent
            {
                strcat(changedWD, token);
                strcat(changedWD, forwardSlash);
                token = strtok(NULL, forwardSlash);
            }
            int sizeofchangedWD = strlen(changedWD);
            changedWD[sizeofchangedWD - 1] = '\0'; // changes the last position of the string to null

            strcpy(currentWorkingDirectory, changedWD);
            return 0;
        }
        else if (strcmp(dotChar, buf) == 0)
        {
            return 0;
        }
        char *placeHolder;
        int foundChild = 0;

        for (int i = 0; i < MAXNUMOFCHILDREN; i++)
        { // checking if the parent location has a child
            if (strcmp(myFS->child[i], buf) == 0)
            {
                foundChild = 1;
                strcpy(placeHolder, myFS->child[i]);
                break;
            }
        }
        if (foundChild == 0)
        { // returns error when it can't find anymore children
            printf("Error: Set Working Current Directory not working!\n");
            return -1;
        }
        strcat(currentWorkingDirectory, forwardSlash); // concatenates the current working directory with the pathname
        strcat(currentWorkingDirectory, placeHolder);
        printf("current working directory: %s\n",currentWorkingDirectory);
        return 0;
        
    }
    else
    {
        //prints error
        printf("Set Current Working Directory Error!\n");
        return -1;
    }
}

int fs_isFile(char *path)
{
    printf("Calling this function: fs_isFile\n");
    return 0;
}
int fs_isDir(char *path)
{
    printf("Calling this function: fs_isDir\n");
    return 1;
}
int fs_delete(char *filename)
{
    printf("Calling this function: fs_delete\n");
    return 0;
}

int fs_stat(const char *path, struct fs_stat *buf)
{
    printf("Calling this function: fs_stat\n");
    return 0;
}

void freeMyFS()
{ //frees the fdDir global variable
    free(myFS);
}

void allocateMyFS()
{ //callocs the fdDir global variable called myFS
    int calFD = roundUpDiv(sizeof(fdDir), getNumVolBlocks());
    myFS = calloc(calFD, calFD * getNumVolBlocks());
}
