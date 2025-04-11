#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#define max 50


int main(){
    int sockfd,newsockfd,k;
    socklen_t clen;
    int r;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[max], temp[max];
    sockfd = socket(AF_INET, SOCK_DGRAM,0);
    if (sockfd==-1){
        printf("\n socket creation error");
        exit(0);
    }

    serveraddr.sin_family= AF_INET;
    serveraddr.sin_port=htons(3200);
    serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    r= bind (sockfd,(struct sockaddr *)& serveraddr, sizeof(serveraddr));
    if (r==-1){
        printf(" binding error");
        close (sockfd);
        exit(0);
    }

    clen = sizeof(clientaddr);

    while(1){
        memset (buff,0,max); //clear buffer
        r= recvfrom(sockfd,buff, sizeof(buff), 0, (struct sockaddr *)&clientaddr, &clen);
        if (r==-1){
            printf("not received");
            close (sockfd);
            exit(0);

        }
        printf("Received: %s \n", buff);
        r= sendto (sockfd, buff , sizeof (buff), 0, (struct sockaddr *)& clientaddr, clen);
        if (r == -1) {
            perror("Send failed");
            close(sockfd);
            exit(1);
        }

        if (strcmp(buff, "exit") == 0) {
            printf("Exit signal received. Closing server.\n");
            break;
        }
    }


    close (sockfd);


}