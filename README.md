# Linux Kernel Module: Parent-Child Process Memory Management

## Overview
This Linux kernel module creates a **parent-child process tree** where each child process dynamically allocates and frees memory. The module logs process creation, memory allocation, and deallocation in a structured format.

## Features
- Creates **multiple parent processes**, each spawning child processes.
- **Memory allocation and deallocation** tracking using `kmalloc` and `kfree`.
- **Thread synchronization** using mutex locks to ensure orderly logging.
- Displays a **tree-structured output** in `dmesg` logs.


## Installation & Usage

### 1. Compile the Module
```sh
make
```
### 2. Insert the Module
 This loads the kernel module into the Linux kernel.

```sh
sudo insmod child_proc_mem.ko
```
### 3. Verify Module Insertion
Checks if the module is loaded successfully

```sh
lsmod | grep child_proc_mem
```
### 4. View Kernel Logs (Output)
Check the structured output in the kernel logs

```sh
sudo dmesg | tail -n 30
```
### 5. Remove the Module
 Unload the kernel module

```sh
sudo rmmod child_proc_mem
```
### 6. Clean Up
 Remove compiled files

```sh
make clean
```
## Example Output (dmesg)
```sh
[11610.657202] Kernel Module Loaded: Creating Parent-Child Process Tree
[11610.657853] ├── Parent Process: parent_0 (PID: 19734)
[11610.658124]     ├── Child Process: child_0 (PID: 19735)
[11610.658139]     │   ├── Memory Allocated: 0x00000000e499410d
[11611.711549]     │   ├── Memory Freed: 0x00000000e499410d
[11612.037524]     │   ├── Memory Freed: 0x00000000205eacad
```

### License
This project is licensed under the GPL License.

### Author
Developed by sxijyoti
