# Project 3 - Community Panic Alert System

### by Fiona Eguare, Fionnán O'Sullivan, Aaron McCann & Santosh Venkataraman (Group 11)

This README contains the setup, compilation and testing guidelines for our project 3 submission. In short, our idea is to provide a service that allows civilians to alert those around them if they feel that they are in danger.
This program is written in C and makes use of basic C libraries as listed below. Enjoy!!!

## Specifications

- This program must be run in WSL or on an Linux machine
- WSL 1 is required to run this program across multiple devices on Windows
- CC or GCC compiler needed


## Libraries

The libraries required to run this program are as follows:

- stdio.h
- stdlib.h
- string.h
- unistd.h
- time.h
- arpa/inet.h
- pthread.h

## How to Setup

- To setup this network, simply clone this repo onto each device
- If you wish to run multiple hosts on the same device, we suggest you place each copy of the repo in a seperate folder to avoid the different hosts writing to the same file
- The program is set to have Raspberry Pi 41 as the tracker distributor. If you wish to change this, just update the IP addresses on line 269 in project3.c & 88 in TrackerServer.c

## How to Compile

- The main program can be compiled like any standard C program using pThreads
- The tracker distributor code can be compiled just like normal
- Ensure you are in the right directory, have all necessary libraries
- Tracker Distributor:
  - For CC: ```cc TrackerServer.c -o track```
  - For GCC: ```gcc TrackerServer.c -o track```
- Connecting Host:
  - For CC: ```cc -o panic project3.c -pthread```
  - For GCC: ```gcc -o panic project3.c -pthread```

## How to Test

- When running the program, ensure to provide the hosts port number and the hosts IP address as arguments (in this order)
  - For example: ```./panic 33001 10.35.70.42```
  - To find your IP address, enter ```hostname -I``` into the terminal
- Port numbers can be randomly selected but must be in the range 33000-33999 to be run on the Raspberry Pi's
- The program will keep running until you close it from the terminal (shift + c) so it can keep listening for alerts, as this suits the use of the program
- At this point you can input 'PANIC1' into any one of the non-tracker-distributor terminals and press enter
- This will broadcast an alert message to all hosts on the network
- Alternatively, for a more urgent alert, enter 'PANIC2'. This will send more alerts to each host on the network
- To close the sender thread, enter 'END'
- Terminal outputs starting with [-] indicate errors


## References

1. YouTube video by Idiot Developer - [File Transfer using UDP Socket in C | Socket Programming](https://www.google.com/search?q=udp+file+in+c&source=lnms&tbm=vid&sa=X&ved=2ahUKEwjcqqu0kbT9AhUPTMAKHXCIDb0Q_AUoAnoECAEQBA&biw=1366&bih=649&dpr=1#fpstate=ive&vld=cid:a0bb07e0,vid:cXT324fFzaM)
2. [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/#what-is-a-socket) by Brian “Beej Jorgensen” Hall, February 11, 2023
3. Pthreads implemented as taught in module CSU23016-202223 Concurrent Systems And Operating Systems by Andrew Butterfield
