# Kernel Work Intro

This repository highlights work related to the Linux kernel, specifically focusing on device driver structures, kernel modules, and related filesystem porting.

## Project Description

This project explores the Linux device driver structures with example code. It includes porting a raw OneNAND flash device driver and filesystem, building kernel modules, and interacting with hardware interfaces. The examples demonstrate how to implement and manage device drivers within the Linux kernel environment.


## Raw Flash Device Driver and Filesystem Porting

In this project, I have ported the raw OneNAND flash driver from a higher kernel version to a lower kernel version.

1. Made required changes in MTD by adding a new OneNAND layer and corresponding adjustments in the MTD subsystem.
2. Implemented the OneNAND driver initialization routine, added an entry in the MTD subsystem Makefile, and updated the Kconfig entry.
3. Ported the flash-supported filesystem (JFFS2) from a higher kernel version, addressing changes in the core kernel filesystem subsystem.
4. Utilized the open-source MTD test package, which includes utilities for verifying raw flash read/write/erase routines.

## Introduction to Character Device Driver

- **HW Access (MMIO, gpiolib)**
- **Introduction to Driver Model**
- **Frameworks**
- **Buses and Device Tree**

Device drivers are abstractions for hardware. Although device drivers can run in userspace (via kernel interfaces such as UIO, I2CDEV, SPIDEV), providing an infrastructure to write and run device drivers is part of an OS kernel's responsibility.

In Linux, a file is the most common abstraction:

- **File (user space)** → **Device Driver (Kernel)** → **HW Device (Hardware)**

Device nodes provide an interface for users, typically in `/dev`, in the form of character or block device files. These device files have three basic attributes:
- **Type**: Block or character
- **Major number**
- **Minor number**

### Implementing a Character Device Driver

Three steps are required to implement a character device driver:

1. Allocate the driver number (major/minor). This can be done with `register_chardev_region` or `alloc_chardev_region`.
2. Implement file operations (open, read, write, ioctl, etc.).
3. Register the character driver in the kernel with `cdev_init` and `cdev_add`.

### Hardware Interface

Depending on the hardware architecture, a CPU can communicate with hardware through a few mechanisms:

- **Port I/O**: A dedicated bus for communication with hardware.
- **Memory-Mapped I/O (MMIO)**: The memory address space is shared with hardware. MMIO is commonly used by popular architectures like ARM.

To interact with an MMIO device:
1. Request access to MMIO registers using APIs such as `request_mem_region`.
2. Map the register physical address to a virtual address with `ioremap`.
3. Use kernel APIs to read from and write to registers, such as `readl` and `writel`.

## Driver Model

The driver model provides abstractions to device drivers, making the code modular, reusable, and easier to maintain. Key components include:

- **Frameworks**: Standardized interfaces for device types (e.g., input, IIO, ALSA, V4L2, RTC, watchdog).
- **Buses**: Abstract device connection information away from drivers.

### Using a Framework

1. Initialize a structure of type `led_classdev`.
2. Provide a callback function to change the LED status.
3. Register the driver in the framework with `led_classdev_register`.

### Bus Infrastructure

- **Bus Core**
- **Bus Adapter**
- **Bus Drivers**
- **Bus Devices**

## Sysfs Entry Creation

1. Obtain the handle of `sysfs`.
2. Create a sysfs directory under `/sysfs/kernel/` using `kobject_create_and_add()`.
3. Use a `struct kobject *` to add subdirectories and files.
4. Free the resource once done with `kobject_put()`.

## Important Points

- When building a module outside of the kernel tree that uses `EXPORT_SYMBOL`, the build may not succeed. To build such a module, copy the `Module.symvers`.
 
