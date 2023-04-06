#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define SIZE 100
#define FILESIZE 1024

// This host will send and recieve data through server_sockfd, on port no. server_port
int server_sockfd, server_port;

// To hold the address data of this host
struct sockaddr_in server_addr;

// ----------------------------------------------------------------------
// Function to recieve and output alerts from other hosts on the network
// ----------------------------------------------------------------------
void receive_alerts(int sockfd) {

    int e;               // To check that data is received successfully
    char buffer[SIZE];   // To hold recieved message
    socklen_t addr_size; // To hold size of server_addr
    FILE *track;         // File to hold connection data for all hosts on the network

    while (1) {
        addr_size = sizeof(server_addr);
        bzero(buffer, SIZE); // Clear old data from buffer
       
        // Recieve any data being sent to user and exit with error message if not successful
        e = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr*)&server_addr, &addr_size);
        if (e == -1) {
            printf("[-] Error recieving data\n");
            exit(1);
        }

        if (buffer[0] == 'X')
        {
            memmove(buffer, buffer+1, strlen(buffer));
            track = fopen ("tracker.txt", "a");
            fprintf(track, "%s\n", buffer);
            fclose(track); // Close tracker file once all messages sent
        }

        else
            printf("[+] Recieving data: %s", buffer); // For demonstration purposes
    }
    return; // In practice the server will always be listening so this line is never reached
}

// --------------------------------------------------------------------------
// Function to send alerts to all other hosts on the network on user request
// --------------------------------------------------------------------------
int send_alert(int sockfd) {
    time_t now = time(NULL);                      // fetching time
    
    char* current_time = ctime(&now);
    char alert[50];
    sprintf(alert, "Time of Alert : %s\n\n", current_time); //formats string
    int e, destination_port;                      // To check data sent successfully, and to hold current destination port
    char buffer[SIZE], ip[20];                    // To hold data to be sent, and each IP address read in respectively
    //char alert[] = "time latitude longitude\n\n"; // Place-holder for actual time and location data which will be retrieved by the app
    struct sockaddr_in dest_addr;                 // To hold the address data of the host recieving data at a given time
    FILE *track;                                  // File to hold connection data for all hosts on the network

    if (fgets(buffer, SIZE, stdin) != NULL) {

        // Open tracker file and print error message if unsucessful
        track = fopen("tracker.txt", "r");
        if (!track) {
            printf("[-] Unable to open tracker\n");
            return 0;
        }

        if (!strcmp(buffer, "END\n")){ // If we want to close the connection
            return 1;
        }

         while (!feof(track)) { // Until end of file

                fscanf(track, "%d %s\n", &destination_port, ip); // Read in a hosts port and ip address
                if (server_port != destination_port) {           // If it's not your port...
                   
                    // Set the destination address data...
                    dest_addr.sin_family = AF_INET;
                    dest_addr.sin_port = destination_port;
                    dest_addr.sin_addr.s_addr = inet_addr(ip);
                if (!strcmp(buffer, "PANIC 1\n")){ //less urgent panic sendss once 
                    // Then send the alert message to it. Print error message if send unsuccessful
                    e = sendto(sockfd, alert, strlen(alert), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
                    if (e == -1) {
                        perror("[-] Error sending alert to the server\n");
                        exit(1);
                    }}

                 if (!strcmp(buffer, "PANIC 2\n")){ //more urgent panic sendss once 
                    // Then send the alert message to it. Print error message if send unsuccessful
                    e = sendto(sockfd, alert, strlen(alert), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
                    e = sendto(sockfd, alert, strlen(alert), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
                    e = sendto(sockfd, alert, strlen(alert), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
                    e = sendto(sockfd, alert, strlen(alert), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
                    if (e == -1) {
                        perror("[-] Error sending alert to the server\n");
                        exit(1);
                    }}

                    printf("[+] Alert sent to host on port %d\n", destination_port); // For demonstration purposes
                    bzero(ip, 20); // Clear ip string
                }
            }
            fclose(track); // Close tracker file once all messages sent
    }
    printf("[+] Finished sending\n");
    return 0;    
}

// --------------------------------------------
// Server thread function for recieving alerts
// --------------------------------------------
void *srvr(void *a) {

    receive_alerts(server_sockfd); // Call reciever function

    printf("[+] Exiting reciever thread\n\n"); // For demonstration purposes
    pthread_exit(NULL);                        // Exit thread (never reached but kept for good practice - see line 36)
}

// ------------------------------------------
// Client thread function for sending alerts
// ------------------------------------------
void *clnt(void *a) {
    int state = 0; //while state = 0, keep the connection open
   
    while (state == 0){
        state = send_alert(server_sockfd); // Call sender function
    }
   
    printf("[+] Exiting sender thread\n\n"); // For demostration purposes
    pthread_exit(NULL);                      // Exit thread

}

// -------------------------------------------------------------
// Add port and ip to tracker so other hosts can find this host
// -------------------------------------------------------------
void addToTracker(char *ip, int port, char* line) {

    FILE *track;
    int e, destination_port;        // To check data sent successfully, and to hold current destination port
    char ip_addr[20];
    // char buffer[26];                // *ALT*
    char newHost[27];               // newHost to hold this hosts addition to the tracker
    struct sockaddr_in dest_addr;   // To hold the address data of the host recieving data at a given time

    // Check if node is already in tracker
    track = fopen("tracker.txt", "r");  // Open tracker file to read
    if (!track) {
        printf("[-] Unable to open tracker\n");
        return;
    }
    while (!feof(track)) { // Until end of file
        fscanf(track, "%d %s\n", &destination_port, ip_addr); // Read in a hosts port and ip address
       
        if (port == destination_port) { // If port is equal to a port in the tracker
            if (!strcmp(ip_addr, ip)){  // If IP-address is equal to that ports IP-address
                printf("[-] Node already in tracker\n");
                bzero(ip_addr, 20);     // Clear ip string
                return;                 // Don't add to tracker a second time
            }  
        }

        //       *ALT*
        // fscanf(track, "%s\n", buffer);  // Read in a hosts port and ip address
        // if (!strcmp(line, buffer)) {    // If they are equal to this hosts port and IP-address
        //     printf("[-] Node already in tracker\n");
        //     bzero(buffer, 26);          // Clear buffer
        //     return;                     // Don't add to tracker a second time
        // }  
        // bzero(buffer, 26); // Clear buffer

        bzero(ip_addr, 20); // Clear ip string
    }
    fclose(track);

    // If node is not in tracker, add it
    track = fopen("tracker.txt", "a");
    if (!track) {
        printf("[-] Unable to open tracker\n");
        return;
    }
    fprintf(track, "%d %s\n", port, ip);
    printf("[+] Node added to tracker\n");
    fclose(track); // Close tracker file once all messages sent

    // Also, send it to all other host in tracker to be added
    bzero(newHost, 27);
    newHost[0] = 'X';                       // Format it to be recognised on recieving end
    strncat(newHost, line, strlen(line));   // Add the port no. and ip line
    track = fopen("tracker.txt", "r");
    if (!track) {
        printf("[-] Unable to open tracker\n");
        return;
    }
    while (!feof(track)) { // Until end of file
        fscanf(track, "%d %s\n", &destination_port, ip_addr);   // Read in a hosts port and ip address
        if (server_port != destination_port) {                  // If it's not your port...
            
            // Set the destination address data...
            dest_addr.sin_family = AF_INET;
            dest_addr.sin_port = htons(destination_port);
            dest_addr.sin_addr.s_addr = inet_addr(ip_addr);

            // Then send the alert message to it. Print error message if send unsuccessful
            e = sendto(server_sockfd, newHost, strlen(newHost), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
            if (e == -1) {
                perror("[-] Error sending host data to the server\n");
                exit(1);
            }

            printf("[+] Host data sent to host on port %d\n", destination_port); // For demonstration purposes
            bzero(ip_addr, 20); // Clear ip string
        }
    }
    fclose(track); // Close tracker file once all messages sent
}

// -------------------------------------
// Create and write to new tracker file
// -------------------------------------
void write_file(int sockfd) {
    int n;
    FILE *fp;
    char *filename = "myTracker.txt";
    char buffer[FILESIZE];

    fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("[-]Error in writing file\n");
        exit(1);
    }

    while (1) {
        n = recv(sockfd, buffer, FILESIZE, 0);
        if(n <= 0) {
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, FILESIZE);
    }
    return;
}

// ------------------------------------------
// Get up to date tracker from TrackerServer
// ------------------------------------------
int getTracker() {
    char *ip = "127.0.0.1";
    int port = 8080; 
    int e;

    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[-]Error in socket\n");
        exit(1);
    }
    printf("[+]Server socket created\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip); 

    e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (e == -1) {
        perror("[-]Error in connecting\n");
        exit(1);
    }
    printf("[+]Connected to server\n");

    write_file(sockfd);
    printf("[+]File data sent successfully\n");

    close(sockfd);
    printf("[+]Disconnected from the server\n");

    return 0;
}





// ------------------------------------------------
// Main function to setup and call other functions
// ------------------------------------------------
int main(int argc, const char *argv[]) {

    char ip[20];               // IP address of host
    char ip_copy[20];          // Used to check against tracker file //************do we need this?***********
    static pthread_t rcv, snd; // Declare a receiver and sender thread
    long rc;
    int e;
    char newHost[26], space = ' '; // newHost to hold this hosts addition to the tracker, space for concatenation

    // Set port no. from argument input. Request port no. and ip if not specified
    if (argc > 2) {
        server_port = atoi(argv[1]); // atoi as argument read as char[]
        strcpy(ip, argv[2]);

        strcpy(newHost, argv[1]);
        strncat(newHost, &space, 1);
        strncat(newHost, ip, strlen(ip));
    }
    else {  
        printf("[-] Please specify user port and ip address\n");
        exit(1);
    }

// int h;
//     h = getTracker();
   
    strcpy(ip_copy, ip); //************do we need this?***********
   
    // Create socket
    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sockfd < 0) {
        perror("[-] Socket error\n");
        exit(1);
    }

    // Set up the current host address data
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Bind address data to socket, print error message if unsuccessful
    e = bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (e < 0) {
        perror("[-] Bind error\n");
        exit(1);
    }
    
    // Add this new hosts port and IP to trackers
    addToTracker(ip_copy, server_port, newHost);

    // Create receiver thread, print error message if unsuccessful
    printf("[+] Creating reciever thread\n");
    rc = pthread_create(&rcv, NULL, srvr, NULL);
    if (rc) {
        printf("[-] Error creating reciever thread\n");
        exit(1);
    }

    // Create sender thread, print error message if unsuccessful
    printf("[+] Creating sender thread\n");
    rc = pthread_create(&snd, NULL, clnt, NULL);
    if (rc) {
        printf("[-] Error creating sender thread\n");
        exit(1);
    }

    // Join sender and receiver threads when complete
    pthread_join(rcv, NULL);
    printf("[+] Joined reciever thread\n");
    pthread_join(snd, NULL);
    printf("[+] Joined sender thread\n"); // Never reached but kept for good practice - see line 36

    // Close socket and finish program (never reached but kept for good practice - see line 36)
    close(server_sockfd);
    return 0;
}
