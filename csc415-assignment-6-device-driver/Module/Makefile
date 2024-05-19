KERNELDIR=/lib/modules/`uname -r`/build

# This the the Makefile for the device driver (not your demonstration program)

#Change the base name here to your module file name - no extension
BASENAME=Park_Haley_HW6_DeviceDriver


MODULES = $(BASENAME).ko 
obj-m += $(BASENAME).o 

all:
	make -C $(KERNELDIR) M=$(PWD) modules

clean:
	make -C $(KERNELDIR) M=$(PWD) clean

