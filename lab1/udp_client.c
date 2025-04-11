#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#define max 50
#include <stdio.h>

int main (){
    int sockfd, r,k,newsockfd;
    socklen_t slen;
    char c;
    char temp [3];
    struct sockaddr_in serveraddr, clientaddr;
    char buff[max];
    char temp1[max];
    sockfd=socket (AF_INET, SOCK_DGRAM,0);
    if (sockfd ==-1){
        printf("\n socket creation error");
        exit (0);

    }

    clientaddr.sin_family= AF_INET;
    clientaddr.sin_port=htons(3201);
    clientaddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3200);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    r= bind (sockfd, (struct sockaddr *)& clientaddr, sizeof(clientaddr));

    if (r==-1){
        close (sockfd);
        exit (0);
    }

    fgets (buff,max,stdin);
    slen= sizeof(serveraddr);

    while (1){
        printf ("Enter msg: ");
        fgets(buff, max,stdin);
        buff[strcspn(buff,"\n")]='\0';
        r= sendto (sockfd, buff, sizeof(buff)+1,0,(struct sockaddr * )& serveraddr, slen);
    
        if (r==-1){
            close (sockfd);
            exit (0);
        }

        // Receive echoed message
        r = recvfrom(sockfd, buff, max, 0, NULL, NULL);
        if (r > 0) {
            printf("Server echoed: %s\n", buff);
        }

        if (strcmp(buff, "exit") == 0) {
            break;
        }
        
    }



    close(sockfd);

}