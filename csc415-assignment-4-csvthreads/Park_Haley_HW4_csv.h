/**************************************************************
* Class::  CSC-415-04 Spring 2024
* Name:: Haley Park
* Student ID:: 923812276
* GitHub-Name:: jung-hyeon
* Project:: Assignment 4 – Processing CSV Data with Threads
*
* File:: Park_Haley_HW4_csv.h
*
* Description::
* This file contains functions for parsing and processing CSV data. 
* It defines functions for opening, reading, and closing CSV files, 
* as well as parsing each line of the CSV file to extract individual tokens.
**************************************************************/
#ifndef CSV_H
#define CSV_H

/*
* Parses a line from the CSV file and splits it into tokens.
* Returns an array of strings, each representing a token.
*/
char ** parse_line_buffer();

/* 
 * Opens the CSV file specified by 'filename' and reads the header.
 * Returns the header as an array of strings.
 */
char ** csvopen (char * filename);

/* 
 * Reads the next line from the CSV file and parses it into tokens.
 * Returns an array of strings representing the tokens.
 */
char ** csvnext (void);

/* 
 * Returns the header of the CSV file.
 */
char ** csvheader (void);

/* 
 * Closes the CSV file, frees memory, and returns the number of data lines read.
 * Returns -1 if an error occurs during file closure.
 */
int csvclose (void);

#else
#error Do Not include header file csv.h more than once
#endif