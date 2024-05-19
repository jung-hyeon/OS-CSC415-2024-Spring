/**************************************************************
* Class::  CSC-415-04 Spring 2024
* Name:: Haley Park
* Student ID:: 923812276
* GitHub-Name:: jung-hyeon
* Project:: Assignment 6 - Device Driver
*
* File:: Park_Haley_HW6_DeviceDriver.c
*
* Description:: 
* This file implements a device driver for a simple encryption program. 
* It provides functionalities to encrypt and decrypt data, read and write strings, 
* and use IOCTL to set encryption/decryption mode and key. 
* The encryption algorithm subtracts a fixed key value from each character 
* of the string, while the decryption algorithm adds the key value to each character. 
* If IOCTL is set to encryption mode, the driver expects a string to be written, 
* then it reads back the encrypted data. If IOCTL is set to decryption mode, 
* the driver expects encrypted data to be written, and it reads back the original message (decrypted).
**************************************************************/

#include <linux/module.h>        // Required for module_init and module_exit macros
#include <linux/kernel.h>        // Contains kernel macros and functions
#include <linux/fs.h>            // File system related functions and data structures
#include <linux/cdev.h>          // Character device related functions and data structures
#include <linux/vmalloc.h>       // Required for memory allocation functions

#include <linux/sched.h>         // Contains task related functions and data structures

#define MY_MAJOR 415             // Major number for the device
#define MY_MINOR 0               // Minor number for the device
#define DEVICE_NAME "Park_Haley_HW6_DeviceDriver"  // Name for the device

// Global variables
int major, minor;                // Variables to store major and minor numbers
char *kernel_buffer = NULL;      // Pointer to store data in kernel space

struct cdev my_cdev;             // Character device structure
int actual_rx_size = 0;          // Variable to store the actual size of received data

MODULE_AUTHOR("Haley Park");     // Author information
MODULE_DESCRIPTION("A simple encryption program");  // Module description
MODULE_LICENSE("GPL");           // License information

#define ENCRYPT_MODE 1           // Encryption mode flag
#define DECRYPT_MODE 2           // Decryption mode flag
#define KEY 4                    // Encryption/Decryption key

// Data structure for keeping track of read/write counts and encryption mode
struct myds {
    int count;                   // Count of read/write operations
    int encryption_mode;         // Mode for encryption or decryption
} myds;

// Function to handle write operation
ssize_t myWrite(struct file *fs, const char *buf, size_t count, loff_t *f_pos)
{   
    struct myds *ds;             // Pointer to myds structure
    int not_copied;              // Variable to store number of bytes not copied

    // Get the pointer to myds structure from private_data
    ds = (struct myds *)fs->private_data;
    ds->count = ds->count + 1;   // Increment the count of write operations

    // Free previously allocated memory and allocate new memory for kernel buffer
    if (kernel_buffer != NULL)
        kfree(kernel_buffer);

    // Allocate memory for kernel buffer
    if ((kernel_buffer = kmalloc(count + 1, GFP_KERNEL)) == NULL)
        return -ENOMEM;

    // Copy data from user space to kernel space
    not_copied = copy_from_user(kernel_buffer, buf, count);

    // Print debug information
    printk("[%s] count = %ld, not_copied = %u. r/w number = %d\n", __func__, count, not_copied, ds->count);

    actual_rx_size = count - not_copied;  // Update actual received size

    return not_copied;
}

// Function to encrypt or decrypt data
static int encryptOrDecrypt(char *buf, struct myds *ds) {
    int i;                        // Loop variable

    // Switch statement to determine encryption or decryption mode
    switch (ds->encryption_mode) {
        case ENCRYPT_MODE:
            // Encryption: Subtract key value from each character
            for (i = 0; buf[i] != '\0'; i++) {
                buf[i] -= KEY;
            }
            break;
        case DECRYPT_MODE:
            // Decryption: Add key value to each character
            for (i = 0; buf[i] != '\0'; i++) {
                buf[i] += KEY;
            }
            break;
        default:
            printk(KERN_ERR "Failed to encrypt or decrypt.\n");
            return -1;
    }
    buf[i] = '\0';               // Null-terminate the string
    return 0;
}

// Function to handle read operation
ssize_t myRead(struct file *fs, char *buf, size_t count, loff_t *fpos)
{ 
    struct myds *ds;             // Pointer to myds structure
    int not_copied;              // Variable to store number of bytes not copied
    
    // Get the pointer to myds structure from private_data
    ds = (struct myds *)fs->private_data;
    ds->count = ds->count + 1;   // Increment the count of read operations

    // Check if kernel buffer is empty
    if (kernel_buffer == NULL)
        return -1;

    // Adjust count if it exceeds actual received size
    if (count > actual_rx_size)
        count = actual_rx_size;

    // Encrypt or decrypt data before copying to user space
    encryptOrDecrypt(kernel_buffer, ds);
    not_copied = copy_to_user(buf, kernel_buffer, count);

    // Print debug information
    printk("[%s] count = %ld, not_copied = %u. r/w number = %d\n", __func__, count, not_copied, ds->count);

    return not_copied;
}

// Function to handle open operation
static int myOpen(struct inode *inode, struct file *fs) {
    struct myds *ds;             // Pointer to myds structure

    // Allocate memory for myds structure
    ds = vmalloc(sizeof(struct myds));

    // Check if memory allocation is successful
    if (ds == 0) {
        printk(KERN_ERR "Can not vmalloc, File not opened.\n");
        return -1;
    }

    ds->count = 0;                // Initialize count of read/write operations
    ds->encryption_mode = ENCRYPT_MODE;  // Set encryption mode by default
    fs->private_data = ds;        // Store myds structure in private_data
    return 0;
}

// Function to handle close operation
static int myClose(struct inode *inode, struct file *fs) {
    struct myds *ds;             // Pointer to myds structure

    ds = (struct myds *)fs->private_data;  // Get myds structure from private_data
    vfree(ds);                   // Free memory allocated for myds structure
    return 0;
}

// Function to handle IOCTL commands
static long myIoCtl(struct file *fs, unsigned int cmd, unsigned long arg) {
    struct myds *ds;             // Pointer to myds structure

    // Get myds structure from private_data
    ds = (struct myds *)fs->private_data;

    // Switch statement to handle different IOCTL commands
    switch (arg) {
        case ENCRYPT_MODE:
            // Set encryption mode
            ds->encryption_mode = arg;
            printk("encryption mode: %d\n", ds->encryption_mode);
            break;
        case DECRYPT_MODE:
            // Set decryption mode
            ds->encryption_mode = arg;
            printk("decryption mode: %d\n", ds->encryption_mode);
            break;
        default:
            return -EINVAL;       // Return error for invalid command
    }
    return 0;
}

// File operations structure
struct file_operations fops = {
    .open = myOpen,
    .release = myClose,
    .write = myWrite,
    .read = myRead,
    .unlocked_ioctl = myIoCtl,
    .owner = THIS_MODULE,
};

// Initialization function
int init_module(void) {
    int result, registers;        // Variables to store result of registration

    dev_t devno;                  // Device number variable

    devno = MKDEV(MY_MAJOR, MY_MINOR);  // Create device number

    // Register character device region
    registers = register_chrdev_region(devno, 1, DEVICE_NAME);
    printk(KERN_INFO "Register chardev succeeded 1: %d\n", registers);

    // Initialize the character device
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    // Add character device to the system
    result = cdev_add(&my_cdev, devno, 1);
    printk(KERN_INFO "Register chardev succeeded 2: %d\n", result);
    printk(KERN_INFO "Welcome - Driver is loaded.\n");

    // Print error message if registration fails
    if (result < 0) {
        printk(KERN_ERR "Register chardev failed: %d\n", result);
    }

    return result;                // Return result of registration
}

// Cleanup function
void cleanup_module(void) {
    dev_t devno;                  // Device number variable

    devno = MKDEV(MY_MAJOR, MY_MINOR);  // Create device number

    // Unregister character device region
    unregister_chrdev_region(devno, 1);

    // Delete the character device from the system
    cdev_del(&my_cdev);

    printk(KERN_INFO "Goodbye from Driver!\n");  // Print exit message
}
