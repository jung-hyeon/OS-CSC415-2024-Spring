/**************************************************************
* Class::  CSC-415-04 Spring 2024
* Name:: Haley Park
* Student ID:: 923812276
* GitHub-Name:: jung-hyeon
* Project:: Assignment 6 - Device Driver
*
* File:: Park_Haley_HW6_main.c
*
* Description:: In this c file, the driver created is tested by user
* 
**************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEV_NAME "/dev/Park_Haley_HW6_DeviceDriver"
#define MAX_LEN 128

int main (int argc, char * argv[]) 
{
    int fd;
    char read_buf[ MAX_LEN ];
    long read_ret, write_ret;
    char mystr[MAX_LEN];
    int key;

    fd = open(DEV_NAME, O_RDWR);
    printf("Returned from open file. %d\n", fd);
    if (fd < 0) 
    {
        printf("Device Open Error\n");
        perror("Device File Open Error");
        return -1;
    }
    else 
    {
        printf("Device Open Success.\n");
    }
    printf("\n");

    printf(">> Enter your string: ");
    scanf("%s", mystr);
    printf("\n");

    write_ret = write(fd, mystr, strlen(mystr));
    printf("write: %s, that returned %ld\n", mystr, write_ret);

    ioctl(fd, 1, 1);
	read_ret = read(fd, read_buf, strlen(mystr));
	printf("encryption result: %s, that returned %ld\n", read_buf, read_ret);

    ioctl(fd, 1, 2);
	read_ret = read(fd, read_buf, strlen(mystr));
	printf("decryption result: %s, that returned %ld\n\n", read_buf, read_ret);

    close(fd);

    return 0;
}