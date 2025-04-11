/*Write two separate C programs (one for server and other for client) using socket
APIs, to implement the following connection-oriented client-server model.
(a) The user at the client side sends an alphanumeric string to the server.
(b) The child process at the server sorts the numbers of the alphanumeric string
in ascending order. The parent process at the server sorts the characters of the
alphanumeric string in descending order.
(c) Both the processes send the results to the client along with its corresponding
process ID.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/wait.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int main (){
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf ("Enter alphanumeric string: ");
    fgets(buffer, sizeof(buffer),stdin);
     buffer[strcspn(buffer, "\n")] = '\0'; 
    send(sockfd, buffer, sizeof (buffer),0);

        //  VERY SIMPLE METHOD, MAKE SERVER PRINT EVERYTHING, RECEIVER JUST NEEDS TO RECEIVE THEN 
    while (1){
        memset(buffer, 0, sizeof(buffer));
        int n = ecv (sockfd, buffer, sizeof(buffer), 0);
        if (n<=0){
            break;
        }
        printf ("%s",buffer);
    }
    close(sockfd);
    return 0;
}