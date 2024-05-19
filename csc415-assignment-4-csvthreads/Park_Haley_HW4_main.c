/**************************************************************
* Class::  CSC-415-04 Spring 2024
* Name:: Haley Park
* Student ID:: 923812276
* GitHub-Name:: jung-hyeon
* Project:: Assignment 4 – Processing CSV Data with Threads
*
* File:: Park_Haley_HW4_main.c
*
* Description::
*
**************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include <linux/time.h>

int main (int argc, char *argv[])
    {
    //***TO DO***  Look at arguments, initialize application

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************
    
    // *** TO DO ***  start your thread processing
    //                wait for the threads to finish


    // ***TO DO *** Display Data

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Clock output
    clock_gettime(CLOCK_REALTIME, &endTime);
    time_t sec = endTime.tv_sec - startTime.tv_sec;
    long n_sec = endTime.tv_nsec - startTime.tv_nsec;
    if (endTime.tv_nsec < startTime.tv_nsec)
        {
        --sec;
        n_sec = n_sec + 1000000000L;
        }

    printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);
    //**************************************************************


    // ***TO DO *** cleanup
    }

