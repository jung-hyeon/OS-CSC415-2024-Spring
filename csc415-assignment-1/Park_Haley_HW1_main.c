/**************************************************************
* Class::  CSC-415-04 Spring 2024
* Name::  Haley
* Student ID::  923812276
* GitHub-Name:: jung-hyeon
* Project:: Assignment 1 – Command Line Arguments
*
* File:: Park_Haley_HW1_main.c
*
* Description::
* This assignment is to write a C program that accepts arguments via the command line and 
* then displays each of those arguments to the terminal along with how many arguments there are.
**************************************************************/

#include <stdio.h>

int main (int argc, char* argv[])
{
    printf("There were %d arguments on the command line.\n", argc);
    
    int count = 0;
    for (count = 0; count < argc; count++)
    {
        printf("Argument %d:    ", count);
        printf("%s\n", argv[count]);
    }
    return 0;
}


