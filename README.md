# Virtual File System in C


Over the past month our team have been designing components of a file system.  We have defined the goals and designed the directory entry structure, the volume structure and the free space.  We have written buffered read and write routines.  Now it is time to implement our file system.  

The routines are in fsLow.c, the necessary header for us to include file is fsLow.h. There are 4 key functions:

```
int startPartitionSystem (char * filename, uint64_t * volSize, uint64_t * blockSize);
```

startPartitionSystem – we specify a file name that is the “volume” of our hard drive.  The volume size is rounded to even block size and is only used when creating the volume file.  The block size must be a power of 2 (nominally 512 bytes).  

On Return, the function will return 0 if success or a negative number if an error occurs.  The values pointed to by volSize and blockSize are filled in with the values of the existing volume file.


`int closePartitionSystem ();`

closePartitionSystem – closes down the volume file and must be called prior to terminating the process, but no read or writes can happen after this call.


`uint64_t LBAwrite (void * buffer, uint64_t lbaCount, uint64_t lbaPosition);`

`uint64_t LBAread (void * buffer, uint64_t lbaCount, uint64_t lbaPosition);`

LBAread and LBAwrite take a buffer, a count of LBA blocks and the starting LBA block number (0 based).  The buffer must be large enough for the number of blocks * the block size.

On return, these function returns the number of **blocks** read or written.


**Our Task is to write a file system!** 

We will need to format our volume, create and maintain a free space management system, initialize a root directory and maintain directory information, create, read, write, and delete files, and display info.  See below for specifics.


Some specifics - we need to provide the following interfaces:

```
int b_open (char * filename, int flags);
int b_read (int fd, char * buffer, int count);
int b_write (int fd, char * buffer, int count);
int b_seek (int fd, off_t offset, int whence);
void b_close (int fd);

```


For our purposes use 10,000,000 or less (minimum 500,000) bytes for the volume size and 512 bytes per sector.  These are the values to pass into startPartitionSystem.




