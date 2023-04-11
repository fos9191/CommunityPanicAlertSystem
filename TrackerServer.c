#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SIZE 1024
#define LINE_SIZE 26
char *filename = "mainTracker.txt";

void send_file(int sockfd) {
    char data[SIZE]  = {0};
    FILE *fp;

    fp = fopen (filename, "r");
    if (!fp) {
        printf("[-] Unable to open mainTracker.txt\n");
        exit(1);
    }

    //send each line of the file to the new node joining
    while(fgets(data, SIZE, fp) != NULL) {
        if (send(sockfd, data, SIZE, 0) == -1) {
            perror("[-] Error in sending data\n");
            exit(1);
        }
        bzero(data, SIZE);
    }
    
    data[0] = 'X'; //terminator to tell client that the file has finished sending

    //send the termintor 'X'
    if (send(sockfd, data, SIZE, 0) == -1) {
        perror("[-] Error in sending terminator\n");
        exit(1);
    }
}

int findLine(char *line) {
    FILE *fp;
    char compareLine[LINE_SIZE] = {0};

    fp = fopen(filename, "r");
    if (!fp) {
        printf("[-] Unable to open mainTracker.txt\n");
        exit(1);
    }

    while (!feof(fp)) {
        fscanf(fp, "%[^\n]\n", compareLine); // Read in a line from mainTracker.txt
        if (!strcmp(compareLine, line))
            return 1; // Return 1 if line found in mainTracker.txt
        bzero(compareLine, LINE_SIZE);
    }
    fclose(fp);
    return 0;
}

void recv_newLine(int sockfd) {
    char newLine[LINE_SIZE]  = {0};
    FILE *fp;
    int found;
    
    if (recv(sockfd, newLine, LINE_SIZE, 0) == -1) {
        perror("[-] Error receiving newLine\n");
        exit(1);
    }
    printf("[+] New host on the network %s\n", newLine);
    
    if (!findLine(newLine)) {
        fp = fopen (filename, "a");
        if (!fp) {
            printf("[-] Unable to open mainTracker.txt\n");
            exit(1);
        }
        fprintf(fp, "%s\n", newLine); // Append line to mainTracker.txt
        printf("[+] Node added to tracker\n");
        fclose(fp); // Close main tracker file once new line appended
    }
    else
        printf("[-] Node already in tracker\n");

    bzero(newLine, LINE_SIZE);
}

int main() {
    char *ip = "10.35.70.41";
    int port = 33333; 
    int e;

    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE] ;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[-] Error creating socket\n");
        exit(1);
    }
    printf("[+] TrackerServer socket created\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (e < 0) {
        perror("[-] Error in binding\n");
        exit(1);
    }
    printf("[+] Binding successful\n");

    while(1) {
        //listen for connections
        e = listen(sockfd, 10);
        if (e == 0) {
            printf("[+] Listening...\n");
        } else {
            perror("[-] Error in listening\n");
            exit(1);
        }

        //accept new connection
        addr_size = sizeof(new_addr);
        new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);

        //send the newest version of the tracker
        send_file(new_sock);
        printf("[+] Data sent successfully\n");

        //receive the port and ip from the new node
        recv_newLine(new_sock);
        printf("[+] New tracker line received successfully\n");

        close(new_sock);
        printf("[+] Disconnected from client\n");
    }

return 0;
}
