/**************************************************************
* Class::  CSC-415-04 Spring 2024
* Name:: Haley Park
* Student ID:: 923812276
* GitHub-Name:: jung-hyeon
* Project:: Assignment 5 – Buffered I/O read
*
* File:: b_io.c
*
* Description::
* This C file implements buffered I/O operations with manual buffer management.
* It provides functions for opening, reading, and closing files (b_open, b_read, b_close).
* Each file descriptor corresponds to a file control block (FCB), 
* containing file information and buffer management variables. 
* The b_open function initializes the FCB, allocates buffer memory, 
* and returns the file descriptor. b_read reads data from the buffer or disk using LBAread,
* handling cases where requested bytes exceed buffer size. 
* b_close frees resources associated with the file descriptor.
**************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "b_io.h"
#include "fsLowSmall.h"

#define MAXFCBS 20	//The maximum number of files open at one time


// This structure is all the information needed to maintain an open file
// It contains a pointer to a fileInfo strucutre and any other information
// that you need to maintain your open file.
typedef struct b_fcb
	{
	fileInfo * fi;	//holds the low level systems file info

	// Add any other needed variables here to track the individual open file
	char * buffer; // own buffer
	int buf_index; // buffer index that is not read yet 
    int buf_remain; // ramain data size that is not read yet
	int block_count; // block count that is read
	int file_index; // file index that is not read yet

	} b_fcb;
	
//static array of file control blocks
b_fcb fcbArray[MAXFCBS];

// Indicates that the file control block array has not been initialized
int startup = 0;	

// Method to initialize our file system / file control blocks
// Anything else that needs one time initialization can go in this routine
void b_init ()
	{
	if (startup)
		return;			//already initialized

	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].fi = NULL; //indicates a free fcbArray
		}
		
	startup = 1;
	}

//Method to get a free File Control Block FCB element
b_io_fd b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].fi == NULL)
			{
			fcbArray[i].fi = (fileInfo *)-2; // used but not assigned
			return i;		//Not thread safe but okay for this project
			}
		}

	return (-1);  //all in use
	}

// b_open is called by the "user application" to open a file.  This routine is 
// similar to the Linux open function.  	
// You will create your own file descriptor which is just an integer index into an
// array of file control blocks (fcbArray) that you maintain for each open file.  
// For this assignment the flags will be read only and can be ignored.

b_io_fd b_open (char * filename, int flags)
	{
	if (startup == 0) b_init();  //Initialize our system

	//*** TODO ***//  Write open function to return your file descriptor
	//				  You may want to allocate the buffer here as well
	//				  But make sure every file has its own buffer

	// This is where you are going to want to call GetFileInfo and b_getFCB
	
    // Get file information
    fileInfo *info = GetFileInfo(filename);
    if (info == NULL) {
        // File not found
        return -1;
    }

    // Get a free file control block (FCB)
    b_io_fd fd = b_getFCB();
    if (fd == -1) {
        // No free FCB available
        return -1;
    }

    // Allocate memory for the buffer
    // You can use malloc() or another memory allocation function
    char * buffer = malloc(B_CHUNK_SIZE);
    if (buffer == NULL) {
        // Memory allocation failed
        return -1;
    }

    // Populate the FCB with necessary information
    fcbArray[fd].fi = info;
    // Assign buffer to FCB or any other required variables
    fcbArray[fd].buffer = buffer;

	// Initialize variables
	fcbArray[fd].buf_index = 0;
	fcbArray[fd].buf_remain = 0;
	fcbArray[fd].block_count = 0;
	fcbArray[fd].file_index = 0;

    // Return the file descriptor
    return fd;
}

// b_read functions just like its Linux counterpart read.  The user passes in
// the file descriptor (index into fcbArray), a buffer where thay want you to 
// place the data, and a count of how many bytes they want from the file.
// The return value is the number of bytes you have copied into their buffer.
// The return value can never be greater then the requested count, but it can
// be less only when you have run out of bytes to read.  i.e. End of File	
int b_read (b_io_fd fd, char * buffer, int count)
	{
	//*** TODO ***//  
	// Write buffered read function to return the data and # bytes read
	// You must use LBAread and you must buffer the data in B_CHUNK_SIZE byte chunks.
		
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}

	// and check that the specified FCB is actually in use	
	if (fcbArray[fd].fi == NULL)		//File not open for this descriptor
		{
		return -1;
		}	

	// Your Read code here - the only function you call to get data is LBAread.
	// Track which byte in the buffer you are at, and which block in the file	

	// if the bytes that user request is bigger than file size, change the count 
	// to remained file bytes
	if (fcbArray[fd].file_index + count > fcbArray[fd].fi->fileSize){
		count = fcbArray[fd].fi->fileSize - fcbArray[fd].file_index;
	}

	// the bytes that read from now on
	int read_count = 0;
	
	// If bytes that remain in own buffer is bigger than user requested bytes,
	// directly buffer from own buffer to caller's buffer (not need to LBAread)
	// because own buffer already has bytes
	if (fcbArray[fd].buf_remain >= count){
		// Copy memory as many bytes as user wants directly from own buffer 
		memcpy(buffer, fcbArray[fd].buffer + fcbArray[fd].buf_index, count);
		read_count += count;
		fcbArray[fd].buf_index += read_count;
		fcbArray[fd].buf_remain -= read_count;
	} 
	// Need to use LBAread to get data (need to get new data)
	else{
		// copy memory from own buffer if it is not empty (Before get new data)
		if (fcbArray[fd].buf_remain > 0){
			// copy memory as many bytes as bytes remained in own buffer
			memcpy(buffer, fcbArray[fd].buffer + fcbArray[fd].buf_index, fcbArray[fd].buf_remain);
			read_count += fcbArray[fd].buf_remain;
			fcbArray[fd].buf_index = 0;
			fcbArray[fd].buf_remain = 0;
		}
		// if bytes wanted by user are bigger than B_CHUNK_SIZE, 
		// buffer directly from file to user buffer
		if (read_count + B_CHUNK_SIZE <= count)
		{
			// calculate needed lba_count 
			int lba_count = (count - read_count) / B_CHUNK_SIZE;
			// LBAread from file to buffer as many blocks as needed
			uint64_t blocks_read = LBAread(buffer + read_count, lba_count, fcbArray[fd].fi->location + fcbArray[fd].block_count);
			fcbArray[fd].block_count += blocks_read;
			fcbArray[fd].buf_index = 0;
			fcbArray[fd].buf_remain = 0;

			read_count += B_CHUNK_SIZE * lba_count;
		}

		// Read B_CHUNK_SIZE a byte chunks from LBAread into the buffer
		uint64_t blocks_read = LBAread(fcbArray[fd].buffer, 1, fcbArray[fd].fi->location + fcbArray[fd].block_count);
		fcbArray[fd].block_count += blocks_read;

		// Buffer the remaining bytes that user wants from own buffer to user buffer
		int remain_count = count - read_count;
		memcpy(buffer + read_count, fcbArray[fd].buffer + fcbArray[fd].buf_index, remain_count);
		read_count += remain_count;
		fcbArray[fd].buf_index = remain_count;
		fcbArray[fd].buf_remain = B_CHUNK_SIZE - fcbArray[fd].buf_index;
	}
	// track file index
	fcbArray[fd].file_index += read_count;
	return read_count;
	}
	


// b_close frees and allocated memory and places the file control block back 
// into the unused pool of file control blocks.
int b_close (b_io_fd fd)
	{
	//*** TODO ***//  Release any resources
    
	// Check if the file descriptor is valid
    if (fd < 0 || fd >= MAXFCBS)
    {
        return -1; // Invalid file descriptor
    }

    // Check if the file is open
    if (fcbArray[fd].fi == NULL)
    {
        return -1; // File not open for this descriptor
    }

    // Free the buffer associated with the file
    free(fcbArray[fd].buffer);
	fcbArray[fd].buffer = NULL;

    // Reset the file control block
    fcbArray[fd].fi = NULL;

    // Return success
    return 0;
	}
	
