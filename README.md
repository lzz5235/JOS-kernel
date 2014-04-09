JOS-kernel
==========

#####kernel-design

> This lab1 is split into three parts. The first part concentrates on getting familiarized with x86 assembly language, 
the QEMU x86 emulator, and the PC's power-on bootstrap procedure. 
The second part examines the boot loader for our 6.828 kernel, which resides in the boot directory of the lab tree. 
Finally, the third part delves into the 
initial template for our 6.828 kernel itself, named JOS, which resides in the kernel directory.


* In this lab2, you will write the memory management code for your operating system. Memory management has two components.

> The first component is a physical memory allocator for the kernel, so that the kernel can allocate memory and later free it. Your allocator will operate in units of 4096 bytes, called pages. Your task will be to maintain data structures that record which physical pages are free and which are allocated, and how many processes are sharing each allocated page. You will also write the routines to allocate and free pages of memory.

> The second component of memory management is virtual memory, which maps the virtual addresses used by kernel and user software to addresses in physical memory. The x86 hardware's memory management unit (MMU) performs the mapping when instructions use memory, consulting a set of page tables. You will modify JOS to set up the MMU's page tables according to a specification we provide.

* In this lab3 you will implement the basic kernel facilities required to get a protected user-mode environment (i.e., "process") running. You will enhance the JOS kernel to set up the data structures to keep track of user environments, create a single user environment, load a program image into it, and start it running. You will also make the JOS kernel capable of handling any system calls the user environment makes and handling any other exceptions it causes.

* In this lab4 you will implement preemptive multitasking among multiple simultaneously active user-mode environments.

> In part A you will add multiprocessor support to JOS, implement round-robin scheduling, and add basic environment management system calls (calls that create and destroy environments, and allocate/map memory).

> In part B, you will implement a Unix-like fork(), which allows a user-mode environment to create copies of itself.

> Finally, in part C you will add support for inter-process communication (IPC), allowing different user-mode environments to communicate and synchronize with each other explicitly. You will also add support for hardware clock interrupts and preemption.

* In this lab5, you will implement spawn, a library call that loads and runs on-disk executables. You will then flesh out your kernel and library operating system enough to run a shell on the console.
