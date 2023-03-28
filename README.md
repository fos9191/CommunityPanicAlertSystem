# Project 2 - Community Panic Alert System

### by Fiona Eguare & Santosh Venkataraman (Group 13)

This README contains the setup, compilation and testing guidelines for our project 2 submission. In short, our idea is to provide a service that allows civilians to alert those around them if they feel that they are in danger.
This program is written in C and makes use of basic C libraries as listed below. Enjoy!!!

## Specifications

- This program must be run in WSL or on an Ubuntu machine
- WSL 1 is required to run this program across multiple devices on Windows
- CC or GCC compiler needed


## Libraries

The libraries required to run this program are as follows:

- stdio.h
- stdlib.h
- string.h
- unistd.h
- arpa/inet.h
- pthread.h

## How to Setup

- This program will come set up to be run in up to 6 terminals on one device, thus all hosts recorded in the tracker will have the local host IP address
- If you wish to run this program with multiple devices as hosts you must edit the IP addresses in the tracker file to the IP addresses of the host machines
  - To retrieve the IP address of a device run the command " hostname -I " in the WSL/ Ubuntu terminal
- If you wish to change the number of hosts, simply edit the tracker.txt file, adding or removing entries, but maintaining the formatting (Port IP\n)
  - ensure there is a new line after the last entry
- If you wish to differentiate between each hosts message, edit the string assigned on line 46 before compiling each host. This is not necessary

## How to Compile

- This program can be compiled like any standard C program using pThreads
- Ensure you are in the right directory, have all necessary libraries
- For CC: cc -o panic project2.c -pthread
- For GCC:  gcc -o panic project2.c -pthread

## How to Test

- When running the program, ensure to provide the hosts port number and the hosts IP address as arguments (in this order)
  - For example: ./panic 12000 127.0.0.1
- Port numbers can be randomly selected but must match those selected in tracker.txt and must be paired with the same IP addresses
- Each host can only broadcast 1 message before being restarted as this suits the use of the program
- The program will keep running until you close it from the terminal (shift + c) so it can keep listening for alerts, as this suits the use of the program
- When all hosts are running, each terminal should look like this:
    ~~~
    [+] Creating receiver thread
    [+] Creating sender thread

    ~~~
- At this point you can input 'PANIC' into any one of the terminals and press enter
- This will broadcast a placeholder alert message to all hosts on the network
  - If you wish to personalise this message to each host see the last point in How to Setup
- terminal outputs starting with [-] indicate errors


## References

1. YouTube video by Idiot Developer - [File Transfer using UDP Socket in C | Socket Programming](https://www.google.com/search?q=udp+file+in+c&source=lnms&tbm=vid&sa=X&ved=2ahUKEwjcqqu0kbT9AhUPTMAKHXCIDb0Q_AUoAnoECAEQBA&biw=1366&bih=649&dpr=1#fpstate=ive&vld=cid:a0bb07e0,vid:cXT324fFzaM)
2. [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/#what-is-a-socket) by Brian “Beej Jorgensen” Hall, February 11, 2023
3. Pthreads implemented as taught in module CSU23016-202223 Concurrent Systems And Operating Systems by Andrew Butterfield