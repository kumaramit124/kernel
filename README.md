# Kernel Work Intro
	This repo is intented to highlight the kernel work done.

## Raw Flash device driver and filesystem porting
	In this project i have done raw onenand flash driver porting from higher kernel version to lower kernel version.
	1. This involves the required changes in MTD added new onenand layer and crossponding required changes in MTD subsystem. 
	2. Implement the onenand driver initialization routine, also add entry in MTD subsystem makefile and Kconfig enty.
	3. Porting the flash supported filesystem jffs2 from higher version of kernel and mitigate the core kernel fs subsystem changes.
	4. Utilize the opensource MTD test package, which has many utilies in it to verify the raw flash read/write/erease routine.  

# Intro of char driver
## HW access (MMIO, gpiolib)
## Intro to driver model
## Frameworks
## Buses and device tree

# Device driver are abstraction to a piece of HW.
	Though we have device driver running in userspace (via some kernel interface UIO or I2CDEV, SPIDEV),
	Providing an infrastructure to write and run device driver is part of an OS kernel responsibility.
	In Linux, a file is most coomon abstruction.
	File (user space) --> Device Driver (Kernel) --> HW Device (HW)
	Device nodes are one of the interfaces that could be used, where files are exported to users in /dev in that from of char or block device files.

## These device files have accociated thee basic information:
	- Type (block or char)
	- Major number
	- Minor number

# Implementing A Char Driver:
	There are 3 steps to implement the char driver
	Step 1: Allocate the driver number(major/minor). This can be done with register_chardev_region or alloc_chardev_region
	Step 2: Implement file operation (open, read, write, ioctl etc)
	Step 3: Register the char driver in the kernel with cdev_init and cdev_add

# HW Interface:
	- Depending on the HW arch there are few mechanisum a CPU can use to communicate with HW
   		- Port I/O a dedicated bus is used to communicate with HW
    		- Memory mapped I/O the memory address space is shared with HW.
	- MMIO is currently the most common approach adopted by propular arch like ARM.
	- CPU-->MMU-->Memory

# How to talk to a MMIO device:
	Step 1: request access to MMIO register using the few kernel API like request_mem_region
	Step 2: Map the register physical address to virtual address using the function like ioremap
	Step 3: Use the kernel API to read from and write to registers with func like readl and writel

# Driver Model:
	- The driver model provides servel abstraction to device driver to make the code more modular, reusable and easy to maintain.
	- Among its components we have,
   	- Frameworks: the interface exported by a type of class of devices is standardized.
        - The most common frameworks - input, IIO, ALSA, V2L2, RTC, watchdog etc.
   	- Buses: information about the device and where they are connected is abstracted away from the driver.

# How to use the framework:
	Step 1: Initialize an structure of type led_classdev
	Step 2: Provide a callback function to chnage the status of led
	Step 3: Register the driver in framework with the function led_classdev_register

# Bus Infrastructure:
	- Bus Core
	- Bus adapter
	- Bus drivers
	- Bus devices

