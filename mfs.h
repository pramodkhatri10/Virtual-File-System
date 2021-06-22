/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: fsLow.h
*
* Description: 
*	This is the file system interface.
*	This is the interface needed by the driver to interact with
*	your filesystem.
*
**************************************************************/
#ifndef _MFS_H
#define _MFS_H
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


#include "b_io.h"

#include <dirent.h>
#define FT_REGFILE	DT_REG
#define FT_DIRECTORY DT_DIR
#define FT_LINK	DT_LNK

#ifndef uint64_t
typedef u_int64_t uint64_t;
#endif
#ifndef uint32_t
typedef u_int32_t uint32_t;
#endif

#define MAXFILENAMESIZE 30
#define MAXPATHNAMESIZE 200
#define MAXNUMOFCHILDREN 10



typedef enum{
	IS_DIR, IS_FILE
}fileType;

struct fs_diriteminfo
	{
    unsigned short d_reclen;    /* length of this record */
    unsigned char d_fileType;    //
    char d_name[256]; 			/* filename max filename is 255 characters */
	};

typedef struct
	{
	/*****TO DO:  Fill in this structure with what your open/read directory needs  *****/
	char name[MAXFILENAMESIZE]; /*directory or file name*/
	char path[MAXPATHNAMESIZE]; /*the path name of this file or directory */
	char parent[MAXPATHNAMESIZE]; /*the parent of the file or directory */
	char child[MAXNUMOFCHILDREN][MAXFILENAMESIZE]; /*the children of the file or directory */
	unsigned short  d_reclen;		/*length of this record */
	uint64_t	directoryStartLocation;		/*Starting LBA of directory */
	fileType type; /*tells whether this is a file or a directory*/
	int childLocation[MAXNUMOFCHILDREN]; //the position of all its children
	int parentLocation;//the position of its parent
	} fdDir;


int fs_mkdir(const char *pathname, mode_t mode);
int fs_rmdir(const char *pathname);
fdDir * fs_opendir(const char *name);
struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp);

char * fs_getcwd(char *buf, size_t size);
int fs_setcwd(char *buf);   //linux chdir
int fs_isFile(char * path);	//return 1 if file, 0 otherwise
int fs_isDir(char * path);		//return 1 if directory, 0 otherwise
int fs_delete(char* filename);	//removes a file

void allocateMyFS(); //callocs the fdDir global variable called myFS
void freeMyFS(); //frees the fdDir global variable



struct fs_stat
	{
	off_t     st_size;    		/* total size, in bytes */
	blksize_t st_blksize; 		/* blocksize for file system I/O */
	blkcnt_t  st_blocks;  		/* number of 512B blocks allocated */
	
	/* add additional attributes here for your file system */
	};

int fs_stat(const char *path, struct fs_stat *buf);

#endif

