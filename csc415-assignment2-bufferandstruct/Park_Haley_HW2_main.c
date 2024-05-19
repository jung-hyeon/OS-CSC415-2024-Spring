/**************************************************************
* Class::  CSC-415-04 Spring 2024
* Name:: Haley Park
* Student ID:: 923812276
* GitHub-Name:: jung-hyeon
* Project:: Assignment 2 – Stucture in Memory and Buffering
*
* File:: Park_Haley_HW2_main.c
*
* Description::
* This assignment is to allocate an instantiation of the personInfo structure
* and to populate it then write personal information structure.
* Also getting a series of C strings by using string Buffer. 
* When the buffer is filled, flush out Buffer.
* Plus, reading hexadecimal dump and understanding binary representation of 
* data in memory and how it can be used for debugging and ensuring code works.
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#include <assignment2.h>

#define MESSAGE_LENGTH 100

int main(int argc, char* argv[]) {
	// allocate an instantiation of the personInfo structure and populate it
	struct personalInfo* pInfo = (personalInfo*)malloc(sizeof(struct personalInfo));
	// initialize and assign values (firstName, lastName, studentID, level, languages and message)
	pInfo->firstName = (char*)malloc(sizeof(char) * (strlen(argv[1])+1)); 
	pInfo->lastName = (char*)malloc(sizeof(char) * (strlen(argv[2])+1)); 
	strcpy(pInfo->firstName, argv[1]); 
	strcpy(pInfo->lastName, argv[2]); 
	printf("firstName address: %p\n", pInfo->firstName);
	printf("lastName address: %p\n", pInfo->lastName);

	pInfo->studentID = 923812276;
	pInfo->level = SENIOR;
	pInfo->languages = KNOWLEDGE_OF_C | KNOWLEDGE_OF_JAVA | KNOWLEDGE_OF_CPLUSPLUS;
	strncpy(pInfo->message, argv[3], MESSAGE_LENGTH);

	// write personal information structure
	if (writePersonalInfo(pInfo) != 0){
		printf("Error: writePersonalInfo");
	}

	// deallocate Memory
	free(pInfo->firstName);
	free(pInfo->lastName);
	free(pInfo);

	// getting a series of C strings
	char* buffer = (char*)malloc(BLOCK_SIZE);
	int usedBufferSize = 0;

	// gather string in buffer
	// commitBlock if buffer full
	// repeat until string is null 
	while(true) {
		const char* newSring = getNext();
		if (newSring == NULL) {
			// flush out buffer
			commitBlock(buffer);
			break;
		}

		// copy the data into the buffer in chunks
		usedBufferSize = strlen(buffer);
		memcpy(buffer + usedBufferSize, newSring, BLOCK_SIZE-usedBufferSize);

		// limit buffer size to BLOCK_SIZE(256)
		if (usedBufferSize + strlen(newSring) >= BLOCK_SIZE) {
			// flush out buffer
			commitBlock(buffer);

			// assign overflowing string to flushed buffer newly
			memcpy(buffer, newSring + (BLOCK_SIZE - usedBufferSize), BLOCK_SIZE);
			usedBufferSize += strlen(newSring) - BLOCK_SIZE;
		}
	}

	// deallocate Memory
	free(buffer);

	return checkIt();
}

