/**************************************************************
* Class::  CSC-415-04 Spring 2024
* Name:: Haley Park
* Student ID:: 923812276
* GitHub-Name:: jung-hyeon
* Project:: Assignment 4 – Processing CSV Data with Threads
*
* File:: Park_Haley_HW4_csv.c
*
* Description::
* This file contains functions for parsing and processing CSV data. 
* It defines functions for opening, reading, and closing CSV files, 
* as well as parsing each line of the CSV file to extract individual tokens.
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "Park_Haley_HW4_csv.h"

#define MAX_COL_SIZE 50

static char **header = NULL;

static FILE *csv_file = NULL;
static char *line_buffer = NULL;
static size_t line_buffer_size = 0;
static ssize_t read;

char ** parse_line_buffer(){
    char cur_char;
    int start_idx = 0;
    int end_idx = 0;
    int quotation_mark_cnt = 0;
    int prev_str_size;
    int cur_str_size;
    char * prev_str = NULL;
    char ** cur_str = (char **)malloc(sizeof(char *) * MAX_COL_SIZE);
    int cur_str_idx = 0;
    int next_line_flag = 0;
    int cur_line_flag = 0;
    // Check the characters one by one while turning the repetition statement by line_buffer_size.
    for (int i = 0; i < line_buffer_size ; i++){
        cur_line_flag = 0;
        end_idx = i;
        cur_str[cur_str_idx] = NULL;
        if (line_buffer[i-1] == '\n') break;
        cur_char = line_buffer[i];
        // If the current text is ‘“‘
        if (cur_char == '"'){
            quotation_mark_cnt++;
            // Check double quotations and change double quotations to single quotations.
            if (line_buffer[i+1] == '"') { 
                strcpy(line_buffer + i, line_buffer + i + 1);
            }
            // If the following character == ‘,’and quotations_mark_cnt is an even number, 
            //it means the end of the string token (unless it is in the string). 
            //Therefore, it initializes quotations_mark_cnt to 0.
            else if (line_buffer[i+1] == ',' && quotation_mark_cnt % 2 == 0){ 
                quotation_mark_cnt = 0;
            }
        }
        // If the current character == ‘,’ and quotations_mark_cnt is 0, (if the end of token)
        else if (cur_char == ',' && quotation_mark_cnt == 0){
            // tokenizing

            // If the current tokenization is a string, assign it to cur_str except for "" at both ends.
            if (line_buffer[end_idx-1] == '"'){
                // If next_line_flag is true here, it means that there is no " at the front, 
                // so it is assigned to cur_str except for the last ".
                if (next_line_flag){
                    cur_str_size = end_idx - start_idx-1;
                    cur_str[cur_str_idx] = (char*)malloc(sizeof(char) * cur_str_size + 1);
                    strncpy(cur_str[cur_str_idx], line_buffer+start_idx, cur_str_size);
                    cur_str[cur_str_idx][cur_str_size] = '\0';
                }
                else {
                    cur_str_size = end_idx - start_idx-2;
                    cur_str[cur_str_idx] = (char*)malloc(sizeof(char) * cur_str_size + 1);
                    strncpy(cur_str[cur_str_idx], line_buffer+start_idx+1, cur_str_size);
                    cur_str[cur_str_idx][cur_str_size] = '\0';
                }
            }
            // If it's not a string, assign the entire token to cur_str.
            else {
                cur_str_size = end_idx - start_idx;
                cur_str[cur_str_idx] = (char*)malloc(sizeof(char) * cur_str_size + 1);
                strncpy(cur_str[cur_str_idx], line_buffer+start_idx, cur_str_size);
                cur_str[cur_str_idx][cur_str_size] = '\0';
            }
            start_idx = end_idx + 1;
            quotation_mark_cnt = 0;
            cur_str_idx++;
            cur_line_flag = 1;
        } 
        // If the current character == '\n'
        else if (cur_char == '\n'){
            // If quotations_mark_cnt is even (if it's the end of token)
            if (quotation_mark_cnt % 2 == 0){
                // tokenizing
                if (line_buffer[end_idx-1] == '"'){
                    if (next_line_flag){
                        cur_str_size = end_idx - start_idx-1;
                        cur_str[cur_str_idx] = (char*)malloc(sizeof(char) * cur_str_size + 1);
                        strncpy(cur_str[cur_str_idx], line_buffer+start_idx, cur_str_size);
                        cur_str[cur_str_idx][cur_str_size] = '\0';
                    }
                    else {
                        cur_str_size = end_idx - start_idx-2;
                        cur_str[cur_str_idx] = (char*)malloc(sizeof(char) * cur_str_size + 1);
                        strncpy(cur_str[cur_str_idx], line_buffer+start_idx+1, cur_str_size);
                        cur_str[cur_str_idx][cur_str_size] = '\0';
                    }
                }
                else {
                    cur_str_size = end_idx - start_idx;
                    cur_str[cur_str_idx] = (char*)malloc(sizeof(char) * cur_str_size + 1);
                    strncpy(cur_str[cur_str_idx], line_buffer+start_idx, cur_str_size);
                    cur_str[cur_str_idx][cur_str_size] = '\0';
                }
                start_idx = end_idx + 1;
                quotation_mark_cnt = 0;
                cur_str_idx++;
                cur_line_flag = 1;
            }
            // If not, it means that there is a '\n' in the middle of the string. 
            // Therefore, the tokens so far are put in prev_str and a new line is received through getline. 
            // After that, parse the above process by repeating it in the same way.
            else {
                next_line_flag = 1;
                // get next line
                prev_str_size = end_idx - start_idx + 1;
                prev_str = (char*)malloc(sizeof(char) * prev_str_size + 1);
                strncpy(prev_str, line_buffer+start_idx+1, prev_str_size);
                prev_str[prev_str_size] = '\0';


                read = getline(&line_buffer, &line_buffer_size, csv_file);
                //printf("%s\n", line_buffer); 
                if (read == -1) {
                    free(line_buffer);
                    printf("Error: getline");
                    exit(EXIT_FAILURE);
                }
                i = 0;
                start_idx = 0;
                end_idx = 0;
                quotation_mark_cnt = 1;
            }
        }
        // If the current string token has '\n' in the middle
        // Prev_str and cur_str are combined and assigned to cur_str.
        if (next_line_flag == 1 && cur_line_flag == 1){
            char merge_str[prev_str_size + cur_str_size];
            strcpy(merge_str, prev_str);
            strcat(merge_str, cur_str[cur_str_idx - 1]);
            cur_str_size += prev_str_size;
            cur_str[cur_str_idx - 1] = (char*)realloc(cur_str[cur_str_idx - 1], sizeof(char) * cur_str_size + 1);
            strcpy(cur_str[cur_str_idx - 1], merge_str);
            cur_str[cur_str_idx - 1][cur_str_size] = '\0';

            free(prev_str);
            prev_str = NULL;

            next_line_flag = 0;
            cur_line_flag = 0;
        }
    }
    return cur_str;   
}

char ** csvopen (char * filename){
    csv_file = fopen(filename, "r");
    if (csv_file == NULL){
        printf("Error: File open");
        return NULL;
    }
    header = csvnext(); // Read the header of the CSV file
    return header;
}

char ** csvnext (void){
    line_buffer = NULL;
    line_buffer_size = 0;
    read = getline(&line_buffer, &line_buffer_size, csv_file); // Read the next line
    if (read == -1) {
        free(line_buffer);
        printf("Error: getline");
        exit(EXIT_FAILURE);
    }
    return parse_line_buffer(); // Parse the line into tokens
}

char ** csvheader (void){
    // return header
    return header;
}

int csvclose(void) {
    int data_line_count = 0;
    
    // Close the CSV file
    if (csv_file != NULL) {
        if (fclose(csv_file) == EOF) {
            perror("Error closing file");
            return -1; // Return -1 if an error occurs during file closure
        }
    } else {
        return -1; // Return -1 if 'csv_file' is NULL
    }
    
    // Free memory associated with the header
    if (header != NULL) {
        for (int i = 0; header[i] != NULL; i++) {
            free(header[i]);
        }
        free(header);
    }

    // Free memory associated with the line buffer
    free(line_buffer);

    // Count the number of data lines read
    if (csv_file != NULL) {
        data_line_count = ftell(csv_file) - 1; 
    }

    return data_line_count; // Return the number of data lines read
}
